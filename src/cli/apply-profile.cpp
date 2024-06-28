// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

#include <FredEmmott/MonitorTool/Config.hpp>
#include <FredEmmott/MonitorTool/EnumAdapterDescs.hpp>
#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/except.hpp>
#include <winrt/base.h>

#include <algorithm>
#include <format>
#include <optional>
#include <ranges>

#include <Windows.h>
#include <string.h>

using namespace FredEmmott::MonitorTool::Config;
using namespace FredEmmott::MonitorTool::CLI;
using Profile = FredEmmott::MonitorTool::Profile;

namespace {
const auto HelpText = std::format(
  "Freds Monitor Tool v{}\n"
  "\n"
  "USAGE:\n"
  "  fmt-apply-profile [--update] [--path] PROFILE_NAME_OR_PATH\n"
  "  fmt-apply-profile --help\n"
  "\n"
  "OPTIONS:\n"
  "  --path: the following argument is a JSON file path, not a profile name\n"
  "  --update: update the graphics adapter list saved in the profile\n"
  "  --help: show this text\n"
  "---\n"
  "{}",
  VersionString,
  LicenseText);

}// namespace

bool operator==(const LUID& a, const LUID& b) {
  return memcmp(&a, &b, sizeof(LUID)) == 0;
}

static Profile ReplaceLUID(
  const std::optional<LUID>& search,
  const LUID& replace,
  const Profile& in) {
  Profile ret {in};

  for (auto& path: ret.mDisplayConfig.mPaths) {
    if ((!search) || *search == path.sourceInfo.adapterId) {
      path.sourceInfo.adapterId = replace;
    }
    if ((!search) || *search == path.targetInfo.adapterId) {
      path.targetInfo.adapterId = replace;
    }
  }
  for (auto& mode: ret.mDisplayConfig.mModes) {
    if ((!search) || *search == mode.adapterId) {
      mode.adapterId = replace;
    }
  }

  return ret;
}

static bool ApplyAdapterlessProfileWithCurrentSingleGPU(
  const Profile& in,
  const std::vector<DXGI_ADAPTER_DESC1>& allAdapters,
  bool saveUpdates) {
  if (!in.mAdapters.empty()) {
    return false;
  }
  const auto badFlags = DXGI_ADAPTER_FLAG_REMOTE | DXGI_ADAPTER_FLAG_SOFTWARE;
  std::vector<DXGI_ADAPTER_DESC1> realAdapters;
  for (const auto& it: allAdapters) {
    if ((it.Flags & badFlags) == 0) {
      realAdapters.push_back(it);
    }
  }
  if (realAdapters.size() != 1) {
    return false;
  }

  const auto replacement = realAdapters.front().AdapterLuid;
  const auto profile = ReplaceLUID(std::nullopt, replacement, in);

  if (!profile.CanApply()) {
    return false;
  }
  profile.Apply();
  if (saveUpdates) {
    profile.Save();
  }
  return true;
}

int WINAPI wWinMain(
  [[maybe_unused]] HINSTANCE hInstance,
  [[maybe_unused]] HINSTANCE hPrevInstance,
  [[maybe_unused]] PWSTR pCmdLine,
  [[maybe_unused]] int nCmdShow) {
  // Using `GetCommandLineW()` instead of `pCmdLine` as `pCmdLine` varies in
  // whether or not argv[0] is the process, depending on how it's launched.
  int argc {};
  const auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  bool profileNameIsPath = false;
  bool updateProfile = false;
  std::string profileName;

  for (int i = 1; i < argc; ++i) {
    const std::wstring_view arg {argv[i]};
    if (arg.starts_with(L"-")) {
      if (arg == L"--help") {
        PrintCOUT(HelpText);
        return 0;
      }
      if (arg == L"--path") {
        profileNameIsPath = true;
        continue;
      if (arg == L"--update") {
        updateProfile = true;
      }
      }
      PrintCERR(HelpText);
      return 1;
    }

    if (!profileName.empty()) {
      PrintCERR(std::format(
        "Multiple profile names provided.\n"
        "First: {}\nNext: {}\n{}",
        profileName,
        winrt::to_string(arg),
        HelpText));
      return 1;
    }
    profileName = winrt::to_string(arg);
  }

  if (profileName.empty()) {
    PrintCERR(
      std::format("Profile name was empty or not provided\n{}", HelpText));
    return 1;
  }

  try {
    Profile profile {};
    if (profileNameIsPath) {
      profile = Profile::Load(profileName);
    } else {
      const auto profiles = Profile::Enumerate();
      // Try an exact match first, then fall back to case-insensitive
      auto it = std::ranges::find(profiles, profileName, &Profile::mName);
      if (it == profiles.end()) {
        it = std::ranges::find_if(profiles, [a = profileName](const auto& b) {
          return _stricmp(a.c_str(), b.mName.c_str()) == 0;
        });
      }
      if (it == profiles.end()) {
        PrintCERR(
          std::format("Couldn't find a profile called '{}'", profileName));
        return 1;
      }

      profile = *it;
    }

    if (!profile.CanApply()) {
      const auto allAdapters = FredEmmott::MonitorTool::EnumAdapterDescs();
      if (ApplyAdapterlessProfileWithCurrentSingleGPU(profile, allAdapters, updateProfile)) {
        return 0;
      }
      PrintCERR("Profile can't be applied due to a configuration change");
      return 1;
    }
    profile.Apply();
  } catch (const FredEmmott::MonitorTool::RuntimeError& e) {
    PrintCERR(std::format("Fatal error: {}", e.what()).c_str());
    return 1;
  }

  return 0;
}
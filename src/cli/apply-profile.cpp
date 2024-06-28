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

inline bool operator==(const LUID& a, const LUID& b) {
  return memcmp(&a, &b, sizeof(LUID)) == 0;
}

static bool IsLikelySameModelGPU(
  const DXGI_ADAPTER_DESC1& a,
  const DXGI_ADAPTER_DESC1& b) {
  return (a.VendorId == b.VendorId) && (a.DeviceId == b.DeviceId)
    && (a.DedicatedVideoMemory == b.DedicatedVideoMemory)
    && (a.Flags == b.Flags);
}

static std::optional<LUID> GetUpdatedLUID(
  const LUID& in,
  const Profile& profile,
  const std::vector<DXGI_ADAPTER_DESC1> currentAdapters) {
  for (const auto& it: currentAdapters) {
    if (it.AdapterLuid == in) {
      return in;
    }
  }

  if (profile.mAdapters.empty()) {
    return {};
  }

  const auto oldIt = std::ranges::find(
    profile.mAdapters, in, &DXGI_ADAPTER_DESC1::AdapterLuid);
  if (oldIt == profile.mAdapters.end()) {
    return {};
  }

  size_t oldIdxForModel = 0;
  for (const auto& it: profile.mAdapters) {
    if (it.AdapterLuid == in) {
      break;
    }
    if (IsLikelySameModelGPU(it, *oldIt)) {
      ++oldIdxForModel;
    }
  }

  for (const auto& it: currentAdapters) {
    if (IsLikelySameModelGPU(it, *oldIt)) {
      if (oldIdxForModel == 0) {
        return it.AdapterLuid;
      }
      --oldIdxForModel;
    }
  }

  return {};
}

template <>
struct std::hash<LUID> {
  std::size_t operator()(const LUID& v) const noexcept {
    return std::hash<uint64_t> {}(std::bit_cast<uint64_t>(v));
  }
};

const std::optional<Profile> UpdateLUIDs(
  const Profile& in,
  const std::vector<DXGI_ADAPTER_DESC1>& currentAdapters) {
  Profile ret {in};
  ret.mAdapters = currentAdapters;
  std::unordered_map<LUID, LUID> luids;

  auto map = [&](const LUID& luid) -> std::optional<LUID> {
    if (luids.contains(luid)) {
      return luids.at(luid);
    }
    const auto ret = GetUpdatedLUID(luid, in, currentAdapters);
    if (ret) {
      luids.emplace(luid, *ret);
    }
    return ret;
  };

  for (auto& mode: ret.mDisplayConfig.mModes) {
    const auto it = map(mode.adapterId);
    if (!it) {
      return {};
    }

    mode.adapterId = *it;
  }

  for (auto& path: ret.mDisplayConfig.mPaths) {
    {
      const auto it = map(path.sourceInfo.adapterId);
      if (!it) {
        return {};
      }
      path.sourceInfo.adapterId = *it;
    }
    {
      const auto it = map(path.targetInfo.adapterId);
      if (!it) {
        return {};
      }
      path.targetInfo.adapterId = *it;
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
  Profile profile {in};

  for (auto& path: profile.mDisplayConfig.mPaths) {
    path.sourceInfo.adapterId = replacement;
    path.targetInfo.adapterId = replacement;
  }
  for (auto& mode: profile.mDisplayConfig.mModes) {
    mode.adapterId = replacement;
  }

  if (!profile.CanApply()) {
    return false;
  }
  profile.Apply();
  if (saveUpdates) {
    profile.mAdapters = allAdapters;
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
  bool saveUpdates = false;
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
      }
      if (arg == L"--update") {
        saveUpdates = true;
        continue;
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
      if (ApplyAdapterlessProfileWithCurrentSingleGPU(
            profile, allAdapters, saveUpdates)) {
        return 0;
      }
      const auto updated = UpdateLUIDs(profile, allAdapters);
      if (updated && updated->CanApply()) {
        updated->Apply();
        if (saveUpdates) {
          updated->Save();
        }
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
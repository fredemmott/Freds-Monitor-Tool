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
  "  fmt-apply-profile [--update] [--path|--guid] PROFILE_NAME\n"
  "  fmt-apply-profile --help\n"
  "\n"
  "OPTIONS:\n"
  "  --path: the following argument is a JSON file path, not a profile name\n"
  "  --guid: the following argument is a profile GUID, not a profile name\n"
  "  --update: update the graphics adapter list saved in the profile\n"
  "  --help: show this text\n"
  "---\n"
  "{}",
  VersionString,
  LicenseText);

enum class ProfileParamKind {
  ProfileName,
  ProfileGUID,
  FilePath,
};

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

static std::optional<Profile> FindProfileByName(const std::string& name) {
  const auto profiles = Profile::Enumerate();
  // Try an exact match first, then fall back to case-insensitive
  auto it = std::ranges::find(profiles, name, &Profile::mName);
  if (it != profiles.end()) {
    return *it;
  }

  it = std::ranges::find_if(profiles, [a = name](const auto& b) {
    return _stricmp(a.c_str(), b.mName.c_str()) == 0;
  });
  if (it != profiles.end()) {
    return *it;
  }

  return {};
}

static std::optional<Profile> FindProfileByGUID(const std::string& guidStrIn) {
  std::string_view guidStr {guidStrIn};
  if (
    guidStr.size() == 38 && (guidStr.front() == '{')
    && (guidStr.back() == '}')) {
    guidStr.remove_prefix(1);
    guidStr.remove_suffix(1);
  }
  winrt::guid guid;
  try {
    guid = winrt::guid {guidStr};
  } catch (const std::invalid_argument&) {
    return {};
  }

  const auto profiles = Profile::Enumerate();
  auto it = std::ranges::find(profiles, guid, &Profile::mGuid);
  if (it != profiles.end()) {
    return *it;
  }

  return {};
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

  std::optional<ProfileParamKind> profileParamKind;
  bool saveUpdates = false;
  std::string profileParam;

  for (int i = 1; i < argc; ++i) {
    const std::wstring_view arg {argv[i]};
    if (arg.starts_with(L"-")) {
      if (arg == L"--help") {
        PrintCOUT(HelpText);
        return 0;
      }
      if (arg == L"--path") {
        if (profileParamKind) {
          PrintCERR(HelpText);
          return 1;
        }

        profileParamKind = ProfileParamKind::FilePath;
        continue;
      }
      if (arg == L"--guid") {
        if (profileParamKind) {
          PrintCERR(HelpText);
          return 1;
        }

        profileParamKind = ProfileParamKind::ProfileGUID;
        continue;
      }
      if (arg == L"--update") {
        saveUpdates = true;
        continue;
      }
      PrintCERR(HelpText);
      return 1;
    }

    if (!profileParam.empty()) {
      PrintCERR(std::format(
        "Multiple profile names provided.\n"
        "First: {}\nNext: {}\n{}",
        profileParam,
        winrt::to_string(arg),
        HelpText));
      return 1;
    }
    profileParam = winrt::to_string(arg);
  }

  if (profileParam.empty()) {
    PrintCERR(
      std::format("Profile name was empty or not provided\n{}", HelpText));
    return 1;
  }

  try {
    Profile profile {};
    switch (profileParamKind.value_or(ProfileParamKind::ProfileName)) {
      case ProfileParamKind::FilePath:
        profile = Profile::Load(profileParam);
        break;
      case ProfileParamKind::ProfileName: {
        auto it = FindProfileByName(profileParam);
        if (!it) {
          PrintCERR(
            std::format("Couldn't find a profile called '{}'", profileParam));
          return 1;
        }

        profile = *it;
        break;
      }
      case ProfileParamKind::ProfileGUID: {
        auto it = FindProfileByGUID(profileParam);
        if (!it) {
          PrintCERR(std::format(
            "Couldn't find a profile with GUID '{}'", profileParam));
          return 1;
        }
        profile = *it;
        break;
      }
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
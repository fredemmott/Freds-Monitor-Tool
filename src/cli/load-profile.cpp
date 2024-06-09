// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/except.hpp>
#include <winrt/base.h>

#include <algorithm>
#include <format>

#include <Windows.h>

using namespace FredEmmott::MonitorTool::CLI;

namespace {
constexpr char HelpText[] {
  "USAGE: \n"
  "  fmt-load-profile [--path] PROFILE_NAME_OR_PATH\n"
  "  fmt-load-profile --help",
};

}// namespace

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
  }

  try {
    using Profile = FredEmmott::MonitorTool::Profile;
    Profile profile {};
    if (profileNameIsPath) {
      profile = Profile::Load(profileName);
    } else {
      const auto profiles = Profile::Enumerate();
      const auto it = std::ranges::find(profiles, profileName, &Profile::mName);
      if (it == profiles.end()) {
        PrintCERR(std::format("Couldn't find a profile called '{}'", profileName));
        return 1;
      }

      profile = *it;
    }

    if (!profile.CanApply()) {
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
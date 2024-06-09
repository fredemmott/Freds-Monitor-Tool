// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/except.hpp>
#include <winrt/base.h>

#include <vector>

#include <Windows.h>
#include <string.h>

using namespace FredEmmott::MonitorTool::CLI;

namespace {
constexpr char HelpText[] {
  "USAGE: \n"
  "  fmt-create-profile PROFILE_NAME [--path PATH] [--force]\n"
  "  fmt-create-profile --help",
};

void HelpCERR() {
  PrintCERR(HelpText);
}

void HelpCOUT() {
  PrintCOUT(HelpText);
}
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

  bool force = false;
  std::wstring_view profilePath;
  std::string profileName;

  for (int i = 1; i < argc; ++i) {
    const std::wstring_view arg {argv[i]};
    if (arg.starts_with(L"-")) {
      if (arg == L"--help") {
        HelpCOUT();
        return 0;
      }
      if (arg == L"--force") {
        force = true;
        continue;
      }
      if (arg == L"--path") {
        if (i + 1 >= argc) {
          HelpCERR();
          return 1;
        }
        profilePath = {argv[++i]};
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
    if (!force) {
      const auto profiles = Profile::Enumerate();
      const auto it
        = std::ranges::find_if(profiles, [&a = profileName](const auto& b) {
            return _stricmp(a.c_str(), b.mName.c_str()) == 0;
          });

      if (it != profiles.end()) {
        if (HaveConsole()) {
          PrintCERR(std::format(
            "A similarly named profile already exists (`{}`); re-run with "
            "`--force` to create a duplicate.",
            it->mName));
          return 1;
        } else {
          const auto result = MessageBoxA(
            NULL, std::format("A similarly named profile already exists (`{})`; Would you like to create this profile anyway?\nRe-run with `--force` to skip this message in the future.", it->mName).c_str(), "Freds Monitor Tool", MB_ICONWARNING | MB_YESNO);
          if (result != IDYES) {
            return 0;
          }
        }
      }
    }

    const auto profile = Profile::CreateFromActiveConfiguration(profileName);
    if (profilePath.empty()) {
      profile.Save();
    } else {
      profile.Save(profilePath);
    }
  } catch (const FredEmmott::MonitorTool::RuntimeError& e) {
    PrintCERR(std::format("Fatal error: {}", e.what()).c_str());
    return 1;
  }

  return 0;
}
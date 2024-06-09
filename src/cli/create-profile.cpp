// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/except.hpp>
#include <winrt/base.h>
#include "console.hpp"

#include <vector>

#include <Windows.h>

using namespace FredEmmott::MonitorTool::CLI;

namespace {
constexpr char HelpText[] {
  "USAGE: \n"
  "  fmt-create-profile PROFILE_NAME [--path PATH]\n"
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

  std::wstring_view profilePath;
  std::string profileName;

  for (int i = 1; i < argc; ++i) {
    const std::wstring_view arg {argv[i]};
    if (arg.starts_with(L"-")) {
      if (arg == L"--help") {
        HelpCOUT();
        return 0;
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
    const auto profile
      = FredEmmott::MonitorTool::Profile::CreateFromActiveConfiguration(
        profileName);
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
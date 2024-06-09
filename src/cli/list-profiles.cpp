// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

#include <FredEmmott/MonitorTool/Config.hpp>
#include <FredEmmott/MonitorTool/Profile.hpp>

#include <format>

#include <Windows.h>

using namespace FredEmmott::MonitorTool::CLI;
using namespace FredEmmott::MonitorTool::Config;

namespace {
const auto HelpText = std::format(
  "Freds Monitor Tool v{}\n"
  "\n"
  "USAGE: \n"
  "  fmt-list-profiles [--help]\n"
  "\n"
  "---\n"
  "{}",
  VersionString,
  LicenseText);

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

  for (int i = 1; i < argc; ++i) {
    const std::wstring_view arg {argv[i]};
    if (arg == L"--help") {
      PrintCOUT(HelpText);
      return 0;
    }

    PrintCERR(HelpText);
    return 1;
  }

  const auto profiles = FredEmmott::MonitorTool::Profile::Enumerate();
  std::string message;
  if (profiles.empty()) {
    message = "No profiles have been saved yet.";
  } else {
    message = "Profiles:";
    for (const auto& profile: profiles) {
      message += std::format(
        "\n- '{}'\t{}",
        profile.mName,
        winrt::to_string(winrt::to_hstring(profile.mGuid)));
    }
  }

  if (HaveConsole()) {
    std::cout << message << std::endl;
  } else {
    MessageBoxA(
      NULL,
      message.c_str(),
      std::format("Freds Monitor Tool v{}", VersionString).c_str(),
      MB_OK | MB_ICONINFORMATION);
  }
  return 0;
}
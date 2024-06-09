// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

#include <FredEmmott/MonitorTool/Profile.hpp>

#include <Windows.h>
#include <format>

using namespace FredEmmott::MonitorTool::CLI;

int WINAPI wWinMain(
  [[maybe_unused]] HINSTANCE hInstance,
  [[maybe_unused]] HINSTANCE hPrevInstance,
  [[maybe_unused]] PWSTR pCmdLine,
  [[maybe_unused]] int nCmdShow) {
  const auto profiles = FredEmmott::MonitorTool::Profile::Enumerate();
  std::string message;
  if (profiles.empty()) {
    message = "No profiles have been saved yet.";
  } else {
    message = "Profiles:";
    for (const auto& profile: profiles) {
        message += std::format("\n- '{}'\t{}", profile.mName, winrt::to_string(winrt::to_hstring(profile.mGuid)));
    }
  }

  if (HaveConsole()) {
    std::cout << message << std::endl;
  }  else {
    MessageBoxA(NULL, message.c_str(), "Freds Monitor Tool", MB_OK | MB_ICONINFORMATION);
  }
  return 0;
}
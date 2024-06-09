// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <FredEmmott/MonitorTool/Config.hpp>

#include <format>
#include <iostream>

#include <Windows.h>

namespace FredEmmott::MonitorTool::CLI {

bool AttachToParentConsole();
bool HaveConsole();

void PrintCERR(auto message) {
  if (HaveConsole()) {
    std::cerr << message << std::endl;
  } else {
    const std::string buf {message};
    OutputDebugStringA(buf.c_str());
    MessageBoxA(
      NULL,
      buf.c_str(),
      std::format("Freds Monitor Tool v{}", Config::VersionString).c_str(),
      MB_ICONERROR | MB_OK);
  }
}

void PrintCOUT(auto message) {
  if (HaveConsole()) {
    std::cout << message << std::endl;
  } else {
    OutputDebugStringA(std::string(message).c_str());
  }
}

}// namespace FredEmmott::MonitorTool::CLI
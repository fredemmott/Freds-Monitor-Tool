// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <iostream>
#include <Windows.h>

namespace FredEmmott::MonitorTool::CLI {

bool AttachToParentConsole();
bool HaveConsole();

void PrintCERR(auto message) {
  if (HaveConsole()) {
    std::cerr << message << std::endl;
  } else {
    const auto buf = std::string(message);
    OutputDebugStringA(buf.c_str());
    MessageBoxA(NULL, buf.c_str(), "Freds Monitor Tool", MB_ICONERROR | MB_OK);
  }
}

void PrintCOUT(auto message) {
  if (HaveConsole()) {
    std::cout << message << std::endl;
  } else {
    OutputDebugStringA(message);
  }
}

}
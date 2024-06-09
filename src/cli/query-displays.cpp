// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/except.hpp>

#include <iostream>
#include <vector>

#include <Windows.h>

int main(int argc, char** argv) {
  try {
    const auto profile = FredEmmott::MonitorTool::Profile::CreateFromActiveConfiguration("Active");
  } catch (const FredEmmott::MonitorTool::RuntimeError& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

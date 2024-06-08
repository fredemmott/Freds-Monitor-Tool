// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/QueryDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/Profile.hpp>

#include <iostream>
#include <vector>

#include <Windows.h>

int main(int argc, char** argv) {
  try {
    const FredEmmott::MonitorTool::Profile profile {
      .mName = "3 Displays",
      .mDisplayConfig = FredEmmott::MonitorTool::QueryDisplayConfig(),
    };
    profile.Save(std::filesystem::path { "3 displays.json" });
  } catch (const FredEmmott::MonitorTool::RuntimeError& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

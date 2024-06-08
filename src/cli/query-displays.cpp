// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/QueryDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/json.hpp>

#include <iostream>
#include <vector>

#include <Windows.h>

int main(int argc, char** argv) {
  try {
    const auto [paths, modes] = FredEmmott::MonitorTool::QueryDisplayConfig();
    const nlohmann::json j {
      {"paths", paths},
      {"modes", modes},
    };

    std::cout << j.dump(2) << std::endl;
  } catch (const FredEmmott::MonitorTool::RuntimeError& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

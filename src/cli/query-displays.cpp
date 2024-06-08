// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/json.hpp>

#include <iostream>
#include <vector>

#include <Windows.h>

int main(int argc, char** argv) {
  std::vector<DISPLAYCONFIG_PATH_INFO> paths;
  std::vector<DISPLAYCONFIG_MODE_INFO> modes;

  const UINT32 flags = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE
    | QDC_VIRTUAL_REFRESH_RATE_AWARE;

  // QueryDisplayConfig
  do {
    UINT32 numPaths {};
    UINT32 numModes {};
    if (
      GetDisplayConfigBufferSizes(flags, &numPaths, &numModes)
      != ERROR_SUCCESS) {
      return 1;
    }
    paths.resize(numPaths);
    modes.resize(numModes);

    const auto result = QueryDisplayConfig(
      flags, &numPaths, paths.data(), &numModes, modes.data(), nullptr);
    if (result != ERROR_SUCCESS) {
      paths.clear();
      modes.clear();
      continue;
    }

  } while (paths.empty() || modes.empty());

  const nlohmann::json j {
    {"paths", paths},
    {"modes", modes},
  };

  std::cout << j.dump(2) << std::endl;

  return 0;
}

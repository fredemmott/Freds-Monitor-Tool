// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#include <FredEmmott/MonitorTool/QueryDisplayConfig.hpp>
#include <stdexcept>
#include <format>

namespace FredEmmott::MonitorTool {

DisplayConfig QueryDisplayConfig(uint32_t flags)
{
    
  std::vector<DISPLAYCONFIG_PATH_INFO> paths;
  std::vector<DISPLAYCONFIG_MODE_INFO> modes;

  // QueryDisplayConfig
  unsigned int tries = 0;
  do {
    UINT32 numPaths {};
    UINT32 numModes {};

    auto result = GetDisplayConfigBufferSizes(flags, &numPaths, &numModes);
    if (result != ERROR_SUCCESS) {
        throw GetDisplayConfigBufferSizesError(
            std::format("GetDisplayConfigBufferSizes() failed with error {}", result));
    }
    paths.resize(numPaths);
    modes.resize(numModes);

    result = QueryDisplayConfig(
      flags, &numPaths, paths.data(), &numModes, modes.data(), nullptr);
      if (result == ERROR_SUCCESS) {
        return {paths, modes};
      }
  } while (++tries < 5);
  throw QueryDisplayConfigError("QueryDisplayConfig() failed 5 times");
}
}
// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/SetDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/except.hpp>

#include <format>

namespace FredEmmott::MonitorTool {

void SetDisplayConfig(const DisplayConfig& config, UINT32 flags) {
  // Copy as `::SetDisplayConfig()` takes non-const pointers
  auto paths = config.mPaths;
  auto modes = config.mModes;

  const auto result = ::SetDisplayConfig(
    paths.size(), paths.data(), modes.size(), modes.data(), flags);
  if (result != ERROR_SUCCESS) {
    throw RuntimeError(
      std::format("SetDisplayConfig() failed with {}", result));
  }
}

}// namespace FredEmmott::MonitorTool
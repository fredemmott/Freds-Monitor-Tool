// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "except.hpp"

#include <vector>

#include <Windows.h>

namespace FredEmmott::MonitorTool {

struct DisplayConfig {
  std::vector<DISPLAYCONFIG_PATH_INFO> mPaths;
  std::vector<DISPLAYCONFIG_MODE_INFO> mModes;
};

constexpr UINT32 DefaultQueryDisplayConfigFlags
  = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE | QDC_VIRTUAL_MODE_AWARE;

class GetDisplayConfigBufferSizesError final : public RuntimeError {
    using RuntimeError::RuntimeError;
};
class QueryDisplayConfigError final : public RuntimeError {
    using RuntimeError::RuntimeError;
};

DisplayConfig QueryDisplayConfig(
  uint32_t flags = DefaultQueryDisplayConfigFlags);

}// namespace FredEmmott::MonitorTool
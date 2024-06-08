// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"

#include <Windows.h>

namespace FredEmmott::MonitorTool {

constexpr UINT32 SetDisplayConfigDefaultFlags = SDC_APPLY
  | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_TOPOLOGY_SUPPLIED
  | SDC_VIRTUAL_MODE_AWARE | SDC_VIRTUAL_REFRESH_RATE_AWARE | SDC_VALIDATE;

void SetDisplayConfig(
    const DisplayConfig& config,
    UINT32 flags = SetDisplayConfigDefaultFlags);

}
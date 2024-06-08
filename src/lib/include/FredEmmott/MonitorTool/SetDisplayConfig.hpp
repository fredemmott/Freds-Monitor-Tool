// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"

#include <Windows.h>

namespace FredEmmott::MonitorTool {

constexpr UINT32 SetDisplayConfigBaseFlags = SDC_USE_SUPPLIED_DISPLAY_CONFIG
  | SDC_VIRTUAL_MODE_AWARE
  | SDC_VIRTUAL_REFRESH_RATE_AWARE;
constexpr UINT32 SetDisplayConfigValidateFlags = SetDisplayConfigBaseFlags | SDC_VALIDATE;
constexpr UINT32 SetDisplayConfigApplyFlags = SetDisplayConfigBaseFlags | SDC_APPLY;
constexpr UINT32 SetDisplayConfigDefaultFlags = SetDisplayConfigApplyFlags;

void SetDisplayConfig(
  const DisplayConfig& config,
  UINT32 flags = SetDisplayConfigApplyFlags);

}// namespace FredEmmott::MonitorTool
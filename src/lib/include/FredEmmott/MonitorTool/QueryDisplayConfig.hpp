// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"
#include "except.hpp"

#include <Windows.h>

namespace FredEmmott::MonitorTool {

constexpr UINT32 QueryDisplayConfigDefaultFlags
  = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE | QDC_VIRTUAL_MODE_AWARE;

class GetDisplayConfigBufferSizesError final : public RuntimeError {
  using RuntimeError::RuntimeError;
};
class QueryDisplayConfigError final : public RuntimeError {
  using RuntimeError::RuntimeError;
};

DisplayConfig QueryDisplayConfig(
  uint32_t flags = QueryDisplayConfigDefaultFlags);

}// namespace FredEmmott::MonitorTool
// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <vector>

#include <Windows.h>
#include <dxgi.h>

namespace FredEmmott::MonitorTool {

std::vector<DXGI_ADAPTER_DESC1> EnumAdapterDescs();

}
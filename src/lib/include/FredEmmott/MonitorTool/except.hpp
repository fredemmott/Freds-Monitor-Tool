// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <stdexcept>

namespace FredEmmott::MonitorTool {

class RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

}// namespace FredEmmott::MonitorTool

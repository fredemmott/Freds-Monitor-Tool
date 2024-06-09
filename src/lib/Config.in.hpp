// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <string>

namespace FredEmmott::MonitorTool {

inline namespace Config {

constexpr std::string_view VersionString { "@VERSION_STRING@" };

constexpr std::string_view LicenseText {R"---LICENSE---(
@LICENSE_TEXT@
)---LICENSE---"};

}

}// namespace FredEmmott::MonitorTool
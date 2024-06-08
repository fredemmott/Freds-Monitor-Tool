// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once
#include <algorithm>
#include <cstring>
#include <vector>

#include <Windows.h>

namespace FredEmmott::MonitorTool {

struct DisplayConfig {
  std::vector<DISPLAYCONFIG_PATH_INFO> mPaths;
  std::vector<DISPLAYCONFIG_MODE_INFO> mModes;

  inline bool operator==(const DisplayConfig& other) const noexcept {
    return std::ranges::equal(
             mPaths,
             other.mPaths,
             [](
               const DISPLAYCONFIG_PATH_INFO& a,
               const DISPLAYCONFIG_PATH_INFO& b) {
               return memcmp(&a, &b, sizeof(a)) == 0;
             })
      && std::ranges::equal(
             mModes,
             other.mModes,
             [](
               const DISPLAYCONFIG_MODE_INFO& a,
               const DISPLAYCONFIG_MODE_INFO& b) {
               return memcmp(&a, &b, sizeof(a)) == 0;
             });
  }
};

}// namespace FredEmmott::MonitorTool
// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace FredEmmott::MonitorTool {
struct Profile final {
  static Profile Load(const std::filesystem::path& path);
  void Save(const std::filesystem::path& path) const;

  static std::vector<Profile> Enumerate();

  std::filesystem::path mPath;

  std::string mName;
  DisplayConfig mDisplayConfig;

};
}// namespace FredEmmott::MonitorTool
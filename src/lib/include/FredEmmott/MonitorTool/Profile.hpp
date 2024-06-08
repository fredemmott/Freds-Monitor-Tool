// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"
#include "except.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace FredEmmott::MonitorTool {

/// Failed to open a file
class FileOpenError final : public RuntimeError {
 public:
  using RuntimeError::RuntimeError;
};

class FileWriteError final : public RuntimeError {
 public:
  using RuntimeError::RuntimeError;
};

class FileReadError final : public RuntimeError {
 public:
  using RuntimeError::RuntimeError;
};

class DisplayConfigValidationError final : public RuntimeError {
  public:
  using RuntimeError::RuntimeError;
};

struct Profile final {
  static Profile CreateFromActiveConfiguration(const std::string& name);

  static Profile Load(const std::filesystem::path& path);
  void Save(const std::filesystem::path& path) const;

  static std::vector<Profile> Enumerate();

  // Can throw DisplayConfigValidation
  bool CanApply() const;
  void Apply() const;

  std::filesystem::path mPath;

  std::string mName;
  DisplayConfig mDisplayConfig;
};
}// namespace FredEmmott::MonitorTool
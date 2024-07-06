// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include "DisplayConfig.hpp"
#include "except.hpp"

#include <winrt/base.h>

#include <filesystem>
#include <string>
#include <vector>

#include <dxgi.h>

namespace FredEmmott::MonitorTool {

enum class ApplyMode {
  Temporary,
  Persistent,
};

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
  /* Saves to the same path it was loaded from, or the user's profile store if
   * it's not yet been saved. */
  void Save() const;

  static std::vector<Profile> Enumerate();

  // Can throw DisplayConfigValidation
  bool CanApply() const;
  void Apply(ApplyMode) const;

  std::string mName;
  std::vector<DXGI_ADAPTER_DESC1> mAdapters;
  DisplayConfig mDisplayConfig;

  // Automatically filled
  winrt::guid mGuid;

  // Automatically filled by `Load()` and `Enumerate()`
  std::filesystem::path mPath;
};
}// namespace FredEmmott::MonitorTool
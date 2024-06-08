// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/QueryDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/json.hpp>
#include <winrt/base.h>

#include <Windows.h>

namespace FredEmmott::MonitorTool {

void Profile::Save(const std::filesystem::path& path) const {
  const nlohmann::json j {
    {"Name", mName},
    {"Paths", mDisplayConfig.mPaths},
    {"Modes", mDisplayConfig.mModes},
  };

  const auto json = j.dump(2);

  // Remove MAX_PATH limitation
  const auto fullPath = L"\\\\?\\" + std::filesystem::absolute(path).wstring();

  winrt::file_handle file {CreateFileW(
    fullPath.c_str(),
    GENERIC_WRITE,
    0,
    nullptr,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    NULL)};
  if (!file) {
    // TODO: throw error
    __debugbreak();
  }

  const auto totalBytes = json.size();
  DWORD bytesWritten = 0;
  while (bytesWritten < totalBytes) {
    DWORD bytesThisLoop = 0;
    if (!WriteFile(
          file.get(),
          json.data() + bytesWritten,
          totalBytes - bytesWritten,
          &bytesThisLoop,
          nullptr)) {
      // TODO
      __debugbreak();
    }
    bytesWritten += bytesThisLoop;
  }
}

Profile Profile::Load(const std::filesystem::path& path) {
  // Remove MAX_PATH limitation
  const auto fullPath = L"\\\\?\\" + std::filesystem::absolute(path).wstring();
  winrt::file_handle file {CreateFileW(
    fullPath.c_str(),
    GENERIC_READ,
    0,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL)};
  if (!file) {
    // TODO: throw error
    __debugbreak();
  }
  const auto fileSize = GetFileSize(file.get(), nullptr);
  std::string buffer(fileSize, '\0');
  DWORD bytesRead = 0;
  while (bytesRead < fileSize) {
    DWORD bytesThisLoop = 0;
    if (!ReadFile(
          file.get(),
          buffer.data() + bytesRead,
          fileSize - bytesRead,
          &bytesThisLoop,
          nullptr)) {
      // TODO
      __debugbreak();
    }
    bytesRead += bytesRead;
  }
  const auto j = nlohmann::json::parse(buffer);
  return {
        .mPath = path,
        .mName = j.at("Name"),
        .mDisplayConfig = {
            .mPaths = j.at("Paths"),
            .mModes = j.at("Modes"),
        },
    };
}

Profile Profile::CreateFromActiveConfiguration(const std::string& name) {
  return {
    .mName = name,
    .mDisplayConfig = QueryDisplayConfig(),
  };
}

}// namespace FredEmmott::MonitorTool
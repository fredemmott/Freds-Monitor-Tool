// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <FredEmmott/MonitorTool/Profile.hpp>
#include <FredEmmott/MonitorTool/EnumAdapterDescs.hpp>
#include <FredEmmott/MonitorTool/QueryDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/SetDisplayConfig.hpp>
#include <FredEmmott/MonitorTool/json.hpp>
#include <winrt/base.h>

#include <ShlObj.h>
#include <Windows.h>

NLOHMANN_JSON_NAMESPACE_BEGIN
template <>
struct adl_serializer<winrt::guid> {
  static void from_json(const nlohmann::json& j, winrt::guid& v) {
    auto s = j.get<std::string_view>();
    if (s.size() == 38 && (s.front() == '{') && (s.back() == '}')) {
      s.remove_prefix(1);
      s.remove_suffix(1);
    }
    v = winrt::guid {s};
  }

  static void to_json(nlohmann::json& j, const winrt::guid& v) {
    j = winrt::to_string(winrt::to_hstring(v));
  }
};
NLOHMANN_JSON_NAMESPACE_END

namespace FredEmmott::MonitorTool {

namespace {
std::filesystem::path RunOnce_GetProfilesPath() {
  PWSTR pathStr {nullptr};

  winrt::check_hresult(SHGetKnownFolderPath(
    FOLDERID_LocalAppData, KF_FLAG_DEFAULT | KF_FLAG_CREATE, NULL, &pathStr));
  if (!pathStr) {
    return {};
  }
  const auto path
    = std::filesystem::path(pathStr) / "Freds Monitor Tool" / "Profiles";
  CoTaskMemFree(pathStr);
  return path;
}
const auto ProfilesPath = RunOnce_GetProfilesPath();

winrt::guid CreateRandomGUID() {
  GUID ret;
  winrt::check_hresult(CoCreateGuid(&ret));
  return std::bit_cast<winrt::guid>(ret);
}
}// namespace

void Profile::Save(const std::filesystem::path& path) const {
  const auto parent = path.parent_path();
  if (!std::filesystem::exists(parent)) {
    std::filesystem::create_directories(parent);
  }

  const nlohmann::json j {
    {"GUID", mGuid},
    {"Name", mName},
    {"Adapters", mAdapters},
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
    const auto ec = GetLastError();
    throw FileOpenError(
      std::format("Failed to open `{}`: {}", winrt::to_string(fullPath), ec));
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
      throw FileWriteError(
        std::format("Failed to write to file: {}", GetLastError()));
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
    const auto ec = GetLastError();
    throw FileOpenError(
      std::format("Failed to open `{}`: {}", winrt::to_string(fullPath), ec));
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
      throw FileReadError(
        std::format("Failed to read from file: {}", GetLastError()));
    }
    bytesRead += bytesThisLoop;
  }
  const auto j = nlohmann::json::parse(buffer);
  return {
        .mName = j.at("Name"),
        .mAdapters = j.value("Adapters", std::vector<DXGI_ADAPTER_DESC1>{}),
        .mDisplayConfig = {
            .mPaths = j.at("Paths"),
            .mModes = j.at("Modes"),
        },
        .mGuid = j.at("GUID"),
        .mPath = path,
    };
}

Profile Profile::CreateFromActiveConfiguration(const std::string& name) {
  return {
    .mName = name,
    .mAdapters = EnumAdapterDescs(),
    .mDisplayConfig = QueryDisplayConfig(),
    .mGuid = CreateRandomGUID(),
  };
}

std::vector<Profile> Profile::Enumerate() {
  if (!std::filesystem::is_directory(ProfilesPath)) {
    return {};
  }

  std::vector<Profile> ret;
  for (auto&& entry: std::filesystem::directory_iterator(ProfilesPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }
    if (entry.path().extension() != ".json") {
      continue;
    }
    ret.push_back(Profile::Load(entry.path()));
  }
  return ret;
}

bool Profile::CanApply() const {
  try {
    SetDisplayConfig(mDisplayConfig, SetDisplayConfigValidateFlags);
    return true;
  } catch (...) {
    return false;
  }
}

void Profile::Apply() const {
  try {
    SetDisplayConfig(mDisplayConfig, SetDisplayConfigValidateFlags);
  } catch (const RuntimeError& e) {
    throw DisplayConfigValidationError(
      std::format("Validation failed: {}", e.what()));
  }
  SetDisplayConfig(mDisplayConfig, SetDisplayConfigApplyFlags);
}

void Profile::Save() const {
  if (!mPath.empty()) {
    this->Save(mPath);
    return;
  }

  const auto profiles = Profile::Enumerate();
  const auto matching = std::ranges::find(profiles, mGuid, &Profile::mGuid);
  if (matching != profiles.end()) {
    this->Save(matching->mPath);
    return;
  }

  // Pick absolutely known-safe chars only
  std::string basename;
  for (const char it: mName) {
    if (
      (it >= 'a' && it <= 'z') || (it >= 'A' && it <= 'Z')
      || (it >= '0' && it <= '9') || (it == ' ') || (it == '-')
      || (it == '_')) {
      basename += it;
    }
  }

  const auto path = ProfilesPath / (basename + ".json");
  if (!std::filesystem::exists(path)) {
    this->Save(path);
    return;
  }

  uint16_t i = 1;
  while (true) {
    const auto path = ProfilesPath / std::format("{}-{:04x}.json", basename, i);
    if (std::filesystem::exists(path)) {
      continue;
    }
    this->Save(path);
    return;
  }
}

}// namespace FredEmmott::MonitorTool
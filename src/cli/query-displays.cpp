// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include <nlohmann/json.hpp>

#include <bit>
#include <iostream>
#include <vector>

#include <Windows.h>

void from_json(const nlohmann::json& j, LUID& v) {
  static_assert(sizeof(uint64_t) == sizeof(LUID));
  v = std::bit_cast<LUID>(j.get<uint64_t>());
}

void to_json(nlohmann::json& j, const LUID& v) {
  static_assert(sizeof(uint64_t) == sizeof(LUID));
  j = std::bit_cast<uint64_t>(v);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_RATIONAL,
  Numerator,
  Denominator)

///// START DISPLAYCONFIG_PATH_SOURCE_INFO /////
// Not using the NLOHMANN macros because they take references, which aren't
// permitted for bitfields
#define XFIELDS \
  X(adapterId) \
  X(id) \
  X(cloneGroupId) \
  X(sourceModeInfoIdx) \
  X(statusFlags)

void from_json(const nlohmann::json& j, DISPLAYCONFIG_PATH_SOURCE_INFO& v) {
#define X(FIELD) v.FIELD = j.at(#FIELD);
  XFIELDS
#undef X
}

void to_json(nlohmann::json& j, const DISPLAYCONFIG_PATH_SOURCE_INFO& v) {
#define X(FIELD) {#FIELD, v.FIELD},
  j.update({XFIELDS});
#undef X
}

#undef XFIELDS
///// END DISPLAYCONFIG_PATH_SOURCE_INFO /////

///// START DISPLAYCONFIG_PATH_TARGET_INFO /////
#define XFIELDS \
  X(adapterId) \
  X(id) \
  X(desktopModeInfoIdx) \
  X(targetModeInfoIdx) \
  X(outputTechnology) \
  X(rotation) \
  X(scaling) \
  X(refreshRate) \
  X(scanLineOrdering) \
  X(targetAvailable) \
  X(statusFlags)

void from_json(const nlohmann::json& j, DISPLAYCONFIG_PATH_TARGET_INFO& v) {
#define X(FIELD) v.FIELD = j.at(#FIELD);
  XFIELDS
#undef X
}

void to_json(nlohmann::json& j, const DISPLAYCONFIG_PATH_TARGET_INFO& v) {
#define X(FIELD) {#FIELD, v.FIELD},
  j.update({XFIELDS});
#undef X
}

#undef XFIELDS
///// END DISPLAYCONFIG_PATH_TARGET_INFO /////

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DISPLAYCONFIG_2DREGION, cx, cy)

static_assert(
  sizeof(DISPLAYCONFIG_VIDEO_SIGNAL_INFO::AdditionalSignalInfo)
  == sizeof(DISPLAYCONFIG_VIDEO_SIGNAL_INFO::videoStandard));
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_VIDEO_SIGNAL_INFO,
  pixelRate,
  hSyncFreq,
  vSyncFreq,
  activeSize,
  totalSize,
  videoStandard,
  scanLineOrdering);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_TARGET_MODE,
  targetVideoSignalInfo)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(POINTL, x, y)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_SOURCE_MODE,
  width,
  height,
  pixelFormat,
  position)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RECTL, left, top, right, bottom);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_DESKTOP_IMAGE_INFO,
  PathSourceSize,
  DesktopImageRegion,
  DesktopImageClip)

///// START DISPLAYCONFIG_MODE_INFO /////
#define XFIELDS \
  X(infoType) \
  X(id) \
  X(adapterId)

void from_json(const nlohmann::json& j, DISPLAYCONFIG_MODE_INFO& v) {
#define X(FIELD) v.FIELD = j.at(#FIELD);
  XFIELDS
#undef X
  switch (j.at("infoType").get<DISPLAYCONFIG_MODE_INFO_TYPE>()) {
    case DISPLAYCONFIG_MODE_INFO_TYPE_TARGET:
      v.targetMode = j.at("targetMode");
      break;
    case DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE:
      v.sourceMode = j.at("sourceMode");
      break;
    case DISPLAYCONFIG_MODE_INFO_TYPE_DESKTOP_IMAGE:
      v.desktopImageInfo = j.at("desktopImageInfo");
      break;
    default:
      break;
  }
}

void to_json(nlohmann::json& j, const DISPLAYCONFIG_MODE_INFO& v) {
#define X(FIELD) {#FIELD, v.FIELD},
  j.update({XFIELDS});
#undef X

  switch (v.infoType) {
    case DISPLAYCONFIG_MODE_INFO_TYPE_TARGET:
      j["targetMode"] = v.targetMode;
      break;
    case DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE:
      j["sourceMode"] = v.sourceMode;
      break;
    case DISPLAYCONFIG_MODE_INFO_TYPE_DESKTOP_IMAGE:
      j["desktopImageInfo"] = v.desktopImageInfo;
      break;
    default:
      break;
  }
}

#undef XFIELDS
///// END DISPLAYCONFIG_MODE_INFO /////

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_PATH_INFO,
  sourceInfo,
  targetInfo,
  flags);

int main(int argc, char** argv) {
  std::vector<DISPLAYCONFIG_PATH_INFO> paths;
  std::vector<DISPLAYCONFIG_MODE_INFO> modes;

  const UINT32 flags = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE
    | QDC_VIRTUAL_REFRESH_RATE_AWARE;

  // QueryDisplayConfig
  do {
    UINT32 numPaths {};
    UINT32 numModes {};
    if (
      GetDisplayConfigBufferSizes(flags, &numPaths, &numModes)
      != ERROR_SUCCESS) {
      return 1;
    }
    paths.resize(numPaths);
    modes.resize(numModes);

    const auto result = QueryDisplayConfig(
      flags, &numPaths, paths.data(), &numModes, modes.data(), nullptr);
    if (result != ERROR_SUCCESS) {
      paths.clear();
      modes.clear();
      continue;
    }

  } while (paths.empty() || modes.empty());

  const nlohmann::json j {
    {"paths", paths},
    {"modes", modes},
  };

  std::cout << j.dump(2) << std::endl;

  return 0;
}

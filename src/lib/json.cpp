// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#include <FredEmmott/MonitorTool/json.hpp>

#include <bit>

void from_json(const nlohmann::json& j, LUID& v) {
  static_assert(sizeof(uint64_t) == sizeof(LUID));
  v = std::bit_cast<LUID>(j.get<uint64_t>());
}

void to_json(nlohmann::json& j, const LUID& v) {
  static_assert(sizeof(uint64_t) == sizeof(LUID));
  j = std::bit_cast<uint64_t>(v);
}

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
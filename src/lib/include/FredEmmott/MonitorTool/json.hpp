// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#pragma once

#include <nlohmann/json.hpp>

#include <Windows.h>
#include <dxgi.h>

void from_json(const nlohmann::json& j, LUID& v);
void to_json(nlohmann::json& j, const LUID& v);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(POINTL, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RECTL, left, top, right, bottom);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_RATIONAL,
  Numerator,
  Denominator)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DISPLAYCONFIG_2DREGION, cx, cy)

void from_json(const nlohmann::json& j, DISPLAYCONFIG_PATH_SOURCE_INFO& v);
void to_json(nlohmann::json& j, const DISPLAYCONFIG_PATH_SOURCE_INFO& v);

void from_json(const nlohmann::json& j, DISPLAYCONFIG_PATH_TARGET_INFO& v);
void to_json(nlohmann::json& j, const DISPLAYCONFIG_PATH_TARGET_INFO& v);

void from_json(const nlohmann::json&, DXGI_ADAPTER_DESC1&);
void to_json(nlohmann::json&, const DXGI_ADAPTER_DESC1&);

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_SOURCE_MODE,
  width,
  height,
  pixelFormat,
  position)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_DESKTOP_IMAGE_INFO,
  PathSourceSize,
  DesktopImageRegion,
  DesktopImageClip)

void from_json(const nlohmann::json& j, DISPLAYCONFIG_MODE_INFO& v);
void to_json(nlohmann::json& j, const DISPLAYCONFIG_MODE_INFO& v);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  DISPLAYCONFIG_PATH_INFO,
  sourceInfo,
  targetInfo,
  flags);
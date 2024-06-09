// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC

#include "console.hpp"

namespace FredEmmott::MonitorTool::CLI {

bool AttachToParentConsole() {
  static bool sAttached;
  static std::once_flag sOnce;
  std::call_once(sOnce, [attached = &sAttached]() {
    *attached = AttachConsole(ATTACH_PARENT_PROCESS);
    if (!*attached) {
      return;
    }
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "w", stdin);
  });
  return sAttached;
}

bool HaveConsole() {
  static bool sHaveConsole;
  static std::once_flag sOnce;
  std::call_once(sOnce, [v = &sHaveConsole]() {
    *v = (!!GetStdHandle(STD_ERROR_HANDLE)) || AttachToParentConsole();
  });
  return sHaveConsole;
}

}
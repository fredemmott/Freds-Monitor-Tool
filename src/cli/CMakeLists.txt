set(
  REDIST_TARGETS
  fmt-create-profile
  fmt-apply-profile
  fmt-list-profiles
)

add_library(
  FredEmmott_MonitorTool_console
  STATIC
  console.cpp
)

add_executable(
  fmt-create-profile
  WIN32
  create-profile.cpp
)
target_link_libraries(
  fmt-create-profile
  FredEmmott_MonitorTool_Config
  FredEmmott_MonitorTool_Profile
  FredEmmott_MonitorTool_console
)

add_executable(
  fmt-apply-profile
  WIN32
  apply-profile.cpp
)
target_link_libraries(
  fmt-apply-profile
  FredEmmott_MonitorTool_Config
  FredEmmott_MonitorTool_Profile
  FredEmmott_MonitorTool_console
)

add_executable(
  fmt-list-profiles
  WIN32
  list-profiles.cpp
)
target_link_libraries(
  fmt-list-profiles
  FredEmmott_MonitorTool_Config
  FredEmmott_MonitorTool_Profile
  FredEmmott_MonitorTool_console
)

include(GNUInstallDirs)

foreach(TARGET IN LISTS REDIST_TARGETS)
  sign_target(${TARGET})
endforeach()
install(TARGETS ${REDIST_TARGETS})

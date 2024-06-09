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
target_link_libraries(
  FredEmmott_MonitorTool_console
  PUBLIC
  FredEmmott_MonitorTool_Config
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

set(VERSION_RC "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/version.in.rc"
  "${VERSION_RC}"
  @ONLY
  NEWLINE_STYLE UNIX
)

foreach(TARGET IN LISTS REDIST_TARGETS)
  sign_target(${TARGET})
  target_sources("${TARGET}" PRIVATE manifest.xml "${VERSION_RC}")
endforeach()
install(TARGETS ${REDIST_TARGETS} DESTINATION ".")

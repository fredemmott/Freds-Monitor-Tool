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
  FredEmmott_MonitorTool_Profile
  FredEmmott_MonitorTool_console
)
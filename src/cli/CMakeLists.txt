add_executable(
  query-displays
  query-displays.cpp
)
target_link_libraries(
  query-displays
  FredEmmott_MonitorTool_Profile
)


add_executable(
  fmt-create-profile
  WIN32
  create-profile.cpp
)
target_link_libraries(
  fmt-create-profile
  FredEmmott_MonitorTool_Profile
)
add_executable(
  query-displays
  query-displays.cpp
)
target_link_libraries(
  query-displays
  FredEmmott_MonitorTool_json
  FredEmmott_MonitorTool_QueryDisplayConfig)

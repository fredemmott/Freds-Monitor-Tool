add_executable(
  query-displays
  query-displays.cpp
)
find_package(nlohmann_json)
target_link_libraries(query-displays PRIVATE nlohmann_json::nlohmann_json)

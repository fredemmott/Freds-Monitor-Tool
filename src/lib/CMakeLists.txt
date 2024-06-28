set(CODEGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
file(READ "../../LICENSE" LICENSE_TEXT)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.in.hpp"
    "${CODEGEN_BUILD_DIR}/include/FredEmmott/MonitorTool/Config.hpp"
    @ONLY
    NEWLINE_STYLE UNIX
)

add_library(
    FredEmmott_MonitorTool_Config
    INTERFACE
)
target_include_directories(
    FredEmmott_MonitorTool_Config
    INTERFACE
    "${CODEGEN_BUILD_DIR}/include"
)

add_library(
    FredEmmott_MonitorTool_json
    STATIC
    json.cpp
)
target_include_directories(FredEmmott_MonitorTool_json
    PUBLIC
    include
)
find_package(nlohmann_json)
target_link_libraries(
    FredEmmott_MonitorTool_json
    PUBLIC
    nlohmann_json::nlohmann_json
)

add_library(
    FredEmmott_MonitorTool_QueryDisplayConfig
    STATIC
    QueryDisplayConfig.cpp
)
target_include_directories(
    FredEmmott_MonitorTool_QueryDisplayConfig
    PUBLIC
    include
)

add_library(
    FredEmmott_MonitorTool_SetDisplayConfig
    STATIC
    SetDisplayConfig.cpp
)
target_include_directories(
    FredEmmott_MonitorTool_SetDisplayConfig
    PUBLIC
    include
)

add_library(System::DXGI INTERFACE IMPORTED)
set_property(
    TARGET System::DXGI
    PROPERTY IMPORTED_LIBNAME DXGI
)

add_library(
    FredEmmott_MonitorTool_EnumAdapterDescs
    STATIC
    EnumAdapterDescs.cpp
)
target_include_directories(
    FredEmmott_MonitorTool_EnumAdapterDescs
    PUBLIC
    include
)
target_link_libraries(
    FredEmmott_MonitorTool_EnumAdapterDescs
    PUBLIC
    System::DXGI
)

add_library(
    FredEmmott_MonitorTool_Profile
    STATIC
    Profile.cpp
)
target_include_directories(
    FredEmmott_MonitorTool_Profile
    PUBLIC
    include
)
target_link_libraries(
    FredEmmott_MonitorTool_Profile
    PRIVATE
    FredEmmott_MonitorTool_EnumAdapterDescs
    FredEmmott_MonitorTool_QueryDisplayConfig
    FredEmmott_MonitorTool_SetDisplayConfig
    FredEmmott_MonitorTool_json
)
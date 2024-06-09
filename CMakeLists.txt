cmake_minimum_required(VERSION 3.28)

# No specific need for this version, but let's set *something* so
# the lower bound is known and enforced
set(MINIMUM_WINDOWS_VERSION "10.0.19041.0")
set(CMAKE_SYSTEM_VERSION "${MINIMUM_WINDOWS_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
  # Standard C++ exception behavior
  "/EHsc"
)

# Require that targets exist
cmake_policy(SET CMP0079 NEW)
set(CMAKE_LINK_LIBRARIES_ONLY_TARGETS ON)
cmake_policy(SET CMP0028 NEW)

# Enable CMAKE_MSVC_RUNTIME_LIBRARY variable
cmake_policy(SET CMP0091 NEW)

# Set extracted file timestamp to extract time
cmake_policy(SET CMP0135 NEW)

set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

if("${VCPKG_TARGET_TRIPLET}" MATCHES "-static$")
  # https://github.com/microsoft/WindowsAppSDK/blob/main/docs/Coding-Guidelines/HybridCRT.md
  set(
    CMAKE_MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>"
  )
  add_link_options(
    "/DEFAULTLIB:ucrt$<$<CONFIG:Debug>:d>.lib" # include the dynamic UCRT
    "/NODEFAULTLIB:libucrt$<$<CONFIG:Debug>:d>.lib" # remove the static UCRT
  )
endif()

set(
  CMAKE_TOOLCHAIN_FILE
  "${CMAKE_CURRENT_SOURCE_DIR}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file"
)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(VERSION_BUILD $ENV{GITHUB_RUN_NUMBER})
else()
  set(VERSION_BUILD 0)
endif()

project(
  freds-monitor-tool
  VERSION 0.0.1.${VERSION_BUILD}
  LANGUAGES CXX C)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(VERSION_STRING "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}+GHA${VERSION_BUILD}")
else()
  set(VERSION_STRING "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}+local")
endif()

message(STATUS "${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION}")

# Handy for CI
file(WRITE "${CMAKE_BINARY_DIR}/version.txt" "${CMAKE_PROJECT_VERSION}")

if(MSVC AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CLANG_CL 1 CACHE BOOL "Whether we're using clang-cl")
else()
  set(CLANG_CL 0)
endif()

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Simulated compiler: ${CMAKE_CXX_SIMULATE_ID}")
message(STATUS "MSVC: ${MSVC}")
message(STATUS "CLANG_CL: ${CLANG_CL}")

set(COMMON_COMPILE_OPTIONS "/DUNICODE" "/D_UNICODE")

if(MSVC AND NOT CLANG_CL)
  list(
    APPEND COMMON_COMPILE_OPTIONS

    # Standards-compliant C++20 coroutines
    "/await:strict"

    # Synchronous writes to PDB files in case building with parallel CL.exe
    "/FS"

    # Include content and marker in error messages
    "/diagnostics:caret"
  )
endif()

add_compile_options(${COMMON_COMPILE_OPTIONS})

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME Default)

include(sign_target.cmake)

add_subdirectory("src")

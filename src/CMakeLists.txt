add_subdirectory(lib)

option(BUILD_CLI "Build the CLI utilities" ${PROJECT_IS_TOP_LEVEL})
if (${BUILD_CLI})
  add_subdirectory(cli)
endif()
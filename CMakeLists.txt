cmake_minimum_required(VERSION 3.20)

project(template
  VERSION 0.1.0
  DESCRIPTION "Template rendering engine for Vix.cpp"
  LANGUAGES CXX
)

# ============================================================================
# Options
# ============================================================================

option(TEMPLATE_BUILD_TESTS "Build template tests" ON)
option(TEMPLATE_BUILD_EXAMPLES "Build template examples" ON)
option(TEMPLATE_BUILD_BENCHMARKS "Build template benchmarks" OFF)
option(TEMPLATE_INSTALL "Enable install rules" ON)

# ============================================================================
# C++ standard
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Default build type
# ============================================================================

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ============================================================================
# Library
# ============================================================================

add_library(template
  src/Engine.cpp
  src/Environment.cpp
  src/Template.cpp
  src/Value.cpp
  src/Context.cpp
  src/Error.cpp
  src/Loader.cpp
  src/FileSystemLoader.cpp
  src/StringLoader.cpp
  src/Lexer.cpp
  src/Parser.cpp
  src/Compiler.cpp
  src/Renderer.cpp
  src/Builtins.cpp
  src/Escape.cpp
  src/Cache.cpp
)

add_library(vix::template ALIAS template)

target_include_directories(template
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(template PUBLIC cxx_std_20)

# ============================================================================
# Warnings
# ============================================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(template PRIVATE
    -Wall
    -Wextra
    -Wpedantic
  )
elseif(MSVC)
  target_compile_options(template PRIVATE
    /W4
  )
endif()

# ============================================================================
# Version macros
# ============================================================================

target_compile_definitions(template
  PUBLIC
    TEMPLATE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    TEMPLATE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    TEMPLATE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# ============================================================================
# Examples
# ============================================================================

if(TEMPLATE_BUILD_EXAMPLES)
  add_executable(template_basic_render examples/basic_render.cpp)
  target_link_libraries(template_basic_render PRIVATE vix::template)

  add_executable(template_render_file examples/render_file.cpp)
  target_link_libraries(template_render_file PRIVATE vix::template)

  add_executable(template_layout_inheritance examples/layout_inheritance.cpp)
  target_link_libraries(template_layout_inheritance PRIVATE vix::template)

  add_executable(template_includes examples/includes.cpp)
  target_link_libraries(template_includes PRIVATE vix::template)

  add_executable(template_loops_and_conditions examples/loops_and_conditions.cpp)
  target_link_libraries(template_loops_and_conditions PRIVATE vix::template)

  add_executable(template_filters examples/filters.cpp)
  target_link_libraries(template_filters PRIVATE vix::template)
endif()

# ============================================================================
# Tests
# ============================================================================

if(TEMPLATE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ============================================================================
# Benchmarks
# ============================================================================

if(TEMPLATE_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

# ============================================================================
# Install
# ============================================================================

if(TEMPLATE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(
    TARGETS template
    EXPORT templateTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(
    EXPORT templateTargets
    FILE templateTargets.cmake
    NAMESPACE vix::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/template
  )

  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/templateConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/template
  )

  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/templateConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/templateConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/templateConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/template
  )
endif()

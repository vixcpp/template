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
option(TEMPLATE_BUILD_BENCHMARKS "Build template benchmarks" ON)
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
  src/Builtins.cpp
  src/Cache.cpp
  src/Compiler.cpp
  src/Context.cpp
  src/Engine.cpp
  src/Environment.cpp
  src/Error.cpp
  src/Escape.cpp
  src/FileSystemLoader.cpp
  src/Lexer.cpp
  src/Loader.cpp
  src/Parser.cpp
  src/Renderer.cpp
  src/StringLoader.cpp
  src/Template.cpp
  src/Value.cpp
  src/Optimizer.cpp
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
  target_compile_options(template
    PRIVATE
      -Wall
      -Wextra
      -Wpedantic
  )
elseif(MSVC)
  target_compile_options(template
    PRIVATE
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
  add_subdirectory(examples)
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
  find_package(benchmark QUIET)

  if(benchmark_FOUND)
    add_subdirectory(benchmarks)
  else()
    message(STATUS "Google Benchmark not found, benchmarks will not be built")
  endif()
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

cmake_minimum_required(VERSION 3.20)

if(NOT TARGET vix::template)
  message(FATAL_ERROR "Target vix::template not found")
endif()

if(NOT TARGET benchmark::benchmark)
  message(FATAL_ERROR "Target benchmark::benchmark not found")
endif()

add_executable(template_render_bench
  render_bench.cpp
)

target_link_libraries(template_render_bench
  PRIVATE
    vix::template
    benchmark::benchmark
)

target_compile_features(template_render_bench PRIVATE cxx_std_20)

add_executable(template_parse_bench
  parse_bench.cpp
)

target_link_libraries(template_parse_bench
  PRIVATE
    vix::template
)

target_compile_features(template_parse_bench PRIVATE cxx_std_20)

add_executable(template_cache_bench
  cache_bench.cpp
)

target_link_libraries(template_cache_bench
  PRIVATE
    vix::template
)

target_compile_features(template_cache_bench PRIVATE cxx_std_20)

set_target_properties(
  template_render_bench
  template_parse_bench
  template_cache_bench
  PROPERTIES
    FOLDER "benchmarks"
)

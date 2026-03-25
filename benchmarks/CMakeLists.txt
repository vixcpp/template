cmake_minimum_required(VERSION 3.20)

if(NOT TARGET vix::template)
  message(FATAL_ERROR "Target vix::template not found")
endif()

add_executable(template_render_bench
  render_bench.cpp
)

target_link_libraries(template_render_bench
  PRIVATE
    vix::template
)

add_executable(template_parse_bench
  parse_bench.cpp
)

target_link_libraries(template_parse_bench
  PRIVATE
    vix::template
)

add_executable(template_cache_bench
  cache_bench.cpp
)

target_link_libraries(template_cache_bench
  PRIVATE
    vix::template
)

set_target_properties(
  template_render_bench
  template_parse_bench
  template_cache_bench
  PROPERTIES
    FOLDER "benchmarks"
)

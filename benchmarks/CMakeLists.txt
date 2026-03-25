cmake_minimum_required(VERSION 3.20)

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

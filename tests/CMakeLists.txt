cmake_minimum_required(VERSION 3.20)

add_executable(template_lexer_test
  lexer_test.cpp
)

target_link_libraries(template_lexer_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_lexer_test
  COMMAND template_lexer_test
)

add_executable(template_parser_test
  parser_test.cpp
)

target_link_libraries(template_parser_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_parser_test
  COMMAND template_parser_test
)

add_executable(template_compiler_test
  compiler_test.cpp
)

target_link_libraries(template_compiler_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_compiler_test
  COMMAND template_compiler_test
)

add_executable(template_renderer_test
  renderer_test.cpp
)

target_link_libraries(template_renderer_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_renderer_test
  COMMAND template_renderer_test
)

add_executable(template_engine_test
  engine_test.cpp
)

target_link_libraries(template_engine_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_engine_test
  COMMAND template_engine_test
)

add_executable(template_loader_test
  loader_test.cpp
)

target_link_libraries(template_loader_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_loader_test
  COMMAND template_loader_test
)

add_executable(template_escape_test
  escape_test.cpp
)

target_link_libraries(template_escape_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_escape_test
  COMMAND template_escape_test
)

add_executable(template_cache_test
  cache_test.cpp
)

target_link_libraries(template_cache_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_cache_test
  COMMAND template_cache_test
)

add_executable(template_integration_render_test
  integration_render_test.cpp
)

target_link_libraries(template_integration_render_test
  PRIVATE
    vix::template
)

add_test(
  NAME template_integration_render_test
  COMMAND template_integration_render_test
)

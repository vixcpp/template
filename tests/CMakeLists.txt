cmake_minimum_required(VERSION 3.20)

function(vix_template_add_test test_name test_source)
  add_executable(${test_name}
    ${test_source}
  )

  target_compile_features(${test_name}
    PRIVATE
      cxx_std_20
  )

  target_link_libraries(${test_name}
    PRIVATE
      vix::template
  )

  if (VIX_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(${test_name}
      PRIVATE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined
    )

    target_link_options(${test_name}
      PRIVATE
        -fsanitize=address,undefined
    )
  endif()

  add_test(
    NAME ${test_name}
    COMMAND ${test_name}
  )
endfunction()

vix_template_add_test(template_lexer_test lexer_test.cpp)
vix_template_add_test(template_parser_test parser_test.cpp)
vix_template_add_test(template_compiler_test compiler_test.cpp)
vix_template_add_test(template_renderer_test renderer_test.cpp)
vix_template_add_test(template_engine_test engine_test.cpp)
vix_template_add_test(template_loader_test loader_test.cpp)
vix_template_add_test(template_escape_test escape_test.cpp)
vix_template_add_test(template_cache_test cache_test.cpp)
vix_template_add_test(template_integration_render_test integration_render_test.cpp)

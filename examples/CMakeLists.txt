cmake_minimum_required(VERSION 3.20)

if(NOT TARGET vix::template)
  message(FATAL_ERROR "Target vix::template not found")
endif()

# ============================================================================
# basic_render
# ============================================================================

add_executable(template_basic_render
  basic_render.cpp
)

target_link_libraries(template_basic_render
  PRIVATE
    vix::template
)

# ============================================================================
# filters
# ============================================================================

add_executable(template_filters
  filters.cpp
)

target_link_libraries(template_filters
  PRIVATE
    vix::template
)

# ============================================================================
# includes
# ============================================================================

add_executable(template_includes
  includes.cpp
)

target_link_libraries(template_includes
  PRIVATE
    vix::template
)

# ============================================================================
# layout_inheritance
# ============================================================================

add_executable(template_layout_inheritance
  layout_inheritance.cpp
)

target_link_libraries(template_layout_inheritance
  PRIVATE
    vix::template
)

# ============================================================================
# loops_and_conditions
# ============================================================================

add_executable(template_loops_and_conditions
  loops_and_conditions.cpp
)

target_link_libraries(template_loops_and_conditions
  PRIVATE
    vix::template
)

# ============================================================================
# render_file
# ============================================================================

add_executable(template_render_file
  render_file.cpp
)

target_link_libraries(template_render_file
  PRIVATE
    vix::template
)

target_compile_definitions(template_render_file
  PRIVATE
    VIX_TEMPLATE_EXAMPLES_DIR="${CMAKE_SOURCE_DIR}/templates"
)

# ============================================================================
# Folder (IDE organization)
# ============================================================================

set_target_properties(
  template_basic_render
  template_filters
  template_includes
  template_layout_inheritance
  template_loops_and_conditions
  template_render_file
  PROPERTIES
    FOLDER "examples"
)

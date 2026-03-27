# Changelog

## v0.9.0

### V9 – Streaming Renderer

#### Goal

Introduce a streaming rendering API to support incremental output and reduce memory usage for large templates.

#### Features

- Added `render_stream(...)` API across the rendering pipeline:
  - `Template::render_stream`
  - `Engine::render_stream`
  - `Renderer::render_stream`
- Introduced streaming execution entrypoint in `ExecutionPlan`
- Output can now be written incrementally to a custom sink (`write(std::string)`)

#### Architecture

- Streaming rendering is implemented without breaking the existing execution model
- `ExecutionPlan` delegates execution to `Renderer` for consistency
- Classic rendering (`std::string` buffer) remains unchanged and fully supported
- Streaming path reuses the same execution logic to guarantee identical output

#### Benchmarks

- Added streaming benchmarks for all major rendering scenarios:
  - plain text
  - variables
  - expressions
  - conditionals
  - loops
  - includes
  - engine (cached / non-cached)

- Results show:
  - near-zero overhead compared to classic rendering
  - identical performance characteristics in most cases
  - no regression introduced by streaming API

#### Tests

- Added streaming rendering tests:
  - text rendering
  - variable rendering
  - HTML escaping
  - conditionals and loops
  - includes
  - equivalence check between `render` and `render_stream`

#### Notes

- Current implementation streams at the API level while internally using a buffer
- This ensures correctness and stability before introducing instruction-level streaming
- Future versions will:
  - eliminate intermediate buffers
  - enable true chunk-by-chunk rendering
  - support real-time streaming (HTTP, WebSocket)

#### Outcome

- Enables incremental rendering API
- Prepares the engine for real-time and large-scale rendering use cases
- Lays the foundation for true streaming execution in future versions

## v0.8.0

Make rendering truly compiled by eliminating runtime expression parsing.

### Highlights

V8 turns the execution plan into a real compiled runtime by removing expression reparsing during rendering.

Expressions are now compiled once and evaluated directly, unlocking major performance gains across the entire rendering pipeline.

### Features

- Compile expressions into instructions
  - `VariableInstr` now stores compiled expressions
  - `JumpIfFalseInstr` uses compiled conditions
- Remove runtime expression parsing
  - no more Lexer/Parser during render
  - direct evaluation from execution plan
- Improve execution plan efficiency
  - fully compiled rendering path
  - better CPU cache locality

### Performance Improvements

- Variable rendering: ~2.5x faster
- Expression rendering: ~4x faster
- Conditional rendering (`if`): ~4x faster
- Loops: ~2x faster
- Mixed templates: ~2x faster
- Engine cached templates: ~3–4x faster

Rendering performance now exceeds V6 across most scenarios.

### Renderer Improvements

- Remove `evaluate_compiled_expression(std::string, ...)`
- Use `evaluate_expression(Expression, ...)` directly
- Avoid temporary parsing structures during execution
- Optimize `render_template_by_name()`:
  - reduce allocations
  - avoid unnecessary `RenderResult` copies
  - use `execute_plan()` directly on fast path

### Compiler Improvements

- Add deep cloning of expression AST
- Store compiled expressions inside execution plan
- Remove `expression_to_string()` path

### Performance Notes

- Core rendering path is now significantly faster than V6
- Include and template composition still remain the main bottleneck
- Cache lookup remains sub-microsecond

### Why this release matters

V6 introduced execution plans.
V7 introduced correctness (cache + inheritance).

V8 completes the model:

- no runtime parsing
- true compiled execution
- predictable performance

This version marks the transition from a partially compiled engine to a fully compiled template runtime.

### Next Focus

- Optimize include and template composition
- Reduce loader and cache overhead
- Further improve inheritance execution path

## v0.7.0

Add cache-aware template freshness and safer inheritance rendering.

### Highlights

V7 improves the template engine with cache freshness validation and a more robust rendering path for inheritance and block overrides.

### Features

- Add source signature support to `Template`
  - freshness token for cache validation
  - enables safe invalidation of stale compiled templates
- Improve cache integration
  - validate compiled templates before reuse
- Improve inheritance rendering
  - better handling of `extends`
  - correct propagation of block overrides
- Keep AST alongside execution plan
  - execution plan = fast path
  - AST = inheritance, debugging, future compiler passes

### Internal Changes

- `Template` is now move-only
  - copy disabled
  - move enabled
- Add `source_signature()` and `set_source_signature()`
- Refine renderer logic for parent-child templates

### Performance Notes

- Cache lookup faster than V6
- Slight overhead on rendering paths (~10-13%)
- Include and inheritance paths heavier due to correctness improvements

---

## v0.6.0

Compiler optimization with execution plan.

### Features

- Introduce execution plan
  - precomputed rendering instructions
  - avoids AST traversal during render
- Add optimized renderer
  - instruction-based execution instead of recursive AST walk
- Improve rendering performance
  - faster execution
  - reduced CPU usage

### Internal Changes

- Separate parsing and execution phases
- Add `ExecutionPlan`
- Renderer supports plan-based execution

### Outcome

- major performance improvement
- foundation for further optimizations

---

## v0.5.0

Add full expression system.

### Features

- Support expressions in templates
  - `{{ user.name }}`
  - `{{ price * quantity }}`
  - `{{ a == b }}`
- Add expression AST
- Add operator parsing
- Add evaluation engine

### Outcome

- more flexible templates
- closer to full template engines

---

## v0.4.0

Add layout inheritance support.

### Features

- Support `{% extends %}`
- Support `{% block %}`
- Template inheritance system
- Parent-child template rendering

### Outcome

- reusable layouts
- modular templates

---

## v0.3.0

Add include support.

### Features

- Support `{% include %}`
- Template loader integration
- Dynamic template composition

### Outcome

- modular template structure
- reuse smaller components

---

## v0.2.3

Add filters support.

### Features

- Support filters on variables
  - `{{ name | upper }}`
- Filter system integration
- Extend rendering pipeline

---

## v0.2.2

Release improvements.

### Changes

- Improve project structure
- Clean examples and templates
- Improve stability

---

## v0.2.1

Documentation improvements.

### Changes

- Improve documentation
- Improve onboarding
- Add clearer examples

---

## v0.2.0

Stabilization release.

### Features

- Improve core engine reliability
- Refine API
- Prepare for advanced features (filters, include)

---

## v0.1.1

Fix cache handling for Template.

### Fixes

- Avoid copying non-copyable `Template`
- Fix ownership issues in cache layer

---

## v0.1.0

Initial template engine.

### Features

- AST-based template engine
- Parser and lexer
- Renderer
- Engine API
- Context system
- Basic template features
- Tests and examples
- Benchmarks

### Outcome

- fully functional template engine
- solid foundation for future versions

---

## Initial commits

- Project initialization
- Repository setup

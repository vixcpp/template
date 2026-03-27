# Changelog

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

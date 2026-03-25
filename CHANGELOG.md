# Changelog

All notable changes to this project will be documented in this file.

This project follows a simple versioning strategy:
- breaking changes → major
- new features → minor
- fixes → patch

---
## [0.1.1] - fix(cache)
- fix(cache): store compiled templates through shared immutable handles instead of copying non-copyable Template objects

## [0.1.0] - Initial release

### Added

#### Core Engine
- Template engine V1 implementation
- deterministic rendering pipeline:
  - Loader → Lexer → Parser → AST → Renderer
- support for:
  - variables (`{{ name }}`)
  - conditionals (`{% if %}`)
  - loops (`{% for %}`)
- HTML auto-escaping enabled by default

#### Architecture
- AST system:
  - RootNode
  - TextNode
  - VariableNode
  - IfNode
  - ForNode
- Context and Value runtime system
- Renderer with explicit node execution

#### API
- Engine (high-level API)
- Environment (configuration layer)
- Template abstraction
- Loader interface:
  - StringLoader
  - FileSystemLoader

#### Compiler & Cache
- Compiler abstraction (V1 pass-through)
- Cache interface for compiled templates

#### Builtins
- filter system foundation:
  - upper
  - lower
  - length
  - default

#### Tests
- full test suite:
  - lexer
  - parser
  - renderer
  - engine
  - cache
  - compiler
  - loader
  - integration tests

#### Examples
- basic rendering
- file-based rendering
- loops and conditions
- includes simulation
- layout inheritance simulation
- filters usage (C++ side)

#### Benchmarks
- parse benchmark
- render benchmark
- cache benchmark

#### Documentation
- architecture overview
- syntax reference
- AST explanation
- rendering pipeline
- roadmap

---

### Design Principles

- deterministic execution
- no hidden state
- minimal core
- explicit behavior
- performance-oriented

---

### Notes

This is the first stable version of the Vix template engine.

Features like filters in templates, includes, and layout inheritance
are planned for future versions.

---

## Next

Planned features:

- filter syntax integration (`{{ name | upper }}`)
- include support (`{% include %}`)
- layout inheritance (`{% extends %}`)
- expression system
- compiler optimizations

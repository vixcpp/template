# Roadmap

## Overview

The Vix template engine is designed to evolve from a minimal deterministic renderer into a high-performance, production-grade system.

This roadmap defines the progression from V1 to advanced capabilities.

---

## V1 – Core Engine (Completed)

### Features

- Variable interpolation
- If conditions
- For loops
- HTML escaping
- AST-based rendering
- String and file loaders
- Engine API
- Basic compiler (pass-through)
- Cache interface
- Built-in filters (not yet integrated)

### Characteristics

- minimal
- deterministic
- explicit
- fully functional

---

## V2 – Filters Integration

### Goal

Enable filter syntax inside templates.

### Example


{{ name | upper }}
{{ items | length }}


### Work Required

- extend Lexer to recognize `|`
- extend Parser to build filter nodes
- extend AST with `FilterNode`
- update Renderer to apply filters
- integrate Builtins registry

### Outcome

- expressive templates
- improved usability

---

## V3 – Includes

### Goal

Allow template composition.

### Example


{% include "header.html" %}


### Work Required

- new AST node: `IncludeNode`
- loader integration in Renderer
- optional caching of included templates

### Outcome

- reusable components
- cleaner templates

---

## V4 – Layout Inheritance

### Goal

Support layout systems (Twig/Jinja style).

### Example


{% extends "base.html" %}
{% block content %}
...
{% endblock %}


### Work Required

- AST nodes: `ExtendsNode`, `BlockNode`
- block resolution system
- rendering override logic

### Outcome

- powerful layout composition
- scalable UI systems

---

## V5 – Expression System

### Goal

Support expressions inside templates.

### Example


{{ user.name }}
{{ price * quantity }}
{{ a == b }}


### Work Required

- expression AST
- operator parsing
- evaluation engine

### Outcome

- flexible logic
- closer to full template engines

---

## V6 – Compiler Optimization

### Goal

Improve rendering performance.

### Ideas

- AST normalization
- constant folding
- precomputed execution plan
- instruction-based renderer

### Outcome

- faster rendering
- lower CPU usage

---

## V7 – Template Cache (Advanced)

### Goal

Production-ready caching.

### Features

- compiled template cache
- cache invalidation
- optional TTL policies

### Outcome

- zero re-parse cost
- improved scalability

---

## V8 – Streaming Renderer

### Goal

Support incremental output.

### Features

- chunked rendering
- output streaming
- reduced memory footprint

### Outcome

- better for large templates
- real-time responses

---

## V9 – Integration with Vix Runtime

### Goal

Native integration with Vix HTTP layer.

### Features

- direct HTTP response rendering
- middleware integration
- zero-copy output

### Outcome

- seamless backend usage
- high performance

---

## V10 – Advanced Features

### Future Ideas

- custom filters
- user-defined functions
- macros
- partial caching
- async rendering

---

## Design Strategy

The roadmap follows a strict philosophy:

1. build minimal core
2. validate correctness
3. add expressiveness
4. optimize performance
5. integrate deeply

---

## Priority Order

1. Filters integration
2. Includes
3. Layout inheritance
4. Expression system
5. Compiler optimization

---

## Summary

The Vix template engine evolves in layers:

- V1: working engine
- V2–V5: expressiveness
- V6–V8: performance
- V9+: ecosystem integration

The goal is not to be complex first, but to be:

- correct
- fast
- extensible

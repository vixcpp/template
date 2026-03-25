# Vix Template Engine Architecture

## Overview

The Vix template engine is a deterministic, minimal and high-performance rendering system designed for server-side applications.

It follows a strict pipeline:

Loader → Lexer → Parser → AST → Compiler → Template → Renderer

Each stage is explicit and independent, ensuring:
- predictable behavior
- no hidden state
- easy extensibility
- offline-first compatibility

---

## Core Components

### 1. Loader

Responsible for retrieving template source.

Implementations:
- `StringLoader` (in-memory)
- `FileSystemLoader` (disk-based)

Responsibilities:
- load raw template string
- check existence

---

### 2. Lexer

Transforms raw template source into tokens.

Input:

Hello {{ name }}

Output:

Text("Hello ")
VariableOpen
Identifier("name")
VariableClose

Responsibilities:
- detect template boundaries (`{{`, `{%`)
- tokenize identifiers, strings, numbers, operators
- track line and column

---

### 3. Parser

Builds an Abstract Syntax Tree (AST) from tokens.

Supported nodes (V1):
- `TextNode`
- `VariableNode`
- `IfNode`
- `ForNode`
- `RootNode`

Example:

{% if user %}Hello{% endif %}

Becomes:

IfNode(condition="user")
└── TextNode("Hello")

---

### 4. AST (Abstract Syntax Tree)

Structured representation of the template.

Key properties:
- deterministic
- minimal
- no runtime side-effects

Used by:
- Renderer
- future Compiler optimizations

---

### 5. Compiler

Transforms AST into a Template.

V1 behavior:
- direct pass-through

Future:
- AST normalization
- constant folding
- instruction-based execution

---

### 6. Template

Represents a compiled template.

Contains:
- template name
- root AST

Provides:
- render interface

---

### 7. Renderer

Traverses AST and produces final output.

Responsibilities:
- resolve variables from Context
- apply control flow (`if`, `for`)
- perform HTML escaping

Key design:
- no hidden allocations
- deterministic execution
- no global state

---

### 8. Context & Value

Context:
- runtime data store

Value:
- dynamic type container
- supports:
  - string
  - number
  - bool
  - array
  - object

Used during rendering.

---

### 9. Engine

High-level API.

Provides:

```cpp
engine.render("home.html", context);
```

Wraps:
- Environment
- Loader
- Renderer

---

### 10. Cache (optional)

Stores compiled templates.

Goals:
- avoid re-parsing
- improve performance

Design:
- explicit (no implicit eviction)
- deterministic

---

## Data Flow

```
[Template File]
      ↓
   Loader
      ↓
    Lexer
      ↓
    Parser
      ↓
     AST
      ↓
  Compiler (V1: pass-through)
      ↓
   Template
      ↓
   Renderer + Context
      ↓
  Final Output (string)
```

---

## Design Principles

### 1. Deterministic

No hidden behavior.
Same input → same output.

### 2. Explicit

No magic:
- no implicit includes
- no hidden globals
- no background processes

### 3. Minimal

V1 focuses on:
- variables
- if
- for

Everything else is layered later.

### 4. Extensible

Future features:
- filters (`{{ name | upper }}`)
- includes
- template inheritance
- compiled execution

### 5. Performance-Oriented

- AST reused across renders
- no dynamic reflection
- minimal allocations

---

## Future Evolution

Planned improvements:

### Filters
```
{{ name | upper }}
```

### Includes
```
{% include "header.html" %}
```

### Layout inheritance
```
{% extends "base.html" %}
{% block content %}
...
{% endblock %}
```

### Compiler optimizations
- precomputed execution plans
- bytecode-like instructions

---

## Summary

The Vix template engine is:

- simple by design
- powerful by architecture
- ready for extension
- optimized for real-world systems

It is built to integrate naturally into the Vix runtime and offline-first systems like Softadastra.


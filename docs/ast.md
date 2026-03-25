# AST (Abstract Syntax Tree)

## Overview

The Abstract Syntax Tree (AST) is the structured representation of a parsed template.

It is produced by the Parser and consumed by the Renderer.

The AST is:

- deterministic
- minimal
- immutable after construction
- free of runtime side-effects

---

## Why AST exists

The AST separates:

- parsing (syntax understanding)
- rendering (execution)

This allows:

- reuse across multiple renders
- future optimizations (compiler)
- predictable execution

---

## Node Types

V1 supports the following node types:

### RootNode

Represents the entire template.

Structure:

```
RootNode
├── Node
├── Node
└── Node
```

Responsibilities:

- holds ordered children
- entry point for rendering

---

### TextNode

Represents raw text.

Example:

```
Hello world
```

Becomes:

```
TextNode("Hello world")
```

Rendered directly to output.

---

### VariableNode

Represents variable interpolation.

Example:

```
{{ name }}
```

Becomes:

```
VariableNode("name")
```

Resolved at runtime using Context.

---

### IfNode

Represents conditional logic.

Example:

```
{% if user %}Hello{% endif %}
```

Becomes:

```
IfNode(condition="user")
└── TextNode("Hello")
```

Behavior:

- condition is evaluated at runtime
- body is rendered only if truthy

---

### ForNode

Represents iteration over arrays.

Example:

```
{% for item in items %}{{ item }}{% endfor %}
```

Becomes:

```
ForNode(item="item", iterable="items")
└── VariableNode("item")
```

Behavior:

- iterable resolved from Context
- loop creates a new scoped context per iteration

---

## AST Structure Example

Template:

```
Hello {{ name }}
{% if user %}Admin{% endif %}
```

AST:

```
RootNode
├── TextNode("Hello ")
├── VariableNode("name")
└── IfNode("user")
    └── TextNode("Admin")
```

---

## Memory Model

Nodes are owned using:

```cpp
std::unique_ptr<Node>
```

Properties:

- no shared ownership
- clear lifetime
- no hidden references

---

## NodeList

A NodeList is:

```cpp
using NodeList = std::vector<NodePtr>;
```

Used by:

- RootNode
- IfNode
- ForNode

---

## Execution Model

The Renderer walks the AST:

```
for each node:
  render_node(node)
```

Each node defines its behavior:

- TextNode → append text
- VariableNode → resolve + output
- IfNode → conditional execution
- ForNode → iterative execution

---

## Design Principles

### 1. Minimal

Only essential nodes exist in V1.

### 2. Deterministic

AST contains no dynamic logic.
All behavior is explicit.

### 3. Extensible

Future nodes:

- IncludeNode
- BlockNode
- ExtendsNode
- FilterNode

### 4. Stable Core

AST is the foundation:

- Renderer depends on it
- Compiler will optimize it
- Features will extend it

---

## Future Evolution

Planned improvements:

### Expression Nodes

```
{{ user.name }}
{{ a + b }}
```

---

### Filter Nodes

```
{{ name | upper }}
```

---

### Block System

```
{% block content %}
...
{% endblock %}
```

---

## Summary

The AST is:

- the core structure of the engine
- simple but powerful
- designed for performance and extensibility

It enables Vix to evolve from a basic template engine to a fully optimized rendering system.


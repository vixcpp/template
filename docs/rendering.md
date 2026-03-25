# Rendering

## Overview

Rendering is the process of transforming a parsed template (AST) into a final output string using a runtime context.

It is handled by the **Renderer**.

AST + Context → Renderer → Output

---

## Rendering Pipeline

The rendering flow is simple and deterministic:

Template
↓
RootNode (AST)
↓
Renderer
↓
Final string output

Steps:

1. Load and parse template (done before rendering)
2. Traverse AST
3. Resolve variables from Context
4. Apply control flow
5. Produce final string

---

## Renderer Responsibilities

The Renderer:

- walks the AST
- resolves variables
- applies `if` conditions
- executes `for` loops
- performs HTML escaping

It does **not**:
- modify the AST
- perform I/O
- hold global state

---

## Node Execution

Each node type defines its behavior.

### RootNode

Iterates over children:

```
for each child:
  render(child)
```

---

### TextNode

Appends raw text:

```
output += text
```

---

### VariableNode

Steps:

1. resolve value from Context
2. convert to string
3. optionally escape HTML
4. append to output

```
value = context.get(name)
output += escape(value)
```

If variable is missing → empty string

---

### IfNode

Steps:

1. resolve condition
2. check truthiness
3. render body if true

```
if truthy(condition):
  render(body)
```

---

### ForNode

Steps:

1. resolve iterable
2. iterate over items
3. create scoped context
4. render body per item

```
for item in iterable:
  context[item_name] = item
  render(body)
```

---

## Context Resolution

Variables are resolved from `Context`.

Example:

```cpp
ctx.set("name", "Alice");
```

Template:

```
{{ name }}
```

Output:

```
Alice
```

---

## Truthy Rules

A value is considered true if:

- non-empty string
- non-zero number
- non-empty array
- boolean true

Otherwise false.

---

## HTML Escaping

By default, rendering escapes HTML:

```
{{ html }}
```

Input:

```
<b>Hello</b>
```

Output:

```
<b>Hello</b>
```

Controlled via:

```cpp
Renderer(true);  // enabled
Renderer(false); // disabled
```

---

## Error Handling

Rendering errors occur when:

- invalid AST structure
- invalid loop iterable
- null nodes (unexpected)

Errors throw:

```
RendererError
```

---

## Performance Model

Rendering is:

- linear over AST size
- allocation-light
- predictable

Key characteristics:

- no reflection
- no dynamic parsing during render
- AST reused across renders

---

## Example

Template:

```
Hello {{ name }}
{% for item in items %}{{ item }}{% endfor %}
```

Context:

```cpp
ctx.set("name", "Alice");
ctx.set("items", ["A", "B"]);
```

Output:

```
Hello Alice
AB
```

---

## Design Principles

### 1. Deterministic

Same input → same output

---

### 2. Stateless

Renderer does not keep state between renders

---

### 3. Explicit

All behavior is visible in AST

---

### 4. Fast

No parsing during rendering

---

## Future Improvements

Planned enhancements:

### Filters

```
{{ name | upper }}
```

---

### Compiled Execution

- precomputed instructions
- faster rendering loop

---

### Streaming Output

- incremental rendering
- reduced memory usage

---

## Summary

Rendering is:

- simple
- fast
- predictable

It is the execution phase of the template engine and is designed for real-world performance and reliability.


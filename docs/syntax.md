# Template Syntax

## Overview

The Vix template engine provides a minimal and deterministic syntax for rendering dynamic content.

V1 supports:

- variables
- conditionals (`if`)
- loops (`for`)
- HTML escaping (automatic)

---

## Variables

Variables are rendered using double curly braces:

```
{{ name }}
```

Example:

```
Hello {{ name }}
```

Context:

```cpp
ctx.set("name", "Alice");
```

Output:

```
Hello Alice
```

---

## HTML Escaping

By default, variables are HTML-escaped.

Example:

```
{{ html }}
```

Context:

```cpp
ctx.set("html", "<b>Hello</b>");
```

Output:

```
&lt;b&gt;Hello&lt;/b&gt;
```

Escaped characters:

- `&` → `&amp;`
- `<` → `&lt;`
- `>` → `&gt;`
- `"` → `&quot;`
- `'` → `&#39;`

---

## If Condition

Conditional blocks use `{% if %}`.

```
{% if user %}
Hello {{ user }}
{% endif %}
```

Context:

```cpp
ctx.set("user", "Alice");
```

Output:

```
Hello Alice
```

If the condition is false:

```cpp
ctx.set("user", false);
```

Output:

```
(empty)
```

---

## Truthy Values

A value is considered true if:

- non-empty string
- non-zero number
- non-empty array
- boolean true

Otherwise it is false.

---

## For Loop

Loop over arrays using `{% for %}`.

```
{% for item in items %}
- {{ item }}
{% endfor %}
```

Context:

```cpp
Array items;
items.emplace_back("A");
items.emplace_back("B");

ctx.set("items", items);
```

Output:

```
- A
- B
```

---

## Text

Any content outside `{{ }}` and `{% %}` is treated as raw text.

```
<h1>Hello</h1>
```

---

## Supported Expressions (V1)

V1 intentionally supports only simple expressions:

- variable name
- identifiers inside `if` and `for`

Example:

```
{{ name }}
{% if user %}
{% for item in items %}
```

Not supported yet:

- arithmetic (`a + b`)
- comparisons (`a == b`)
- nested expressions

---

## Whitespace

Whitespace inside expressions is ignored:

```
{{   name   }}
{%   if user   %}
```

---

## Missing Variables

If a variable is missing, it renders as empty:

```
Hello {{ missing }}
```

Output:

```
Hello
```

---

## Limitations (V1)

The following features are not yet available:

### Filters
```
{{ name | upper }}
```

### Includes
```
{% include "header.html" %}
```

### Template Inheritance
```
{% extends "base.html" %}
{% block content %}
...
{% endblock %}
```

---

## Design Philosophy

The syntax is:

- minimal
- predictable
- explicit

No hidden behavior.
No magic.

---

## Summary

Supported syntax:

```
{{ variable }}
{% if condition %} ... {% endif %}
{% for item in items %} ... {% endfor %}
```

This foundation is intentionally simple and will be extended in future versions.


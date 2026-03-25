<table>
  <tr>
    <td valign="top" width="70%">

<h1>Vix Template Engine</h1>

<p>
  <a href="https://x.com/vix_cpp">
    <img src="https://img.shields.io/badge/X-Follow-black?logo=x" alt="X" />
  </a>
  <a href="https://www.youtube.com/@vixcpp">
    <img src="https://img.shields.io/badge/YouTube-Subscribe-red?logo=youtube" alt="YouTube" />
  </a>
  <img src="https://img.shields.io/github/stars/vixcpp/template?style=flat" alt="Stars" />
  <img src="https://img.shields.io/github/forks/vixcpp/template?style=flat" alt="Forks" />
</p>

<p>
  <b>Vix Template Engine</b> is a minimal, deterministic and high-performance
  <b>C++ template rendering engine</b> built for real systems.
</p>

<p>
  Designed for <b>predictability</b>, <b>performance</b>, and <b>clarity</b>.
</p>

</td>
    <td valign="middle" width="30%" align="right">
      <img
        src="https://res.cloudinary.com/dwjbed2xb/image/upload/v1762524350/vixcpp_etndhz.png"
        alt="Vix Logo"
        width="160"
        style="border-radius:50%;"
      />
    </td>
  </tr>
</table>

<hr />

## Simple. Deterministic. Fast.

The Vix template engine removes complexity found in traditional template systems.

No magic. No hidden state. Just a clean pipeline:

```
Loader → Lexer → Parser → AST → Template → Renderer
```

---

## Example

### Template

```
Hello {{ name }}
{% if user %}Welcome{% endif %}
```

### C++

```cpp
#include <vix/template/Engine.hpp>

using namespace vix::template_;

int main() {
  Engine engine;

  Context ctx;
  ctx.set("name", "Alice");
  ctx.set("user", true);

  auto result = engine.render_string(
    "Hello {{ name }} {% if user %}Welcome{% endif %}",
    ctx
  );

  std::cout << result.output;
}
```

### Output

```
Hello Alice Welcome
```

---

## Features (V1)

- Variable interpolation
- If conditions
- For loops
- HTML escaping (automatic)
- AST-based rendering
- Deterministic execution
- Optional caching

---

## Syntax

### Variables

```
{{ name }}
```

### Condition

```
{% if user %}
Hello
{% endif %}
```

### Loop

```
{% for item in items %}
{{ item }}
{% endfor %}
```

---

## Why Vix Template Engine

### Deterministic

Same input → same output
No hidden behavior

### Minimal

Only essential features
No over-engineering

### Performance-first

- AST reused across renders
- no dynamic parsing at runtime
- low allocations

### C++ Native

No runtime dependency
Full control over memory and performance

---

## Build

```bash
cmake --preset dev-ninja
cmake --build --preset build-ninja
```

---

## Run Examples

```bash
./build-ninja/template_basic_render
./build-ninja/template_loops_and_conditions
```

---

## Benchmarks

```bash
./build-ninja/template_render_bench
./build-ninja/template_parse_bench
./build-ninja/template_cache_bench
```

---

## Architecture

The engine is built on a clean separation of concerns:

- Loader → source retrieval
- Lexer → tokenization
- Parser → AST creation
- Template → compiled structure
- Renderer → execution

This makes the system:

- easy to understand
- easy to extend
- easy to optimize

---

## Roadmap

Upcoming features:

- Filters (`{{ name | upper }}`)
- Includes
- Layout inheritance
- Expression system
- Compiler optimizations

---

## Philosophy

The goal is not to be complex first.

The goal is to be:

- correct
- fast
- extensible

---

## Contributing

Contributions are welcome.

If you care about modern C++, performance, and real-world systems, this project is for you.

---

⭐ Star the project if you like the direction.

MIT License


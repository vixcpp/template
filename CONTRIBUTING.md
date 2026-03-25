# Contributing to Vix Template Engine

Thank you for your interest in contributing.

This project focuses on performance, clarity, and deterministic behavior. Contributions should follow these principles.

---

## Philosophy

Before contributing, understand the core goals:

- deterministic behavior
- minimal design
- no hidden state
- performance-first mindset

If your change adds complexity, it must bring clear value.

---

## Getting Started

### 1. Fork the repository

```bash
git clone https://github.com/vixcpp/template.git
cd template
```

### 2. Create a branch

```bash
git checkout -b feature/my-feature
```

### 3. Build the project

```bash
cmake --preset dev-ninja
cmake --build --preset build-ninja
```

### 4. Run tests

```bash
ctest --preset test-ninja
```

---

## Code Guidelines

### C++ Standards

- Use modern C++ (C++20)
- Prefer RAII
- Avoid raw pointers unless necessary
- Use `std::unique_ptr` for ownership
- Avoid unnecessary allocations

### Style

- Clear and explicit code
- Small, focused functions
- No hidden behavior
- Avoid magic

### Naming

- Use English names
- Be descriptive and consistent

---

## Architecture Rules

Do not break core guarantees:

- AST must remain deterministic
- Renderer must remain stateless
- No I/O inside rendering
- No global mutable state

---

## Tests

All changes must include tests when relevant.

- Add tests in `tests/`
- Keep tests deterministic
- Avoid external dependencies

---

## Benchmarks

If your change impacts performance:

- update benchmarks in `benchmarks/`
- explain performance impact in PR

---

## Commit Messages

Use clear messages:

```bash
feat(parser): add filter node support
fix(renderer): avoid unnecessary allocations
refactor(cache): use shared template handles
```

---

## Pull Requests

Before opening a PR:

- ensure project builds
- ensure tests pass
- keep PR focused (one change = one PR)
- describe clearly what you changed and why

---

## What Not to Do

- do not introduce heavy dependencies
- do not add magic behavior
- do not break deterministic design
- do not add features without discussion (for large changes)

---

## Discussions

For large features:

- open an issue first
- discuss design before implementation

---

## Summary

Good contributions are:

- simple
- explicit
- fast
- aligned with architecture

---

Thanks for contributing to Vix Template Engine 🚀


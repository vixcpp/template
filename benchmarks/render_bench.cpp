/**
 *
 *  @file render_bench.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */
#include <benchmark/benchmark.h>

#include <memory>
#include <string>
#include <utility>

#include <vix/template/Compiler.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/StringLoader.hpp>
#include <vix/template/Template.hpp>

using namespace vix::template_;

namespace
{
  [[nodiscard]] Template compile_template(
      const std::string &name,
      const std::string &source,
      std::shared_ptr<Loader> loader = nullptr)
  {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    RootNode root = parser.parse();

    Compiler compiler;
    return compiler.compile(name, std::move(root), std::move(loader));
  }

  [[nodiscard]] Context make_basic_context()
  {
    Context ctx;
    ctx.set("name", "Alice");
    ctx.set("bio", "offline-first systems");
    ctx.set("price", 6);
    ctx.set("quantity", 7);
    ctx.set("enabled", true);

    Object user;
    user["name"] = Value("Alice");
    user["role"] = Value("admin");
    ctx.set("user", user);

    Array items;
    items.emplace_back("Book");
    items.emplace_back("Phone");
    items.emplace_back("Pen");
    items.emplace_back("Bag");
    items.emplace_back("Mouse");
    ctx.set("items", items);

    return ctx;
  }

} // namespace

static void BM_render_plain_text(benchmark::State &state)
{
  const Template tpl = compile_template(
      "plain_text",
      "Hello world. This is a simple static template used for benchmarking.");
  const Context ctx;

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_plain_text);

static void BM_render_variable(benchmark::State &state)
{
  const Template tpl = compile_template(
      "variable",
      "Hello {{ name }}");
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_variable);

static void BM_render_expression(benchmark::State &state)
{
  const Template tpl = compile_template(
      "expression",
      "{{ price * quantity }}");
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_expression);

static void BM_render_if(benchmark::State &state)
{
  const Template tpl = compile_template(
      "if_template",
      "{% if enabled %}Hello {{ name }}{% endif %}");
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_if);

static void BM_render_for_loop(benchmark::State &state)
{
  const Template tpl = compile_template(
      "for_loop",
      "{% for item in items %}[{{ item }}]{% endfor %}");
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_for_loop);

static void BM_render_mixed_template(benchmark::State &state)
{
  const Template tpl = compile_template(
      "mixed",
      "Hello {{ user.name }}\n"
      "{% if enabled %}"
      "Role: {{ user.role }}\n"
      "Total: {{ price * quantity }}\n"
      "{% endif %}"
      "Items: {% for item in items %}[{{ item | upper }}]{% endfor %}");
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_mixed_template);

static void BM_render_include(benchmark::State &state)
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("header.html", "Header {{ name }}\n");
  loader->set("body.html", "{% include \"header.html\" %}Total: {{ price * quantity }}");
  const Template tpl = compile_template(
      "include_template",
      "{% include \"body.html\" %}",
      loader);
  const Context ctx = make_basic_context();

  for (auto _ : state)
  {
    const RenderResult result = tpl.render(ctx);
    benchmark::DoNotOptimize(result.output);
  }

  state.SetItemsProcessed(
      static_cast<std::int64_t>(state.iterations()));
}
BENCHMARK(BM_render_include);

BENCHMARK_MAIN();

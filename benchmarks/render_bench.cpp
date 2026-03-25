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
#include <chrono>
#include <iostream>
#include <string>

#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Renderer.hpp>
#include <vix/template/Context.hpp>

using namespace vix::template_;

static RootNode build_ast(const std::string &tpl)
{
  Lexer lexer(tpl);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  return parser.parse();
}

int main()
{
  const std::string tpl =
      "<h1>{{ title }}</h1>\n"
      "{% if user %}Hello {{ user }}{% endif %}\n"
      "<ul>\n"
      "{% for item in items %}<li>{{ item }}</li>{% endfor %}\n"
      "</ul>";

  // Build AST once (simulate compiled template)
  RootNode root = build_ast(tpl);

  // Prepare context
  Context ctx;
  ctx.set("title", "Benchmark");
  ctx.set("user", "Gaspard");

  Array items;
  items.emplace_back("Laptop");
  items.emplace_back("Phone");
  items.emplace_back("Book");
  ctx.set("items", items);

  Renderer renderer(true);

  const int iterations = 100000;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; ++i)
  {
    auto result = renderer.render(root, ctx);
    (void)result;
  }

  auto end = std::chrono::high_resolution_clock::now();

  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Render benchmark\n";
  std::cout << "Iterations: " << iterations << '\n';
  std::cout << "Total time (ms): " << duration.count() << '\n';
  std::cout << "Avg per render (us): "
            << (duration.count() * 1000.0 / iterations) << '\n';

  return 0;
}

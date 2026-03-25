/**
 *
 *  @file parse_bench.cpp
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

using namespace vix::template_;

int main()
{
  const std::string tpl =
      "<h1>{{ title }}</h1>\n"
      "{% if user %}Hello {{ user }}{% endif %}\n"
      "<ul>\n"
      "{% for item in items %}<li>{{ item }}</li>{% endfor %}\n"
      "</ul>";

  const int iterations = 10000;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; ++i)
  {
    Lexer lexer(tpl);
    auto tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    RootNode root = parser.parse();

    (void)root;
  }

  auto end = std::chrono::high_resolution_clock::now();

  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Parse benchmark\n";
  std::cout << "Iterations: " << iterations << '\n';
  std::cout << "Total time (ms): " << duration.count() << '\n';
  std::cout << "Avg per parse (us): "
            << (duration.count() * 1000.0 / iterations) << '\n';

  return 0;
}

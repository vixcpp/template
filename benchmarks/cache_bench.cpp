/**
 *
 *  @file cache_bench.cpp
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
#include <memory>
#include <string>

#include <vix/template/Cache.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/Template.hpp>

using namespace vix::template_;

static std::shared_ptr<const Template> make_template(
    const std::string &name,
    const std::string &signature)
{
  return std::make_shared<Template>(
      name,
      RootNode{},
      ExecutionPlan{},
      nullptr,
      signature);
}

int main()
{
  Cache cache;

  constexpr int entries = 1000;
  constexpr int iterations = 100000;

  for (int i = 0; i < entries; ++i)
  {
    const std::string key = "tpl_" + std::to_string(i);
    const std::string signature = "sig_" + std::to_string(i);

    cache.put(key, make_template(key, signature), signature);
  }

  const auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; ++i)
  {
    const int index = i % entries;
    const std::string key = "tpl_" + std::to_string(index);
    const std::string signature = "sig_" + std::to_string(index);

    auto tpl = cache.get(key, signature);
    (void)tpl;
  }

  const auto end = std::chrono::high_resolution_clock::now();

  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  std::cout << "Cache benchmark\n";
  std::cout << "Entries: " << entries << '\n';
  std::cout << "Iterations: " << iterations << '\n';
  std::cout << "Total time (ms): " << duration.count() << '\n';
  std::cout << "Avg per lookup (ns): "
            << (duration.count() * 1'000'000.0 / iterations) << '\n';

  return 0;
}

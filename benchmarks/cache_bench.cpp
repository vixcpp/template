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
#include <map>
#include <optional>
#include <string>

#include <vix/template/Cache.hpp>
#include <vix/template/Template.hpp>

using namespace vix::template_;

class MemoryCache : public Cache
{
public:
  std::optional<Template> get(const std::string &name) const override
  {
    const auto it = store_.find(name);
    if (it == store_.end())
      return std::nullopt;
    return it->second;
  }

  void put(const std::string &name, const Template &tpl) override
  {
    store_[name] = tpl;
  }

  bool erase(const std::string &name) override
  {
    return store_.erase(name) > 0;
  }

  void clear() noexcept override
  {
    store_.clear();
  }

  bool has(const std::string &name) const override
  {
    return store_.find(name) != store_.end();
  }

private:
  std::map<std::string, Template> store_;
};

static Template make_template(const std::string &name)
{
  return Template(name, RootNode{});
}

int main()
{
  MemoryCache cache;

  const int entries = 1000;
  const int iterations = 100000;

  // Fill cache
  for (int i = 0; i < entries; ++i)
  {
    cache.put("tpl_" + std::to_string(i), make_template("tpl"));
  }

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; ++i)
  {
    const std::string key = "tpl_" + std::to_string(i % entries);
    auto tpl = cache.get(key);
    (void)tpl;
  }

  auto end = std::chrono::high_resolution_clock::now();

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

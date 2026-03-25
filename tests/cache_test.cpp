/**
 *
 *  @file cache_test.cpp
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
#include <cassert>
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
    {
      return std::nullopt;
    }
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

static void test_put_and_get()
{
  MemoryCache cache;

  auto tpl = make_template("home");
  cache.put("home", tpl);

  auto result = cache.get("home");

  assert(result.has_value());
  assert(result->name() == "home");
}

static void test_has()
{
  MemoryCache cache;

  assert(!cache.has("home"));

  cache.put("home", make_template("home"));
  assert(cache.has("home"));
}

static void test_erase()
{
  MemoryCache cache;

  cache.put("home", make_template("home"));
  assert(cache.has("home"));

  bool removed = cache.erase("home");
  assert(removed);
  assert(!cache.has("home"));
}

static void test_clear()
{
  MemoryCache cache;

  cache.put("a", make_template("a"));
  cache.put("b", make_template("b"));

  cache.clear();

  assert(!cache.has("a"));
  assert(!cache.has("b"));
}

static void test_get_missing()
{
  MemoryCache cache;

  auto result = cache.get("missing");
  assert(!result.has_value());
}

int main()
{
  test_put_and_get();
  test_has();
  test_erase();
  test_clear();
  test_get_missing();

  std::cout << "[OK] template cache tests passed\n";
  return 0;
}

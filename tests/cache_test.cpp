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
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <vix/template/Cache.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/Template.hpp>

using namespace vix::template_;

static std::shared_ptr<const Template> make_template(
    const std::string &name,
    const std::string &signature = {})
{
  return std::make_shared<Template>(
      name,
      RootNode{},
      ExecutionPlan{},
      nullptr,
      signature);
}

static void test_put_and_get()
{
  Cache cache;

  auto tpl = make_template("home", "sig1");
  cache.put("home", tpl, "sig1");

  auto result = cache.get("home", "sig1");

  assert(result != nullptr);
  assert(result->name() == "home");
  assert(result->source_signature() == "sig1");
}

static void test_contains()
{
  Cache cache;

  assert(!cache.contains("home"));
  assert(!cache.contains("home", "sig1"));

  cache.put("home", make_template("home", "sig1"), "sig1");

  assert(cache.contains("home"));
  assert(cache.contains("home", "sig1"));
  assert(!cache.contains("home", "sig2"));
}

static void test_invalidate()
{
  Cache cache;

  cache.put("home", make_template("home", "sig1"), "sig1");
  assert(cache.contains("home", "sig1"));

  const bool removed = cache.invalidate("home");
  assert(removed);
  assert(!cache.contains("home", "sig1"));
}

static void test_clear()
{
  Cache cache;

  cache.put("a", make_template("a", "s1"), "s1");
  cache.put("b", make_template("b", "s2"), "s2");

  assert(cache.size() == 2);

  cache.clear();

  assert(cache.empty());
  assert(cache.size() == 0);
}

static void test_get_missing()
{
  Cache cache;

  auto result = cache.get("missing");
  assert(result == nullptr);
}

static void test_signature_mismatch_returns_null()
{
  Cache cache;

  cache.put("home", make_template("home", "sig1"), "sig1");

  auto result = cache.get("home", "sig2");
  assert(result == nullptr);
}

static void test_replace_existing_entry()
{
  Cache cache;

  cache.put("home", make_template("home", "sig1"), "sig1");
  cache.put("home", make_template("home", "sig2"), "sig2");

  assert(cache.size() == 1);
  assert(!cache.contains("home", "sig1"));
  assert(cache.contains("home", "sig2"));

  auto result = cache.get("home", "sig2");
  assert(result != nullptr);
  assert(result->source_signature() == "sig2");
}

static void test_ttl_expiration()
{
  CachePolicy policy;
  policy.ttl = std::chrono::milliseconds(1);

  Cache cache(policy);

  cache.put("home", make_template("home", "sig1"), "sig1");
  assert(cache.contains("home", "sig1"));

  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  assert(!cache.contains("home", "sig1"));

  auto result = cache.get("home", "sig1");
  assert(result == nullptr);
}

static void test_invalidate_expired()
{
  CachePolicy policy;
  policy.ttl = std::chrono::milliseconds(1);

  Cache cache(policy);

  cache.put("a", make_template("a", "s1"), "s1");
  cache.put("b", make_template("b", "s2"), "s2");

  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  const std::size_t removed = cache.invalidate_expired();
  assert(removed == 2);
  assert(cache.empty());
}

static void test_no_ttl_means_no_expiration()
{
  CachePolicy policy;
  policy.ttl = std::nullopt;

  Cache cache(policy);

  cache.put("home", make_template("home", "sig1"), "sig1");

  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  assert(cache.contains("home", "sig1"));

  auto result = cache.get("home", "sig1");
  assert(result != nullptr);
  assert(result->name() == "home");
}

int main()
{
  test_put_and_get();
  test_contains();
  test_invalidate();
  test_clear();
  test_get_missing();
  test_signature_mismatch_returns_null();
  test_replace_existing_entry();
  test_ttl_expiration();
  test_invalidate_expired();
  test_no_ttl_means_no_expiration();

  std::cout << "[OK] template cache tests passed\n";
  return 0;
}

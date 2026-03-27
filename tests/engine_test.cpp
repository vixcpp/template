/**
 *
 *  @file engine_test.cpp
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

#include <vix/template/Engine.hpp>
#include <vix/template/StringLoader.hpp>

using namespace vix::template_;

static Engine make_engine()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("hello", "Hello {{ name }}");
  loader->set("if_test", "{% if user %}OK{% endif %}");
  loader->set("loop", "{% for item in items %}{{ item }}{% endfor %}");

  return Engine(loader);
}

static std::shared_ptr<StringLoader> make_loader()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("hello", "Hello {{ name }}");
  loader->set("if_test", "{% if user %}OK{% endif %}");
  loader->set("loop", "{% for item in items %}{{ item }}{% endfor %}");

  return loader;
}

static void test_render_simple()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("name", "Alice");

  auto result = engine.render("hello", ctx);
  assert(result.success);
  assert(result.output == "Hello Alice");
}

static void test_render_if()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("user", true);

  auto result = engine.render("if_test", ctx);
  assert(result.success);
  assert(result.output == "OK");
}

static void test_render_if_false()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("user", false);

  auto result = engine.render("if_test", ctx);
  assert(result.success);
  assert(result.output.empty());
}

static void test_render_loop()
{
  Engine engine = make_engine();

  Context ctx;
  Array items;
  items.emplace_back("A");
  items.emplace_back("B");
  items.emplace_back("C");

  ctx.set("items", items);

  auto result = engine.render("loop", ctx);
  assert(result.success);
  assert(result.output == "ABC");
}

static void test_render_object_context()
{
  Engine engine = make_engine();

  Object values;
  values["name"] = "Bob";

  auto result = engine.render("hello", values);
  assert(result.success);
  assert(result.output == "Hello Bob");
}

static void test_auto_escape()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("html", "{{ html }}");

  Engine engine(loader);

  Context ctx;
  ctx.set("html", "<b>test</b>");

  auto result = engine.render("html", ctx);
  assert(result.success);
  assert(result.output == "&lt;b&gt;test&lt;/b&gt;");
  assert(result.escaped);
}

static void test_cache_store_after_first_render()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");

  Engine engine(loader);

  Context ctx;
  ctx.set("name", "Alice");

  assert(engine.cache().empty());

  auto result = engine.render("hello", ctx);
  assert(result.success);
  assert(result.output == "Hello Alice");

  assert(engine.cache().size() == 1);
  assert(engine.cache().contains("hello", loader->source_signature("hello")));
}

static void test_cache_invalidation_when_template_changes()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");

  Engine engine(loader);

  Context ctx;
  ctx.set("name", "Alice");

  auto result1 = engine.render("hello", ctx);
  assert(result1.success);
  assert(result1.output == "Hello Alice");
  assert(engine.cache().size() == 1);

  const std::string old_signature = loader->source_signature("hello");
  assert(engine.cache().contains("hello", old_signature));

  loader->set("hello", "Hi {{ name }}");

  const std::string new_signature = loader->source_signature("hello");
  assert(old_signature != new_signature);

  auto result2 = engine.render("hello", ctx);
  assert(result2.success);
  assert(result2.output == "Hi Alice");

  assert(engine.cache().size() == 1);
  assert(engine.cache().contains("hello", new_signature));
  assert(!engine.cache().contains("hello", old_signature));
}

static void test_cache_manual_invalidate()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");

  Engine engine(loader);

  Context ctx;
  ctx.set("name", "Alice");

  auto result = engine.render("hello", ctx);
  assert(result.success);
  assert(engine.cache().size() == 1);

  const bool removed = engine.invalidate("hello");
  assert(removed);
  assert(engine.cache().empty());
}

static void test_cache_disable()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");

  Engine engine(loader);
  engine.set_cache_enabled(false);

  Context ctx;
  ctx.set("name", "Alice");

  auto result1 = engine.render("hello", ctx);
  auto result2 = engine.render("hello", ctx);

  assert(result1.success);
  assert(result2.success);
  assert(result1.output == "Hello Alice");
  assert(result2.output == "Hello Alice");
  assert(engine.cache().empty());
}

static void test_clear_cache()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");
  loader->set("if_test", "{% if user %}OK{% endif %}");

  Engine engine(loader);

  Context ctx1;
  ctx1.set("name", "Alice");

  Context ctx2;
  ctx2.set("user", true);

  auto result1 = engine.render("hello", ctx1);
  auto result2 = engine.render("if_test", ctx2);

  assert(result1.success);
  assert(result2.success);
  assert(engine.cache().size() == 2);

  engine.clear_cache();
  assert(engine.cache().empty());
}

static void test_cache_ttl_expiration()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("hello", "Hello {{ name }}");

  CachePolicy policy;
  policy.ttl = std::chrono::milliseconds(1);

  Engine engine(loader, policy);

  Context ctx;
  ctx.set("name", "Alice");

  auto result = engine.render("hello", ctx);
  assert(result.success);
  assert(engine.cache().size() == 1);

  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  const std::string signature = loader->source_signature("hello");
  assert(!engine.cache().contains("hello", signature));
}

int main()
{
  test_render_simple();
  test_render_if();
  test_render_if_false();
  test_render_loop();
  test_render_object_context();
  test_auto_escape();

  test_cache_store_after_first_render();
  test_cache_invalidation_when_template_changes();
  test_cache_manual_invalidate();
  test_cache_disable();
  test_clear_cache();
  test_cache_ttl_expiration();

  std::cout << "[OK] template engine tests passed\n";
  return 0;
}

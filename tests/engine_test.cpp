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
#include <iostream>
#include <memory>
#include <string>

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

static void test_render_simple()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("name", "Alice");

  auto result = engine.render("hello", ctx);
  assert(result.output == "Hello Alice");
}

static void test_render_if()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("user", true);

  auto result = engine.render("if_test", ctx);
  assert(result.output == "OK");
}

static void test_render_if_false()
{
  Engine engine = make_engine();

  Context ctx;
  ctx.set("user", false);

  auto result = engine.render("if_test", ctx);
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
  assert(result.output == "ABC");
}

static void test_render_object_context()
{
  Engine engine = make_engine();

  Object values;
  values["name"] = "Bob";

  auto result = engine.render("hello", values);
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
  assert(result.output == "&lt;b&gt;test&lt;/b&gt;");
}

int main()
{
  test_render_simple();
  test_render_if();
  test_render_if_false();
  test_render_loop();
  test_render_object_context();
  test_auto_escape();

  std::cout << "[OK] template engine tests passed\n";
  return 0;
}

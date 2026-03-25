/**
 *
 *  @file integration_render_test.cpp
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

static std::shared_ptr<StringLoader> make_loader()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set(
      "home",
      "Hello {{ name }}!\n"
      "{% if is_admin %}Admin panel enabled.\n{% endif %}"
      "Items: {% for item in items %}[{{ item }}]{% endfor %}");

  loader->set(
      "profile",
      "User: {{ username }}\n"
      "Bio: {{ bio }}");

  loader->set(
      "empty_loop",
      "Start{% for item in items %}{{ item }}{% endfor %}End");

  return loader;
}

static void test_home_template_full_render()
{
  Engine engine(make_loader());

  Context ctx;
  ctx.set("name", "Alice");
  ctx.set("is_admin", true);

  Array items;
  items.emplace_back("Book");
  items.emplace_back("Phone");
  items.emplace_back("Pen");
  ctx.set("items", items);

  const auto result = engine.render("home", ctx);

  assert(result.success);
  assert(!result.from_cache);
  assert(result.escaped);
  assert(
      result.output ==
      "Hello Alice!\n"
      "Admin panel enabled.\n"
      "Items: [Book][Phone][Pen]");
}

static void test_home_template_without_admin()
{
  Engine engine(make_loader());

  Context ctx;
  ctx.set("name", "Bob");
  ctx.set("is_admin", false);

  Array items;
  items.emplace_back("A");
  items.emplace_back("B");
  ctx.set("items", items);

  const auto result = engine.render("home", ctx);

  assert(result.success);
  assert(
      result.output ==
      "Hello Bob!\n"
      "Items: [A][B]");
}

static void test_profile_template_with_escape()
{
  Engine engine(make_loader());

  Context ctx;
  ctx.set("username", "gaspard");
  ctx.set("bio", "<b>builder</b>");

  const auto result = engine.render("profile", ctx);

  assert(result.success);
  assert(
      result.output ==
      "User: gaspard\n"
      "Bio: &lt;b&gt;builder&lt;/b&gt;");
}

static void test_empty_loop_render()
{
  Engine engine(make_loader());

  Context ctx;
  ctx.set("items", Array{});

  const auto result = engine.render("empty_loop", ctx);

  assert(result.success);
  assert(result.output == "StartEnd");
}

static void test_render_with_object_context()
{
  Engine engine(make_loader());

  Object values;
  values["username"] = "adastra";
  values["bio"] = "offline-first";

  const auto result = engine.render("profile", values);

  assert(result.success);
  assert(
      result.output ==
      "User: adastra\n"
      "Bio: offline-first");
}

int main()
{
  test_home_template_full_render();
  test_home_template_without_admin();
  test_profile_template_with_escape();
  test_empty_loop_render();
  test_render_with_object_context();

  std::cout << "[OK] template integration render tests passed\n";
  return 0;
}

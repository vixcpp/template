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

  const RenderResult result = engine.render("home", ctx);

  assert(result.success);
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

  const RenderResult result = engine.render("home", ctx);

  assert(result.success);
  assert(result.escaped);
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

  const RenderResult result = engine.render("profile", ctx);

  assert(result.success);
  assert(result.escaped);
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

  const RenderResult result = engine.render("empty_loop", ctx);

  assert(result.success);
  assert(result.escaped);
  assert(result.output == "StartEnd");
}

static void test_render_with_object_context()
{
  Engine engine(make_loader());

  Object values;
  values["username"] = Value("adastra");
  values["bio"] = Value("offline-first");

  const RenderResult result = engine.render("profile", Context(values));

  assert(result.success);
  assert(result.escaped);
  assert(
      result.output ==
      "User: adastra\n"
      "Bio: offline-first");
}

static void test_second_render_uses_cache()
{
  auto loader = make_loader();
  Engine engine(loader);

  Context ctx;
  ctx.set("username", "gaspard");
  ctx.set("bio", "builder");

  const RenderResult first = engine.render("profile", ctx);
  const RenderResult second = engine.render("profile", ctx);

  assert(first.success);
  assert(second.success);

  assert(first.output == "User: gaspard\nBio: builder");
  assert(second.output == "User: gaspard\nBio: builder");

  assert(!first.from_cache);
  assert(second.from_cache);
}

static void test_cache_invalidation_after_template_update()
{
  auto loader = make_loader();
  Engine engine(loader);

  Context ctx;
  ctx.set("username", "gaspard");
  ctx.set("bio", "builder");

  const RenderResult first = engine.render("profile", ctx);
  assert(first.success);
  assert(first.output == "User: gaspard\nBio: builder");

  loader->set(
      "profile",
      "Account: {{ username }}\n"
      "About: {{ bio }}");

  const RenderResult second = engine.render("profile", ctx);

  assert(second.success);
  assert(second.escaped);
  assert(
      second.output ==
      "Account: gaspard\n"
      "About: builder");

  assert(!second.from_cache);
}

static void test_cache_can_be_disabled()
{
  auto loader = make_loader();
  Engine engine(loader);
  engine.set_cache_enabled(false);

  Context ctx;
  ctx.set("username", "gaspard");
  ctx.set("bio", "builder");

  const RenderResult first = engine.render("profile", ctx);
  const RenderResult second = engine.render("profile", ctx);

  assert(first.success);
  assert(second.success);

  assert(first.output == "User: gaspard\nBio: builder");
  assert(second.output == "User: gaspard\nBio: builder");

  assert(!first.from_cache);
  assert(!second.from_cache);
  assert(engine.cache().empty());
}

int main()
{
  test_home_template_full_render();
  test_home_template_without_admin();
  test_profile_template_with_escape();
  test_empty_loop_render();
  test_render_with_object_context();
  test_second_render_uses_cache();
  test_cache_invalidation_after_template_update();
  test_cache_can_be_disabled();

  std::cout << "[OK] template integration render tests passed\n";
  return 0;
}

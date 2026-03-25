/**
 *
 *  @file renderer_test.cpp
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
#include <string>

#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Renderer.hpp>
#include <vix/template/Context.hpp>

using namespace vix::template_;

static std::string render(const std::string &tpl, const Context &ctx)
{
  Lexer lexer(tpl);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  RootNode root = parser.parse();

  Renderer renderer(true);
  return renderer.render(root, ctx).output;
}

static void test_render_text()
{
  Context ctx;
  const std::string out = render("Hello world", ctx);
  assert(out == "Hello world");
}

static void test_render_variable()
{
  Context ctx;
  ctx.set("name", "Alice");

  const std::string out = render("Hello {{ name }}", ctx);
  assert(out == "Hello Alice");
}

static void test_render_missing_variable()
{
  Context ctx;
  const std::string out = render("Hello {{ name }}", ctx);
  assert(out == "Hello ");
}

static void test_render_if_true()
{
  Context ctx;
  ctx.set("user", true);

  const std::string out = render("{% if user %}OK{% endif %}", ctx);
  assert(out == "OK");
}

static void test_render_if_false()
{
  Context ctx;
  ctx.set("user", false);

  const std::string out = render("{% if user %}OK{% endif %}", ctx);
  assert(out.empty());
}

static void test_render_for()
{
  Context ctx;
  Array arr;
  arr.emplace_back("A");
  arr.emplace_back("B");
  arr.emplace_back("C");

  ctx.set("items", arr);

  const std::string out =
      render("{% for item in items %}{{ item }}{% endfor %}", ctx);

  assert(out == "ABC");
}

static void test_render_nested()
{
  Context ctx;
  Array arr;
  arr.emplace_back("X");
  arr.emplace_back("");
  arr.emplace_back("Y");

  ctx.set("items", arr);

  const std::string out =
      render("{% for item in items %}{% if item %}{{ item }}{% endif %}{% endfor %}", ctx);

  assert(out == "XY");
}

static void test_html_escape()
{
  Context ctx;
  ctx.set("html", "<b>test</b>");

  const std::string out = render("{{ html }}", ctx);
  assert(out == "&lt;b&gt;test&lt;/b&gt;");
}

int main()
{
  test_render_text();
  test_render_variable();
  test_render_missing_variable();
  test_render_if_true();
  test_render_if_false();
  test_render_for();
  test_render_nested();
  test_html_escape();

  std::cout << "[OK] template renderer tests passed\n";
  return 0;
}

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
#include <memory>
#include <string>

#include <vix/template/Context.hpp>
#include <vix/template/Error.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Renderer.hpp>
#include <vix/template/StringLoader.hpp>

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

static std::string render_with_loader(
    const std::string &tpl,
    const Context &ctx,
    const std::shared_ptr<Loader> &loader)
{
  Lexer lexer(tpl);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  RootNode root = parser.parse();

  Renderer renderer(true, loader);
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

static void test_render_upper_filter()
{
  Context ctx;
  ctx.set("name", "Alice");

  const std::string out = render("{{ name | upper }}", ctx);
  assert(out == "ALICE");
}

static void test_render_lower_filter()
{
  Context ctx;
  ctx.set("name", "ALICE");

  const std::string out = render("{{ name | lower }}", ctx);
  assert(out == "alice");
}

static void test_render_length_filter_on_string()
{
  Context ctx;
  ctx.set("name", "Alice");

  const std::string out = render("{{ name | length }}", ctx);
  assert(out == "5");
}

static void test_render_length_filter_on_array()
{
  Context ctx;
  Array arr;
  arr.emplace_back("A");
  arr.emplace_back("B");
  arr.emplace_back("C");

  ctx.set("items", arr);

  const std::string out = render("{{ items | length }}", ctx);
  assert(out == "3");
}

static void test_render_chained_filters()
{
  Context ctx;
  ctx.set("name", "Alice");

  const std::string out = render("{{ name | upper | length }}", ctx);
  assert(out == "5");
}

static void test_render_default_filter_on_missing_variable()
{
  Context ctx;

  const std::string out = render("{{ missing | default }}", ctx);
  assert(out == "");
}

static void test_render_default_filter_on_empty_string()
{
  Context ctx;
  ctx.set("name", "");

  const std::string out = render("{{ name | default }}", ctx);
  assert(out == "");
}

static void test_render_include_simple()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("header.html", "<header>Header</header>");

  Context ctx;
  const std::string out =
      render_with_loader("{% include \"header.html\" %}", ctx, loader);

  assert(out == "<header>Header</header>");
}

static void test_render_include_with_context()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("header.html", "Hello {{ name }}");

  Context ctx;
  ctx.set("name", "Alice");

  const std::string out =
      render_with_loader("{% include \"header.html\" %}", ctx, loader);

  assert(out == "Hello Alice");
}

static void test_render_include_missing_template()
{
  auto loader = std::make_shared<StringLoader>();

  Context ctx;

  bool thrown = false;
  try
  {
    (void)render_with_loader("{% include \"missing.html\" %}", ctx, loader);
  }
  catch (const RendererError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_render_circular_include()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("a.html", "A {% include \"b.html\" %}");
  loader->set("b.html", "B {% include \"a.html\" %}");

  Context ctx;

  bool thrown = false;
  try
  {
    (void)render_with_loader("{% include \"a.html\" %}", ctx, loader);
  }
  catch (const RendererError &)
  {
    thrown = true;
  }

  assert(thrown);
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
  test_render_upper_filter();
  test_render_lower_filter();
  test_render_length_filter_on_string();
  test_render_length_filter_on_array();
  test_render_chained_filters();
  test_render_default_filter_on_missing_variable();
  test_render_default_filter_on_empty_string();
  test_render_include_simple();
  test_render_include_with_context();
  test_render_include_missing_template();
  test_render_circular_include();

  std::cout << "[OK] template renderer tests passed\n";
  return 0;
}

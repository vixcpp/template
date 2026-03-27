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
#include <utility>

#include <vix/template/Cache.hpp>
#include <vix/template/Compiler.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/Error.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/StringLoader.hpp>
#include <vix/template/Template.hpp>

using namespace vix::template_;

static Template compile_template(
    const std::string &tpl,
    const std::shared_ptr<Loader> &loader = nullptr,
    const std::string &name = "inline",
    const std::string &source_signature = {})
{
  Lexer lexer(tpl);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  RootNode root = parser.parse();

  Compiler compiler;
  return compiler.compile(name, std::move(root), loader, source_signature);
}

static std::string render(
    const std::string &tpl,
    const Context &ctx,
    bool auto_escape_html = true)
{
  Template compiled = compile_template(tpl);
  return compiled.render(ctx, auto_escape_html).output;
}

static std::string render_with_loader(
    const std::string &tpl,
    const Context &ctx,
    const std::shared_ptr<Loader> &loader,
    bool auto_escape_html = true,
    const std::string &name = "inline")
{
  const std::string signature =
      loader ? loader->source_signature(name) : std::string{};

  Template compiled = compile_template(
      tpl,
      loader,
      name,
      signature);

  return compiled.render(ctx, auto_escape_html).output;
}

static std::string render_with_loader_and_cache(
    const std::string &tpl,
    const Context &ctx,
    const std::shared_ptr<Loader> &loader,
    Cache &cache,
    bool auto_escape_html = true,
    const std::string &name = "inline")
{
  const std::string signature =
      loader ? loader->source_signature(name) : std::string{};

  Template compiled = compile_template(
      tpl,
      loader,
      name,
      signature);

  cache.put(
      name,
      std::make_shared<Template>(std::move(compiled)),
      signature);

  return compiled.render(ctx, auto_escape_html).output;
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

static void test_render_member_expression()
{
  Context ctx;
  Object user;
  user["name"] = Value("Alice");
  ctx.set("user", user);

  const std::string out = render("Hello {{ user.name }}", ctx);
  assert(out == "Hello Alice");
}

static void test_render_nested_member_expression()
{
  Context ctx;

  Object customer;
  customer["name"] = Value("Alice");

  Object order;
  order["customer"] = Value(customer);

  ctx.set("order", order);

  const std::string out = render("{{ order.customer.name }}", ctx);
  assert(out == "Alice");
}

static void test_render_missing_member_expression()
{
  Context ctx;
  Object user;
  ctx.set("user", user);

  const std::string out = render("Hello {{ user.name }}", ctx);
  assert(out == "Hello ");
}

static void test_render_integer_literal()
{
  Context ctx;
  const std::string out = render("{{ 123 }}", ctx);
  assert(out == "123");
}

static void test_render_string_literal()
{
  Context ctx;
  const std::string out = render("{{ \"hello\" }}", ctx);
  assert(out == "hello");
}

static void test_render_boolean_literal_true()
{
  Context ctx;
  const std::string out = render("{{ true }}", ctx);
  assert(out == "true");
}

static void test_render_boolean_literal_false()
{
  Context ctx;
  const std::string out = render("{{ false }}", ctx);
  assert(out == "false");
}

static void test_render_addition()
{
  Context ctx;
  ctx.set("a", 2);
  ctx.set("b", 3);

  const std::string out = render("{{ a + b }}", ctx);
  assert(out == "5");
}

static void test_render_subtraction()
{
  Context ctx;
  ctx.set("a", 10);
  ctx.set("b", 4);

  const std::string out = render("{{ a - b }}", ctx);
  assert(out == "6");
}

static void test_render_multiplication()
{
  Context ctx;
  ctx.set("price", 6);
  ctx.set("quantity", 7);

  const std::string out = render("{{ price * quantity }}", ctx);
  assert(out == "42");
}

static void test_render_division()
{
  Context ctx;
  ctx.set("a", 8);
  ctx.set("b", 2);

  const std::string out = render("{{ a / b }}", ctx);
  assert(out == "4");
}

static void test_render_modulo()
{
  Context ctx;
  ctx.set("a", 10);
  ctx.set("b", 3);

  const std::string out = render("{{ a % b }}", ctx);
  assert(out == "1");
}

static void test_render_unary_not_true()
{
  Context ctx;
  ctx.set("enabled", true);

  const std::string out = render("{{ !enabled }}", ctx);
  assert(out == "false");
}

static void test_render_unary_not_false()
{
  Context ctx;
  ctx.set("enabled", false);

  const std::string out = render("{{ !enabled }}", ctx);
  assert(out == "true");
}

static void test_render_unary_minus()
{
  Context ctx;
  ctx.set("price", 5);

  const std::string out = render("{{ -price }}", ctx);
  assert(out == "-5");
}

static void test_render_comparison_equal()
{
  Context ctx;
  ctx.set("a", 5);
  ctx.set("b", 5);

  const std::string out = render("{{ a == b }}", ctx);
  assert(out == "true");
}

static void test_render_comparison_not_equal()
{
  Context ctx;
  ctx.set("a", 5);
  ctx.set("b", 7);

  const std::string out = render("{{ a != b }}", ctx);
  assert(out == "true");
}

static void test_render_comparison_greater()
{
  Context ctx;
  ctx.set("a", 7);
  ctx.set("b", 3);

  const std::string out = render("{{ a > b }}", ctx);
  assert(out == "true");
}

static void test_render_comparison_less_equal()
{
  Context ctx;
  ctx.set("a", 3);
  ctx.set("b", 3);

  const std::string out = render("{{ a <= b }}", ctx);
  assert(out == "true");
}

static void test_render_logical_and()
{
  Context ctx;
  ctx.set("a", true);
  ctx.set("b", true);

  const std::string out = render("{{ a && b }}", ctx);
  assert(out == "true");
}

static void test_render_logical_or()
{
  Context ctx;
  ctx.set("a", false);
  ctx.set("b", true);

  const std::string out = render("{{ a || b }}", ctx);
  assert(out == "true");
}

static void test_render_parenthesized_expression()
{
  Context ctx;
  ctx.set("price", 10);
  ctx.set("tax", 2);
  ctx.set("quantity", 3);

  const std::string out = render("{{ (price + tax) * quantity }}", ctx);
  assert(out == "36");
}

static void test_render_operator_precedence()
{
  Context ctx;
  ctx.set("a", 2);
  ctx.set("b", 3);
  ctx.set("c", 4);

  const std::string out = render("{{ a + b * c }}", ctx);
  assert(out == "14");
}

static void test_render_string_concatenation()
{
  Context ctx;
  ctx.set("first", "Hello ");
  ctx.set("second", "World");

  const std::string out = render("{{ first + second }}", ctx);
  assert(out == "Hello World");
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

static void test_render_if_with_expression_true()
{
  Context ctx;
  ctx.set("price", 6);
  ctx.set("quantity", 7);

  const std::string out =
      render("{% if price * quantity > 40 %}OK{% endif %}", ctx);

  assert(out == "OK");
}

static void test_render_if_with_expression_false()
{
  Context ctx;
  ctx.set("price", 2);
  ctx.set("quantity", 3);

  const std::string out =
      render("{% if price * quantity > 40 %}OK{% endif %}", ctx);

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

static void test_render_without_html_escape()
{
  Context ctx;
  ctx.set("html", "<b>test</b>");

  const std::string out = render("{{ html }}", ctx, false);
  assert(out == "<b>test</b>");
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

static void test_render_expression_with_filter()
{
  Context ctx;
  ctx.set("name", "alice");

  const std::string out = render("{{ name | upper }}", ctx);
  assert(out == "ALICE");
}

static void test_render_member_expression_with_filter()
{
  Context ctx;
  Object user;
  user["name"] = Value("alice");
  ctx.set("user", user);

  const std::string out = render("{{ user.name | upper }}", ctx);
  assert(out == "ALICE");
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

static void test_render_include_with_expression()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("header.html", "{{ price * quantity }}");

  Context ctx;
  ctx.set("price", 6);
  ctx.set("quantity", 7);

  const std::string out =
      render_with_loader("{% include \"header.html\" %}", ctx, loader);

  assert(out == "42");
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

static void test_render_block_default()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("base.html",
              "<html>{% block content %}Default{% endblock %}</html>");

  Context ctx;

  const std::string out =
      render_with_loader("{% include \"base.html\" %}", ctx, loader);

  assert(out == "<html>Default</html>");
}

static void test_render_extends_override_block()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}Default{% endblock %}</html>");

  Context ctx;

  const std::string out =
      render_with_loader(
          "{% extends \"base.html\" %}"
          "{% block content %}Hello{% endblock %}",
          ctx,
          loader);

  assert(out == "<html>Hello</html>");
}

static void test_render_extends_with_variable()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}{% endblock %}</html>");

  Context ctx;
  ctx.set("name", "Alice");

  const std::string out =
      render_with_loader(
          "{% extends \"base.html\" %}"
          "{% block content %}Hello {{ name }}{% endblock %}",
          ctx,
          loader);

  assert(out == "<html>Hello Alice</html>");
}

static void test_render_extends_with_expression()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}{% endblock %}</html>");

  Context ctx;
  ctx.set("price", 6);
  ctx.set("quantity", 7);

  const std::string out =
      render_with_loader(
          "{% extends \"base.html\" %}"
          "{% block content %}{{ price * quantity }}{% endblock %}",
          ctx,
          loader);

  assert(out == "<html>42</html>");
}

static void test_render_extends_fallback_to_parent_block()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}Default{% endblock %}</html>");

  Context ctx;

  const std::string out =
      render_with_loader(
          "{% extends \"base.html\" %}",
          ctx,
          loader);

  assert(out == "<html>Default</html>");
}

static void test_render_extends_multiple_blocks()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>"
              "{% block header %}H{% endblock %}"
              "{% block content %}C{% endblock %}"
              "</html>");

  Context ctx;

  const std::string out =
      render_with_loader(
          "{% extends \"base.html\" %}"
          "{% block content %}X{% endblock %}",
          ctx,
          loader);

  assert(out == "<html>HX</html>");
}

static void test_render_extends_nested()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}Base{% endblock %}</html>");

  loader->set("layout.html",
              "{% extends \"base.html\" %}"
              "{% block content %}Layout{% endblock %}");

  Context ctx;

  const std::string out =
      render_with_loader(
          "{% extends \"layout.html\" %}"
          "{% block content %}Page{% endblock %}",
          ctx,
          loader);

  assert(out == "<html>Page</html>");
}

static void test_render_extends_invalid_child_content()
{
  auto loader = std::make_shared<StringLoader>();

  loader->set("base.html",
              "<html>{% block content %}{% endblock %}</html>");

  Context ctx;

  bool thrown = false;
  try
  {
    (void)render_with_loader(
        "{% extends \"base.html\" %}INVALID",
        ctx,
        loader);
  }
  catch (const RendererError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_render_division_by_zero()
{
  Context ctx;
  ctx.set("a", 8);
  ctx.set("b", 0);

  bool thrown = false;
  try
  {
    (void)render("{{ a / b }}", ctx);
  }
  catch (const RendererError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_render_modulo_by_zero()
{
  Context ctx;
  ctx.set("a", 8);
  ctx.set("b", 0);

  bool thrown = false;
  try
  {
    (void)render("{{ a % b }}", ctx);
  }
  catch (const RendererError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_template_source_signature_is_preserved()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("inline", "Hello {{ name }}");

  Template compiled = compile_template(
      "Hello {{ name }}",
      loader,
      "inline",
      loader->source_signature("inline"));

  assert(!compiled.source_signature().empty());
  assert(compiled.source_signature() == loader->source_signature("inline"));
}

static void test_cache_put_compiled_template()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("inline", "Hello {{ name }}");

  const std::string signature = loader->source_signature("inline");

  Template compiled = compile_template(
      "Hello {{ name }}",
      loader,
      "inline",
      signature);

  Cache cache;
  cache.put(
      "inline",
      std::make_shared<Template>(std::move(compiled)),
      signature);

  auto cached = cache.get("inline", signature);

  assert(cached != nullptr);
  assert(cached->name() == "inline");
  assert(cached->source_signature() == signature);
}

static void test_cache_invalidation_on_source_change()
{
  auto loader = std::make_shared<StringLoader>();
  loader->set("inline", "Hello {{ name }}");

  const std::string signature1 = loader->source_signature("inline");

  Template compiled1 = compile_template(
      "Hello {{ name }}",
      loader,
      "inline",
      signature1);

  Cache cache;
  cache.put(
      "inline",
      std::make_shared<Template>(std::move(compiled1)),
      signature1);

  assert(cache.contains("inline", signature1));

  loader->set("inline", "Hi {{ name }}");
  const std::string signature2 = loader->source_signature("inline");

  assert(signature1 != signature2);
  assert(!cache.contains("inline", signature2));

  Template compiled2 = compile_template(
      "Hi {{ name }}",
      loader,
      "inline",
      signature2);

  cache.put(
      "inline",
      std::make_shared<Template>(std::move(compiled2)),
      signature2);

  auto cached = cache.get("inline", signature2);
  assert(cached != nullptr);
  assert(cached->source_signature() == signature2);
}

int main()
{
  test_render_text();
  test_render_variable();
  test_render_missing_variable();
  test_render_member_expression();
  test_render_nested_member_expression();
  test_render_missing_member_expression();
  test_render_integer_literal();
  test_render_string_literal();
  test_render_boolean_literal_true();
  test_render_boolean_literal_false();
  test_render_addition();
  test_render_subtraction();
  test_render_multiplication();
  test_render_division();
  test_render_modulo();
  test_render_unary_not_true();
  test_render_unary_not_false();
  test_render_unary_minus();
  test_render_comparison_equal();
  test_render_comparison_not_equal();
  test_render_comparison_greater();
  test_render_comparison_less_equal();
  test_render_logical_and();
  test_render_logical_or();
  test_render_parenthesized_expression();
  test_render_operator_precedence();
  test_render_string_concatenation();
  test_render_if_true();
  test_render_if_false();
  test_render_if_with_expression_true();
  test_render_if_with_expression_false();
  test_render_for();
  test_render_nested();
  test_html_escape();
  test_render_without_html_escape();
  test_render_upper_filter();
  test_render_lower_filter();
  test_render_length_filter_on_string();
  test_render_length_filter_on_array();
  test_render_expression_with_filter();
  test_render_member_expression_with_filter();
  test_render_chained_filters();
  test_render_default_filter_on_missing_variable();
  test_render_default_filter_on_empty_string();
  test_render_include_simple();
  test_render_include_with_context();
  test_render_include_with_expression();
  test_render_include_missing_template();
  test_render_circular_include();
  test_render_block_default();
  test_render_extends_override_block();
  test_render_extends_with_variable();
  test_render_extends_with_expression();
  test_render_extends_fallback_to_parent_block();
  test_render_extends_multiple_blocks();
  test_render_extends_nested();
  test_render_extends_invalid_child_content();
  test_render_division_by_zero();
  test_render_modulo_by_zero();
  test_template_source_signature_is_preserved();
  test_cache_put_compiled_template();
  test_cache_invalidation_on_source_change();

  std::cout << "[OK] template renderer tests passed\n";
  return 0;
}

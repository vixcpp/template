/**
 *
 *  @file escape_test.cpp
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

#include <vix/template/Escape.hpp>

using namespace vix::template_;

static void test_needs_html_escape()
{
  assert(Escape::needs_html_escape('&'));
  assert(Escape::needs_html_escape('<'));
  assert(Escape::needs_html_escape('>'));
  assert(Escape::needs_html_escape('"'));
  assert(Escape::needs_html_escape('\''));

  assert(!Escape::needs_html_escape('a'));
  assert(!Escape::needs_html_escape('1'));
  assert(!Escape::needs_html_escape(' '));
}

static void test_html_escape_empty()
{
  const std::string result = Escape::html("");
  assert(result.empty());
}

static void test_html_escape_plain_text()
{
  const std::string result = Escape::html("hello world");
  assert(result == "hello world");
}

static void test_html_escape_special_chars()
{
  const std::string result = Escape::html(R"(<div class="x">Tom & 'Jerry'</div>)");
  assert(result == "&lt;div class=&quot;x&quot;&gt;Tom &amp; &#39;Jerry&#39;&lt;/div&gt;");
}

static void test_html_escape_mixed()
{
  const std::string result = Escape::html("A&B<C>D\"E'F");
  assert(result == "A&amp;B&lt;C&gt;D&quot;E&#39;F");
}

int main()
{
  test_needs_html_escape();
  test_html_escape_empty();
  test_html_escape_plain_text();
  test_html_escape_special_chars();
  test_html_escape_mixed();

  std::cout << "[OK] template escape tests passed\n";
  return 0;
}

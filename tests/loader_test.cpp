/**
 *
 *  @file loader_test.cpp
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
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <vix/template/Error.hpp>
#include <vix/template/FileSystemLoader.hpp>
#include <vix/template/StringLoader.hpp>

using namespace vix::template_;

static void test_string_loader_set_and_load()
{
  StringLoader loader;
  loader.set("home", "Hello {{ name }}");

  assert(loader.exists("home"));
  assert(loader.load("home") == "Hello {{ name }}");
}

static void test_string_loader_missing_template()
{
  StringLoader loader;

  bool thrown = false;
  try
  {
    (void)loader.load("missing");
  }
  catch (const LoaderError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_filesystem_loader_load_and_exists()
{
  const std::string path = "template_loader_test_file.html";

  {
    std::ofstream out(path);
    assert(out.good());
    out << "Hello from file";
  }

  FileSystemLoader loader(".");

  assert(loader.exists(path));
  assert(loader.load(path) == "Hello from file");

  const int rc = std::remove(path.c_str());
  assert(rc == 0);
}

static void test_filesystem_loader_missing_template()
{
  FileSystemLoader loader(".");

  assert(!loader.exists("definitely_missing_template_file_12345.html"));

  bool thrown = false;
  try
  {
    (void)loader.load("definitely_missing_template_file_12345.html");
  }
  catch (const LoaderError &)
  {
    thrown = true;
  }

  assert(thrown);
}

static void test_filesystem_loader_root()
{
  FileSystemLoader loader("./templates");
  assert(loader.root() == "./templates");
}

int main()
{
  test_string_loader_set_and_load();
  test_string_loader_missing_template();
  test_filesystem_loader_load_and_exists();
  test_filesystem_loader_missing_template();
  test_filesystem_loader_root();

  std::cout << "[OK] template loader tests passed\n";
  return 0;
}

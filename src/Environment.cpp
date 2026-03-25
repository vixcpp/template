/**
 *
 *  @file Environment.cpp
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
#include <vix/template/Environment.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Error.hpp>

#include <utility>

namespace vix::template_
{
  Environment::Environment(std::shared_ptr<Loader> loader)
      : loader_(std::move(loader))
  {
    if (!loader_)
    {
      throw LoaderError("environment requires a valid loader");
    }
  }

  void Environment::set_auto_escape(bool enabled) noexcept
  {
    auto_escape_ = enabled;
  }

  bool Environment::auto_escape() const noexcept
  {
    return auto_escape_;
  }

  Template Environment::load(const std::string &name) const
  {
    if (!loader_)
    {
      throw LoaderError("no loader configured");
    }

    if (!loader_->exists(name))
    {
      throw LoaderError("template not found: " + name);
    }

    // 1. Load raw template source
    std::string source = loader_->load(name);

    // 2. Lexing
    Lexer lexer(std::move(source));
    auto tokens = lexer.tokenize();

    // 3. Parsing
    Parser parser(std::move(tokens));
    RootNode root = parser.parse();

    // 4. Build template
    return Template(name, std::move(root));
  }

  const std::shared_ptr<Loader> &Environment::loader() const noexcept
  {
    return loader_;
  }

} // namespace vix::template_

/**
 *
 *  @file Template.cpp
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
#include <vix/template/Template.hpp>
#include <vix/template/Renderer.hpp>

#include <utility>

namespace vix::template_
{
  Template::Template(std::string name, RootNode root)
      : name_(std::move(name)),
        root_(std::move(root))
  {
  }

  const std::string &Template::name() const noexcept
  {
    return name_;
  }

  const RootNode &Template::root() const noexcept
  {
    return root_;
  }

  bool Template::empty() const noexcept
  {
    return root_.children().empty();
  }

  RenderResult Template::render(
      const Context &context,
      bool auto_escape_html) const
  {
    Renderer renderer(auto_escape_html);
    return renderer.render(root_, context);
  }

} // namespace vix::template_

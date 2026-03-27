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

#include <algorithm>
#include <utility>

namespace vix::template_
{
  Template::Template(
      std::string name,
      RootNode root,
      ExecutionPlan plan,
      std::shared_ptr<Loader> loader,
      std::string source_signature)
      : name_(std::move(name)),
        root_(std::move(root)),
        plan_(std::move(plan)),
        loader_(std::move(loader)),
        source_signature_(std::move(source_signature))
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

  const ExecutionPlan &Template::plan() const noexcept
  {
    return plan_;
  }

  bool Template::empty() const noexcept
  {
    return plan_.empty();
  }

  RenderResult Template::render(
      const Context &context,
      bool auto_escape_html) const
  {
    Renderer renderer(auto_escape_html, loader_);

    const bool has_extends =
        std::any_of(
            root_.children().begin(),
            root_.children().end(),
            [](const NodePtr &node)
            {
              return node && node->type() == NodeType::Extends;
            });

    if (has_extends)
    {
      return renderer.render(root_, context);
    }

    return renderer.render(plan_, context);
  }

  const std::shared_ptr<Loader> &Template::loader() const noexcept
  {
    return loader_;
  }

  const std::string &Template::source_signature() const noexcept
  {
    return source_signature_;
  }

  void Template::set_source_signature(std::string source_signature)
  {
    source_signature_ = std::move(source_signature);
  }

} // namespace vix::template_

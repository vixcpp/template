/**
 *
 *  @file Renderer.cpp
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
#include <vix/template/Renderer.hpp>
#include <vix/template/Escape.hpp>
#include <vix/template/Error.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>

#include <algorithm>
#include <utility>

namespace vix::template_
{
  Renderer::Renderer(
      bool auto_escape_html,
      std::shared_ptr<Loader> loader)
      : auto_escape_html_(auto_escape_html),
        filters_(Builtins::filters()),
        loader_(std::move(loader))
  {
  }

  RenderResult Renderer::render(
      const RootNode &root,
      const Context &context) const
  {
    RenderResult result;
    result.escaped = auto_escape_html_;
    result.from_cache = false;
    result.success = true;

    render_root(root, context, result.output);
    return result;
  }

  void Renderer::render_node(
      const Node &node,
      const Context &context,
      std::string &output) const
  {
    switch (node.type())
    {
    case NodeType::Root:
      render_root(static_cast<const RootNode &>(node), context, output);
      break;

    case NodeType::Text:
      render_text(static_cast<const TextNode &>(node), output);
      break;

    case NodeType::Variable:
      render_variable(static_cast<const VariableNode &>(node), context, output);
      break;

    case NodeType::If:
      render_if(static_cast<const IfNode &>(node), context, output);
      break;

    case NodeType::For:
      render_for(static_cast<const ForNode &>(node), context, output);
      break;

    case NodeType::Include:
      render_include(static_cast<const IncludeNode &>(node), context, output);
      break;

    default:
      throw RendererError("unsupported AST node type");
    }
  }

  void Renderer::render_root(
      const RootNode &node,
      const Context &context,
      std::string &output) const
  {
    for (const auto &child : node.children())
    {
      if (!child)
      {
        throw RendererError("null child node in root");
      }

      render_node(*child, context, output);
    }
  }

  void Renderer::render_text(
      const TextNode &node,
      std::string &output) const
  {
    output += node.value();
  }

  void Renderer::render_variable(
      const VariableNode &node,
      const Context &context,
      std::string &output) const
  {
    const Value *value = resolve_variable(node.name(), context);

    const Value initial = (value != nullptr) ? *value : Value{};
    Value transformed = apply_filters(initial, node.filters());

    std::string rendered = transformed.to_string();
    if (auto_escape_html_)
    {
      rendered = Escape::html(rendered);
    }

    output += rendered;
  }

  void Renderer::render_if(
      const IfNode &node,
      const Context &context,
      std::string &output) const
  {
    const Value *value = resolve_variable(node.condition(), context);
    if (value == nullptr || !value->truthy())
    {
      return;
    }

    for (const auto &child : node.body())
    {
      if (!child)
      {
        throw RendererError("null child node in if body");
      }

      render_node(*child, context, output);
    }
  }

  void Renderer::render_for(
      const ForNode &node,
      const Context &context,
      std::string &output) const
  {
    const Value *iterable = resolve_variable(node.iterable_name(), context);
    if (iterable == nullptr)
    {
      return;
    }

    if (!iterable->is_array())
    {
      throw RendererError("for loop iterable must be an array");
    }

    for (const auto &item : iterable->as_array())
    {
      Context loop_context = context;
      loop_context.set(node.item_name(), item);

      for (const auto &child : node.body())
      {
        if (!child)
        {
          throw RendererError("null child node in for body");
        }

        render_node(*child, loop_context, output);
      }
    }
  }

  void Renderer::render_include(
      const IncludeNode &node,
      const Context &context,
      std::string &output) const
  {
    if (!loader_)
    {
      throw RendererError("include requires a configured loader");
    }

    const std::string &template_name = node.template_name();

    if (is_in_include_stack(template_name))
    {
      throw RendererError("circular include detected: " + template_name);
    }

    if (!loader_->exists(template_name))
    {
      throw RendererError("included template not found: " + template_name);
    }

    include_stack_.push_back(template_name);

    try
    {
      const std::string source = loader_->load(template_name);

      Lexer lexer(source);
      auto tokens = lexer.tokenize();

      Parser parser(std::move(tokens));
      RootNode root = parser.parse();

      render_root(root, context, output);

      include_stack_.pop_back();
    }
    catch (...)
    {
      include_stack_.pop_back();
      throw;
    }
  }

  const Value *Renderer::resolve_variable(
      const std::string &name,
      const Context &context) const noexcept
  {
    return context.get(name);
  }

  Value Renderer::apply_filters(
      const Value &input,
      const std::vector<FilterNode> &filters) const
  {
    Value current = input;

    for (const auto &filter : filters)
    {
      const auto it = filters_.find(filter.name());
      if (it == filters_.end())
      {
        throw RendererError("unknown filter: " + filter.name());
      }

      current = it->second(current);
    }

    return current;
  }

  bool Renderer::is_in_include_stack(
      const std::string &template_name) const noexcept
  {
    return std::find(
               include_stack_.begin(),
               include_stack_.end(),
               template_name) != include_stack_.end();
  }

} // namespace vix::template_

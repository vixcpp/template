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
#include <cmath>
#include <cstdlib>
#include <utility>

namespace vix::template_
{
  namespace
  {
    [[nodiscard]] bool is_integer_literal(const std::string &value) noexcept
    {
      if (value.empty())
      {
        return false;
      }

      std::size_t index = 0;
      if (value[index] == '+' || value[index] == '-')
      {
        ++index;
      }

      if (index >= value.size())
      {
        return false;
      }

      for (; index < value.size(); ++index)
      {
        const char c = value[index];
        if (c < '0' || c > '9')
        {
          return false;
        }
      }

      return true;
    }

    [[nodiscard]] bool is_floating_literal(const std::string &value) noexcept
    {
      if (value.empty())
      {
        return false;
      }

      bool seen_dot = false;
      std::size_t index = 0;

      if (value[index] == '+' || value[index] == '-')
      {
        ++index;
      }

      if (index >= value.size())
      {
        return false;
      }

      for (; index < value.size(); ++index)
      {
        const char c = value[index];
        if (c == '.')
        {
          if (seen_dot)
          {
            return false;
          }

          seen_dot = true;
          continue;
        }

        if (c < '0' || c > '9')
        {
          return false;
        }
      }

      return seen_dot;
    }

    [[nodiscard]] bool are_both_integers(const Value &lhs, const Value &rhs) noexcept
    {
      return lhs.is_int() && rhs.is_int();
    }
  } // namespace

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

    const ExtendsNode *extends_node = find_extends(root);

    if (extends_node != nullptr)
    {
      if (!loader_)
      {
        throw RendererError("extends requires a configured loader");
      }

      const BlockMap previous = block_overrides_;
      block_overrides_ = collect_blocks(root);

      try
      {
        render_template_by_name(
            extends_node->template_name(),
            context,
            result.output);
      }
      catch (...)
      {
        block_overrides_ = previous;
        throw;
      }

      block_overrides_ = previous;
      return result;
    }

    const BlockMap previous = block_overrides_;
    block_overrides_.clear();

    try
    {
      render_root(root, context, result.output);
    }
    catch (...)
    {
      block_overrides_ = previous;
      throw;
    }

    block_overrides_ = previous;
    return result;
  }

  RenderResult Renderer::render(
      const RootNode &root,
      const Context &context,
      const BlockMap &overrides) const
  {
    RenderResult result;
    result.escaped = auto_escape_html_;
    result.from_cache = false;
    result.success = true;

    const BlockMap previous = block_overrides_;
    block_overrides_ = overrides;

    try
    {
      render_root(root, context, result.output);
    }
    catch (...)
    {
      block_overrides_ = previous;
      throw;
    }

    block_overrides_ = previous;
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

    case NodeType::Block:
      render_block(static_cast<const BlockNode &>(node), context, output);
      break;

    case NodeType::Extends:
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
    Value current = evaluate_expression(node.expression(), context);
    current = apply_filters(current, node.filters());

    std::string rendered = current.to_string();
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
    const Value condition = evaluate_expression(node.condition(), context);
    if (!is_truthy(condition))
    {
      return;
    }

    render_node_list(node.body(), context, output);
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

      render_node_list(node.body(), loop_context, output);
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

  void Renderer::render_block(
      const BlockNode &node,
      const Context &context,
      std::string &output) const
  {
    const auto it = block_overrides_.find(node.name());

    if (it != block_overrides_.end() && it->second != nullptr)
    {
      if (it->second == &node)
      {
        render_node_list(node.body(), context, output);
        return;
      }

      render_node_list(it->second->body(), context, output);
      return;
    }

    render_node_list(node.body(), context, output);
  }

  void Renderer::render_node_list(
      const NodeList &nodes,
      const Context &context,
      std::string &output) const
  {
    for (const auto &child : nodes)
    {
      if (!child)
      {
        throw RendererError("null child node in node list");
      }

      render_node(*child, context, output);
    }
  }

  Value Renderer::evaluate_expression(
      const Expression &expr,
      const Context &context) const
  {
    switch (expr.type())
    {
    case ExprType::Name:
      return evaluate_name_expression(
          static_cast<const NameExpression &>(expr),
          context);

    case ExprType::Literal:
      return evaluate_literal_expression(
          static_cast<const LiteralExpression &>(expr));

    case ExprType::Member:
      return evaluate_member_expression(
          static_cast<const MemberExpression &>(expr),
          context);

    case ExprType::Unary:
      return evaluate_unary_expression(
          static_cast<const UnaryExpression &>(expr),
          context);

    case ExprType::Binary:
      return evaluate_binary_expression(
          static_cast<const BinaryExpression &>(expr),
          context);

    default:
      throw RendererError("unsupported expression type");
    }
  }

  Value Renderer::evaluate_name_expression(
      const NameExpression &expr,
      const Context &context) const
  {
    const Value *value = resolve_variable(expr.name(), context);
    if (value == nullptr)
    {
      return Value{};
    }

    return *value;
  }

  Value Renderer::evaluate_literal_expression(
      const LiteralExpression &expr) const
  {
    const std::string &raw = expr.value();

    if (raw == "true")
    {
      return Value(true);
    }

    if (raw == "false")
    {
      return Value(false);
    }

    if (is_integer_literal(raw))
    {
      return Value(static_cast<std::int64_t>(std::stoll(raw)));
    }

    if (is_floating_literal(raw))
    {
      return Value(std::stod(raw));
    }

    return Value(raw);
  }

  Value Renderer::evaluate_member_expression(
      const MemberExpression &expr,
      const Context &context) const
  {
    const Value object = evaluate_expression(expr.object(), context);
    const Value *member = resolve_member(object, expr.member());

    if (member == nullptr)
    {
      return Value{};
    }

    return *member;
  }

  Value Renderer::evaluate_unary_expression(
      const UnaryExpression &expr,
      const Context &context) const
  {
    const Value operand = evaluate_expression(expr.operand(), context);

    switch (expr.op())
    {
    case UnaryOperator::Plus:
      if (operand.is_int() || operand.is_double())
      {
        return operand;
      }

      return Value(to_number(operand));

    case UnaryOperator::Minus:
      if (operand.is_int())
      {
        return Value(-operand.as_int());
      }

      return Value(-to_number(operand));

    case UnaryOperator::Not:
      return Value(!is_truthy(operand));
    }

    throw RendererError("unsupported unary operator");
  }

  Value Renderer::evaluate_binary_expression(
      const BinaryExpression &expr,
      const Context &context) const
  {
    if (expr.op() == BinaryOperator::And)
    {
      const Value left = evaluate_expression(expr.left(), context);
      if (!is_truthy(left))
      {
        return Value(false);
      }

      const Value right = evaluate_expression(expr.right(), context);
      return Value(is_truthy(right));
    }

    if (expr.op() == BinaryOperator::Or)
    {
      const Value left = evaluate_expression(expr.left(), context);
      if (is_truthy(left))
      {
        return Value(true);
      }

      const Value right = evaluate_expression(expr.right(), context);
      return Value(is_truthy(right));
    }

    const Value left = evaluate_expression(expr.left(), context);
    const Value right = evaluate_expression(expr.right(), context);

    switch (expr.op())
    {
    case BinaryOperator::Add:
      if (left.is_string() || right.is_string())
      {
        return Value(left.to_string() + right.to_string());
      }

      if (are_both_integers(left, right))
      {
        return Value(left.as_int() + right.as_int());
      }

      return Value(to_number(left) + to_number(right));

    case BinaryOperator::Subtract:
      if (are_both_integers(left, right))
      {
        return Value(left.as_int() - right.as_int());
      }

      return Value(to_number(left) - to_number(right));

    case BinaryOperator::Multiply:
      if (are_both_integers(left, right))
      {
        return Value(left.as_int() * right.as_int());
      }

      return Value(to_number(left) * to_number(right));

    case BinaryOperator::Divide:
    {
      const double divisor = to_number(right);
      if (divisor == 0.0)
      {
        throw RendererError("division by zero");
      }

      return Value(to_number(left) / divisor);
    }

    case BinaryOperator::Modulo:
    {
      if (!left.is_int() || !right.is_int())
      {
        throw RendererError("modulo requires integer operands");
      }

      const auto divisor = right.as_int();
      if (divisor == 0)
      {
        throw RendererError("modulo by zero");
      }

      return Value(left.as_int() % divisor);
    }

    case BinaryOperator::Equal:
      return Value(left.to_string() == right.to_string());

    case BinaryOperator::NotEqual:
      return Value(left.to_string() != right.to_string());

    case BinaryOperator::Less:
      if (left.is_string() || right.is_string())
      {
        return Value(left.to_string() < right.to_string());
      }

      return Value(to_number(left) < to_number(right));

    case BinaryOperator::LessEqual:
      if (left.is_string() || right.is_string())
      {
        return Value(left.to_string() <= right.to_string());
      }

      return Value(to_number(left) <= to_number(right));

    case BinaryOperator::Greater:
      if (left.is_string() || right.is_string())
      {
        return Value(left.to_string() > right.to_string());
      }

      return Value(to_number(left) > to_number(right));

    case BinaryOperator::GreaterEqual:
      if (left.is_string() || right.is_string())
      {
        return Value(left.to_string() >= right.to_string());
      }

      return Value(to_number(left) >= to_number(right));

    case BinaryOperator::And:
    case BinaryOperator::Or:
      break;
    }

    throw RendererError("unsupported binary operator");
  }

  const Value *Renderer::resolve_variable(
      const std::string &name,
      const Context &context) const noexcept
  {
    return context.get(name);
  }

  const Value *Renderer::resolve_member(
      const Value &value,
      const std::string &member) const noexcept
  {
    if (!value.is_object())
    {
      return nullptr;
    }

    const auto &object = value.as_object();
    const auto it = object.find(member);

    if (it == object.end())
    {
      return nullptr;
    }

    return &it->second;
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

  bool Renderer::is_truthy(const Value &value) const noexcept
  {
    return value.truthy();
  }

  double Renderer::to_number(const Value &value) const
  {
    if (value.is_int())
    {
      return static_cast<double>(value.as_int());
    }

    if (value.is_double())
    {
      return value.as_double();
    }

    if (value.is_bool())
    {
      return value.as_bool() ? 1.0 : 0.0;
    }

    if (value.is_string())
    {
      const std::string text = value.as_string();

      if (is_integer_literal(text) || is_floating_literal(text))
      {
        return std::stod(text);
      }
    }

    if (!value.truthy())
    {
      return 0.0;
    }

    throw RendererError("value cannot be converted to number");
  }

  bool Renderer::is_in_include_stack(
      const std::string &template_name) const noexcept
  {
    return std::find(
               include_stack_.begin(),
               include_stack_.end(),
               template_name) != include_stack_.end();
  }

  const ExtendsNode *Renderer::find_extends(
      const RootNode &root) const noexcept
  {
    for (const auto &child : root.children())
    {
      if (child && child->type() == NodeType::Extends)
      {
        return static_cast<const ExtendsNode *>(child.get());
      }
    }

    return nullptr;
  }

  Renderer::BlockMap Renderer::collect_blocks(
      const RootNode &root) const
  {
    BlockMap blocks;

    for (const auto &child : root.children())
    {
      if (!child)
      {
        throw RendererError("null child node in root");
      }

      if (child->type() == NodeType::Extends)
      {
        continue;
      }

      if (child->type() != NodeType::Block)
      {
        throw RendererError(
            "child template using extends may only contain block nodes");
      }

      const auto &block = static_cast<const BlockNode &>(*child);
      blocks[block.name()] = &block;
    }

    return blocks;
  }

  void Renderer::render_template_by_name(
      const std::string &template_name,
      const Context &context,
      std::string &output) const
  {
    if (!loader_)
    {
      throw RendererError("template inheritance requires a configured loader");
    }

    if (is_in_include_stack(template_name))
    {
      throw RendererError(
          "circular template inheritance/include detected: " + template_name);
    }

    if (!loader_->exists(template_name))
    {
      throw RendererError("template not found: " + template_name);
    }

    include_stack_.push_back(template_name);

    try
    {
      const std::string source = loader_->load(template_name);

      Lexer lexer(source);
      auto tokens = lexer.tokenize();

      Parser parser(std::move(tokens));
      RootNode root = parser.parse();

      const ExtendsNode *extends_node = find_extends(root);
      if (extends_node != nullptr)
      {
        BlockMap merged = block_overrides_;
        BlockMap parent_blocks = collect_blocks(root);

        for (const auto &[name, block] : parent_blocks)
        {
          if (merged.find(name) == merged.end())
          {
            merged[name] = block;
          }
        }

        const BlockMap previous = block_overrides_;
        block_overrides_ = merged;

        render_template_by_name(
            extends_node->template_name(),
            context,
            output);

        block_overrides_ = previous;
      }
      else
      {
        render_root(root, context, output);
      }

      include_stack_.pop_back();
    }
    catch (...)
    {
      include_stack_.pop_back();
      throw;
    }
  }
} // namespace vix::template_

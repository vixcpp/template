/**
 *
 *  @file Compiler.cpp
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
#include <vix/template/Compiler.hpp>

#include <stdexcept>
#include <utility>

namespace vix::template_
{
  namespace
  {
    /**
     * @brief Convert an expression AST into a stable textual form.
     *
     * This is used in V6 as an intermediate compiled representation for
     * expression-driven instructions.
     *
     * @param expression Expression to stringify.
     * @return Canonical textual representation.
     */
    [[nodiscard]] std::string expression_to_string(const Expression &expression)
    {
      switch (expression.type())
      {
      case ExprType::Name:
      {
        const auto &node = static_cast<const NameExpression &>(expression);
        return node.name();
      }

      case ExprType::Literal:
      {
        const auto &node = static_cast<const LiteralExpression &>(expression);
        const std::string &value = node.value();

        if (value == "true" || value == "false")
        {
          return value;
        }

        auto is_integer_literal = [](const std::string &text) -> bool
        {
          if (text.empty())
          {
            return false;
          }

          std::size_t index = 0;
          if (text[index] == '+' || text[index] == '-')
          {
            ++index;
          }

          if (index >= text.size())
          {
            return false;
          }

          for (; index < text.size(); ++index)
          {
            const char c = text[index];
            if (c < '0' || c > '9')
            {
              return false;
            }
          }

          return true;
        };

        auto is_floating_literal = [](const std::string &text) -> bool
        {
          if (text.empty())
          {
            return false;
          }

          bool seen_dot = false;
          std::size_t index = 0;

          if (text[index] == '+' || text[index] == '-')
          {
            ++index;
          }

          if (index >= text.size())
          {
            return false;
          }

          for (; index < text.size(); ++index)
          {
            const char c = text[index];
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
        };

        if (is_integer_literal(value) || is_floating_literal(value))
        {
          return value;
        }

        std::string escaped;
        escaped.reserve(value.size());

        for (const char c : value)
        {
          if (c == '\\' || c == '"')
          {
            escaped.push_back('\\');
          }

          escaped.push_back(c);
        }

        return "\"" + escaped + "\"";
      }
      case ExprType::Member:
      {
        const auto &node = static_cast<const MemberExpression &>(expression);
        return expression_to_string(node.object()) + "." + node.member();
      }

      case ExprType::Unary:
      {
        const auto &node = static_cast<const UnaryExpression &>(expression);

        std::string op;
        switch (node.op())
        {
        case UnaryOperator::Plus:
          op = "+";
          break;
        case UnaryOperator::Minus:
          op = "-";
          break;
        case UnaryOperator::Not:
          op = "!";
          break;
        }

        return op + expression_to_string(node.operand());
      }

      case ExprType::Binary:
      {
        const auto &node = static_cast<const BinaryExpression &>(expression);

        std::string op;
        switch (node.op())
        {
        case BinaryOperator::Add:
          op = "+";
          break;
        case BinaryOperator::Subtract:
          op = "-";
          break;
        case BinaryOperator::Multiply:
          op = "*";
          break;
        case BinaryOperator::Divide:
          op = "/";
          break;
        case BinaryOperator::Modulo:
          op = "%";
          break;
        case BinaryOperator::Equal:
          op = "==";
          break;
        case BinaryOperator::NotEqual:
          op = "!=";
          break;
        case BinaryOperator::Less:
          op = "<";
          break;
        case BinaryOperator::LessEqual:
          op = "<=";
          break;
        case BinaryOperator::Greater:
          op = ">";
          break;
        case BinaryOperator::GreaterEqual:
          op = ">=";
          break;
        case BinaryOperator::And:
          op = "&&";
          break;
        case BinaryOperator::Or:
          op = "||";
          break;
        }

        return "(" + expression_to_string(node.left()) + " " + op + " " +
               expression_to_string(node.right()) + ")";
      }
      }

      throw std::runtime_error("unknown expression type");
    }

  } // namespace

  Template Compiler::compile(
      std::string name,
      RootNode root,
      std::shared_ptr<Loader> loader) const
  {
    Optimizer optimizer;
    RootNode optimized = optimizer.optimize(std::move(root));
    ExecutionPlan plan = build_plan(optimized);

    return Template(
        std::move(name),
        std::move(optimized),
        std::move(plan),
        std::move(loader));
  }

  ExecutionPlan Compiler::build_plan(const RootNode &root) const
  {
    ExecutionPlan plan;
    plan.reserve(root.children().size());
    compile_nodes(root.children(), plan);
    return plan;
  }

  void Compiler::compile_nodes(
      const NodeList &nodes,
      ExecutionPlan &plan) const
  {
    for (const auto &node : nodes)
    {
      if (!node)
      {
        continue;
      }

      compile_node(*node, plan);
    }
  }

  void Compiler::compile_node(
      const Node &node,
      ExecutionPlan &plan) const
  {
    switch (node.type())
    {
    case NodeType::Text:
      compile_text(static_cast<const TextNode &>(node), plan);
      return;

    case NodeType::Variable:
      compile_variable(static_cast<const VariableNode &>(node), plan);
      return;

    case NodeType::If:
      compile_if(static_cast<const IfNode &>(node), plan);
      return;

    case NodeType::For:
      compile_for(static_cast<const ForNode &>(node), plan);
      return;

    case NodeType::Include:
      compile_include(static_cast<const IncludeNode &>(node), plan);
      return;

    case NodeType::Block:
      compile_block(static_cast<const BlockNode &>(node), plan);
      return;

    case NodeType::Extends:
      // Extends is handled at a higher composition level.
      return;

    case NodeType::Root:
      compile_nodes(static_cast<const RootNode &>(node).children(), plan);
      return;
    }

    throw std::runtime_error("unsupported node type during compilation");
  }

  void Compiler::compile_text(
      const TextNode &node,
      ExecutionPlan &plan) const
  {
    if (node.value().empty())
    {
      return;
    }

    plan.emplace(OpCode::EmitText, TextInstr{node.value()});
  }

  void Compiler::compile_variable(
      const VariableNode &node,
      ExecutionPlan &plan) const
  {
    plan.emplace(
        OpCode::EmitVariable,
        VariableInstr{
            expression_to_string(node.expression()),
            node.filters()});
  }

  void Compiler::compile_if(
      const IfNode &node,
      ExecutionPlan &plan) const
  {
    const std::size_t jumpIndex = plan.size();

    plan.emplace(
        OpCode::JumpIfFalse,
        JumpIfFalseInstr{
            expression_to_string(node.condition()),
            0});

    compile_nodes(node.body(), plan);

    auto &jumpInstr = std::get<JumpIfFalseInstr>(plan.at(jumpIndex).data);
    jumpInstr.target = plan.size();
  }

  void Compiler::compile_for(
      const ForNode &node,
      ExecutionPlan &plan) const
  {
    const std::size_t beginIndex = plan.size();

    plan.emplace(
        OpCode::ForEachBegin,
        ForEachInstr{
            node.iterable_name(),
            node.item_name(),
            0});

    compile_nodes(node.body(), plan);

    plan.emplace(
        OpCode::ForEachEnd,
        JumpInstr{beginIndex});

    auto &beginInstr = std::get<ForEachInstr>(plan.at(beginIndex).data);
    beginInstr.jump_end = plan.size();
  }

  void Compiler::compile_include(
      const IncludeNode &node,
      ExecutionPlan &plan) const
  {
    plan.emplace(
        OpCode::Include,
        IncludeInstr{node.template_name()});
  }

  void Compiler::compile_block(
      const BlockNode &node,
      ExecutionPlan &plan) const
  {
    compile_nodes(node.body(), plan);
  }

} // namespace vix::template_

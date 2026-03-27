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
  Template Compiler::compile(
      std::string name,
      RootNode root,
      std::shared_ptr<Loader> loader,
      std::string source_signature) const
  {
    RootNode optimized = optimizer_.optimize(std::move(root));

    ExecutionPlan plan = build_plan(optimized);

    return Template(
        std::move(name),
        std::move(optimized),
        std::move(plan),
        std::move(loader),
        std::move(source_signature));
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
            clone_expression(node.expression()),
            node.filters()});
  }

  void Compiler::compile_if(
      const IfNode &node,
      ExecutionPlan &plan) const
  {
    const std::size_t jump_index = plan.size();

    plan.emplace(
        OpCode::JumpIfFalse,
        JumpIfFalseInstr{
            clone_expression(node.condition()),
            0});

    compile_nodes(node.body(), plan);

    auto &jump_instr = std::get<JumpIfFalseInstr>(plan.at(jump_index).data);
    jump_instr.target = plan.size();
  }

  void Compiler::compile_for(
      const ForNode &node,
      ExecutionPlan &plan) const
  {
    const std::size_t begin_index = plan.size();

    plan.emplace(
        OpCode::ForEachBegin,
        ForEachInstr{
            node.iterable_name(),
            node.item_name(),
            0});

    compile_nodes(node.body(), plan);

    plan.emplace(
        OpCode::ForEachEnd,
        JumpInstr{begin_index});

    auto &begin_instr = std::get<ForEachInstr>(plan.at(begin_index).data);
    begin_instr.jump_end = plan.size();
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

  CompiledExprPtr Compiler::clone_expression(
      const Expression &expression) const
  {
    ExprPtr cloned = clone_expression_node(expression);

    if (!cloned)
    {
      throw std::runtime_error("failed to clone expression");
    }

    return CompiledExprPtr(cloned.release());
  }

  ExprPtr Compiler::clone_expression_node(
      const Expression &expression) const
  {
    switch (expression.type())
    {
    case ExprType::Name:
    {
      const auto &node = static_cast<const NameExpression &>(expression);
      return std::make_unique<NameExpression>(node.name());
    }

    case ExprType::Literal:
    {
      const auto &node = static_cast<const LiteralExpression &>(expression);
      return std::make_unique<LiteralExpression>(node.value());
    }

    case ExprType::Member:
    {
      const auto &node = static_cast<const MemberExpression &>(expression);

      ExprPtr object = clone_expression_node(node.object());
      if (!object)
      {
        throw std::runtime_error("failed to clone member expression object");
      }

      return std::make_unique<MemberExpression>(
          std::move(object),
          node.member());
    }

    case ExprType::Unary:
    {
      const auto &node = static_cast<const UnaryExpression &>(expression);

      ExprPtr operand = clone_expression_node(node.operand());
      if (!operand)
      {
        throw std::runtime_error("failed to clone unary expression operand");
      }

      return std::make_unique<UnaryExpression>(
          node.op(),
          std::move(operand));
    }

    case ExprType::Binary:
    {
      const auto &node = static_cast<const BinaryExpression &>(expression);

      ExprPtr left = clone_expression_node(node.left());
      ExprPtr right = clone_expression_node(node.right());

      if (!left || !right)
      {
        throw std::runtime_error("failed to clone binary expression operands");
      }

      return std::make_unique<BinaryExpression>(
          std::move(left),
          node.op(),
          std::move(right));
    }
    }

    throw std::runtime_error("unknown expression type");
  }

} // namespace vix::template_

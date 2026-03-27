/**
 *
 *  @file Optimizer.cpp
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
#include <vix/template/Optimizer.hpp>

#include <utility>

namespace vix::template_
{
  RootNode Optimizer::optimize(RootNode root) const
  {
    optimize_root(root);
    return root;
  }

  NodePtr Optimizer::optimize_node(NodePtr node) const
  {
    if (!node)
    {
      return nullptr;
    }

    if (auto *ifNode = dynamic_cast<IfNode *>(node.get()))
    {
      optimize_if(*ifNode);
      return node;
    }

    if (auto *forNode = dynamic_cast<ForNode *>(node.get()))
    {
      optimize_for(*forNode);
      return node;
    }

    if (auto *blockNode = dynamic_cast<BlockNode *>(node.get()))
    {
      optimize_block(*blockNode);
      return node;
    }

    return node;
  }

  void Optimizer::optimize_root(RootNode &root) const
  {
    normalize_children(root.children());
  }

  void Optimizer::optimize_if(IfNode &node) const
  {
    normalize_children(node.body());
  }

  void Optimizer::optimize_for(ForNode &node) const
  {
    normalize_children(node.body());
  }

  void Optimizer::optimize_block(BlockNode &node) const
  {
    normalize_children(node.body());
  }

  void Optimizer::normalize_children(NodeList &children) const
  {
    NodeList optimized;
    optimized.reserve(children.size());

    for (auto &child : children)
    {
      NodePtr current = optimize_node(std::move(child));
      if (!current)
      {
        continue;
      }

      if (is_empty_text_node(current))
      {
        continue;
      }

      if (!optimized.empty() &&
          can_merge_text_nodes(optimized.back(), current))
      {
        merge_text_nodes(optimized.back(), current);
        continue;
      }

      optimized.push_back(std::move(current));
    }

    children = std::move(optimized);
  }

  bool Optimizer::is_empty_text_node(const NodePtr &node) const noexcept
  {
    if (!node)
    {
      return false;
    }

    const auto *textNode = dynamic_cast<const TextNode *>(node.get());
    if (!textNode)
    {
      return false;
    }

    return textNode->value().empty();
  }

  bool Optimizer::can_merge_text_nodes(
      const NodePtr &left,
      const NodePtr &right) const noexcept
  {
    if (!left || !right)
    {
      return false;
    }

    return dynamic_cast<const TextNode *>(left.get()) != nullptr &&
           dynamic_cast<const TextNode *>(right.get()) != nullptr;
  }

  void Optimizer::merge_text_nodes(
      const NodePtr &left,
      const NodePtr &right) const
  {
    auto *leftText = dynamic_cast<TextNode *>(left.get());
    const auto *rightText = dynamic_cast<const TextNode *>(right.get());

    if (!leftText || !rightText)
    {
      return;
    }

    leftText->set_value(leftText->value() + rightText->value());
  }

} // namespace vix::template_

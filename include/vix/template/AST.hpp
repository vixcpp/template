/**
 *
 *  @file AST.hpp
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
#ifndef VIX_TEMPLATE_AST_HPP
#define VIX_TEMPLATE_AST_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace vix::template_
{
  /**
   * @brief Base node kinds used by the template AST.
   */
  enum class NodeType
  {
    Root,
    Text,
    Variable,
    If,
    For
  };

  class Node;
  class RootNode;
  class TextNode;
  class VariableNode;
  class IfNode;
  class ForNode;

  /**
   * @brief Owning pointer to an AST node.
   */
  using NodePtr = std::unique_ptr<Node>;

  /**
   * @brief Sequence of AST nodes.
   */
  using NodeList = std::vector<NodePtr>;

  /**
   * @brief Base class for all AST nodes.
   */
  class Node
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Node() = default;

    /**
     * @brief Get the concrete node type.
     *
     * @return Node kind.
     */
    [[nodiscard]] virtual NodeType type() const noexcept = 0;
  };

  /**
   * @brief Root node of a parsed template.
   *
   * RootNode owns the full ordered sequence of child nodes
   * representing the whole template.
   */
  class RootNode final : public Node
  {
  public:
    /**
     * @brief Construct an empty root node.
     */
    RootNode() = default;

    /**
     * @brief Construct a root node from child nodes.
     *
     * @param children Child node list.
     */
    explicit RootNode(NodeList children)
        : children_(std::move(children))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Root
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Root;
    }

    /**
     * @brief Get the child nodes.
     *
     * @return Immutable child list.
     */
    [[nodiscard]] const NodeList &children() const noexcept
    {
      return children_;
    }

    /**
     * @brief Get the child nodes.
     *
     * @return Mutable child list.
     */
    [[nodiscard]] NodeList &children() noexcept
    {
      return children_;
    }

    /**
     * @brief Append a child node.
     *
     * @param node Child node to append.
     */
    void append(NodePtr node)
    {
      children_.push_back(std::move(node));
    }

  private:
    /**
     * @brief Ordered child nodes of the template.
     */
    NodeList children_;
  };

  /**
   * @brief Raw text node.
   *
   * TextNode stores literal text copied directly to the output.
   */
  class TextNode final : public Node
  {
  public:
    /**
     * @brief Construct a text node.
     *
     * @param value Raw text value.
     */
    explicit TextNode(std::string value)
        : value_(std::move(value))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Text
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Text;
    }

    /**
     * @brief Get the text value.
     *
     * @return Raw text.
     */
    [[nodiscard]] const std::string &value() const noexcept
    {
      return value_;
    }

  private:
    /**
     * @brief Raw text content.
     */
    std::string value_;
  };

  /**
   * @brief Variable interpolation node.
   *
   * Example:
   * {{ user }}
   */
  class VariableNode final : public Node
  {
  public:
    /**
     * @brief Construct a variable node.
     *
     * @param name Variable name.
     */
    explicit VariableNode(std::string name)
        : name_(std::move(name))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Variable
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Variable;
    }

    /**
     * @brief Get the variable name.
     *
     * @return Variable name.
     */
    [[nodiscard]] const std::string &name() const noexcept
    {
      return name_;
    }

  private:
    /**
     * @brief Variable name to resolve from the rendering context.
     */
    std::string name_;
  };

  /**
   * @brief Conditional node.
   *
   * Example:
   * {% if user %} ... {% endif %}
   */
  class IfNode final : public Node
  {
  public:
    /**
     * @brief Construct a conditional node.
     *
     * @param condition Variable or expression name used as condition.
     * @param body Child nodes rendered when the condition is truthy.
     */
    IfNode(std::string condition, NodeList body)
        : condition_(std::move(condition)),
          body_(std::move(body))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::If
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::If;
    }

    /**
     * @brief Get the condition identifier.
     *
     * @return Condition name.
     */
    [[nodiscard]] const std::string &condition() const noexcept
    {
      return condition_;
    }

    /**
     * @brief Get the conditional body.
     *
     * @return Immutable child node list.
     */
    [[nodiscard]] const NodeList &body() const noexcept
    {
      return body_;
    }

    /**
     * @brief Get the conditional body.
     *
     * @return Mutable child node list.
     */
    [[nodiscard]] NodeList &body() noexcept
    {
      return body_;
    }

  private:
    /**
     * @brief Condition identifier.
     */
    std::string condition_;

    /**
     * @brief Nodes rendered when the condition is truthy.
     */
    NodeList body_;
  };

  /**
   * @brief Loop node.
   *
   * Example:
   * {% for item in items %} ... {% endfor %}
   */
  class ForNode final : public Node
  {
  public:
    /**
     * @brief Construct a loop node.
     *
     * @param item_name Loop variable name.
     * @param iterable_name Iterable variable name.
     * @param body Child nodes rendered for each item.
     */
    ForNode(
        std::string item_name,
        std::string iterable_name,
        NodeList body)
        : item_name_(std::move(item_name)),
          iterable_name_(std::move(iterable_name)),
          body_(std::move(body))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::For
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::For;
    }

    /**
     * @brief Get the loop item variable name.
     *
     * @return Item variable name.
     */
    [[nodiscard]] const std::string &item_name() const noexcept
    {
      return item_name_;
    }

    /**
     * @brief Get the iterable variable name.
     *
     * @return Iterable name.
     */
    [[nodiscard]] const std::string &iterable_name() const noexcept
    {
      return iterable_name_;
    }

    /**
     * @brief Get the loop body.
     *
     * @return Immutable child node list.
     */
    [[nodiscard]] const NodeList &body() const noexcept
    {
      return body_;
    }

    /**
     * @brief Get the loop body.
     *
     * @return Mutable child node list.
     */
    [[nodiscard]] NodeList &body() noexcept
    {
      return body_;
    }

  private:
    /**
     * @brief Variable name bound for each loop item.
     */
    std::string item_name_;

    /**
     * @brief Iterable variable name.
     */
    std::string iterable_name_;

    /**
     * @brief Nodes rendered for each iteration.
     */
    NodeList body_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_AST_HPP

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
    For,
    Include,
    Extends,
    Block
  };

  class Node;
  class RootNode;
  class TextNode;
  class VariableNode;
  class IfNode;
  class ForNode;
  class IncludeNode;
  class ExtendsNode;
  class BlockNode;

  /**
   * @brief Owning pointer to an AST node.
   */
  using NodePtr = std::unique_ptr<Node>;

  /**
   * @brief Sequence of AST nodes.
   */
  using NodeList = std::vector<NodePtr>;

  /**
   * @brief Filter node attached to a variable interpolation.
   *
   * Example:
   * {{ name | upper | length }}
   *
   * In this example:
   * - upper is one FilterNode
   * - length is another FilterNode
   *
   * V2 keeps filters simple:
   * - filter name only
   * - no arguments yet
   */
  class FilterNode
  {
  public:
    /**
     * @brief Construct a filter node.
     *
     * @param name Filter name.
     */
    explicit FilterNode(std::string name)
        : name_(std::move(name))
    {
    }

    /**
     * @brief Get the filter name.
     *
     * @return Filter name.
     */
    [[nodiscard]] const std::string &name() const noexcept
    {
      return name_;
    }

  private:
    /**
     * @brief Filter name.
     */
    std::string name_;
  };

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
   * Examples:
   * {{ user }}
   * {{ user | upper }}
   * {{ items | length }}
   */
  class VariableNode final : public Node
  {
  public:
    /**
     * @brief Construct a variable node without filters.
     *
     * @param name Variable name.
     */
    explicit VariableNode(std::string name)
        : name_(std::move(name))
    {
    }

    /**
     * @brief Construct a variable node with filters.
     *
     * @param name Variable name.
     * @param filters Filter pipeline.
     */
    VariableNode(std::string name, std::vector<FilterNode> filters)
        : name_(std::move(name)),
          filters_(std::move(filters))
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

    /**
     * @brief Get the filter pipeline.
     *
     * @return Ordered list of filters to apply.
     */
    [[nodiscard]] const std::vector<FilterNode> &filters() const noexcept
    {
      return filters_;
    }

    /**
     * @brief Check whether the variable has filters.
     *
     * @return True if one or more filters are attached.
     */
    [[nodiscard]] bool has_filters() const noexcept
    {
      return !filters_.empty();
    }

  private:
    /**
     * @brief Variable name to resolve from the rendering context.
     */
    std::string name_;

    /**
     * @brief Ordered filter pipeline.
     */
    std::vector<FilterNode> filters_;
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

  /**
   * @brief Include node.
   *
   * Example:
   * {% include "header.html" %}
   *
   * V3 keeps includes simple:
   * - static string target only
   * - current context is reused
   * - no custom include arguments yet
   */
  class IncludeNode final : public Node
  {
  public:
    /**
     * @brief Construct an include node.
     *
     * @param template_name Included template name.
     */
    explicit IncludeNode(std::string template_name)
        : template_name_(std::move(template_name))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Include
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Include;
    }

    /**
     * @brief Get the included template name.
     *
     * @return Template name.
     */
    [[nodiscard]] const std::string &template_name() const noexcept
    {
      return template_name_;
    }

  private:
    /**
     * @brief Logical name of the included template.
     */
    std::string template_name_;
  };

  /**
   * @brief Extends node.
   *
   * Example:
   * {% extends "base.html" %}
   */
  class ExtendsNode final : public Node
  {
  public:
    explicit ExtendsNode(std::string template_name)
        : template_name_(std::move(template_name))
    {
    }

    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Extends;
    }

    [[nodiscard]] const std::string &template_name() const noexcept
    {
      return template_name_;
    }

  private:
    std::string template_name_;
  };

  /**
   * @brief Block node.
   *
   * Example:
   * {% block content %} ... {% endblock %}
   */
  class BlockNode final : public Node
  {
  public:
    BlockNode(std::string name, NodeList body)
        : name_(std::move(name)),
          body_(std::move(body))
    {
    }

    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Block;
    }

    [[nodiscard]] const std::string &name() const noexcept
    {
      return name_;
    }

    [[nodiscard]] const NodeList &body() const noexcept
    {
      return body_;
    }

    [[nodiscard]] NodeList &body() noexcept
    {
      return body_;
    }

  private:
    std::string name_;
    NodeList body_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_AST_HPP

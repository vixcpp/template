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

  /**
   * @brief Base expression kinds used by the template AST.
   */
  enum class ExprType
  {
    Name,
    Literal,
    Member,
    Unary,
    Binary
  };

  /**
   * @brief Supported unary operators for expressions.
   */
  enum class UnaryOperator
  {
    Plus,
    Minus,
    Not
  };

  /**
   * @brief Supported binary operators for expressions.
   */
  enum class BinaryOperator
  {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or
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

  class Expression;
  class NameExpression;
  class LiteralExpression;
  class MemberExpression;
  class UnaryExpression;
  class BinaryExpression;

  /**
   * @brief Owning pointer to an AST node.
   */
  using NodePtr = std::unique_ptr<Node>;

  /**
   * @brief Sequence of AST nodes.
   */
  using NodeList = std::vector<NodePtr>;

  /**
   * @brief Owning pointer to an expression node.
   */
  using ExprPtr = std::unique_ptr<Expression>;

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
   * @brief Base class for all expression nodes.
   */
  class Expression
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Expression() = default;

    /**
     * @brief Get the concrete expression type.
     *
     * @return Expression kind.
     */
    [[nodiscard]] virtual ExprType type() const noexcept = 0;
  };

  /**
   * @brief Expression that references a name from the rendering context.
   *
   * Examples:
   * - user
   * - price
   * - quantity
   */
  class NameExpression final : public Expression
  {
  public:
    /**
     * @brief Construct a name expression.
     *
     * @param name Referenced name.
     */
    explicit NameExpression(std::string name)
        : name_(std::move(name))
    {
    }

    /**
     * @brief Get the expression type.
     *
     * @return ExprType::Name
     */
    [[nodiscard]] ExprType type() const noexcept override
    {
      return ExprType::Name;
    }

    /**
     * @brief Get the referenced name.
     *
     * @return Name.
     */
    [[nodiscard]] const std::string &name() const noexcept
    {
      return name_;
    }

  private:
    /**
     * @brief Referenced identifier.
     */
    std::string name_;
  };

  /**
   * @brief Literal expression.
   *
   * V5 keeps literals simple and stores their raw textual form.
   * Parsing and conversion to runtime values will be handled by
   * the evaluation layer.
   *
   * Examples:
   * - 42
   * - 3.14
   * - true
   * - false
   * - "hello"
   */
  class LiteralExpression final : public Expression
  {
  public:
    /**
     * @brief Construct a literal expression.
     *
     * @param value Raw literal text.
     */
    explicit LiteralExpression(std::string value)
        : value_(std::move(value))
    {
    }

    /**
     * @brief Get the expression type.
     *
     * @return ExprType::Literal
     */
    [[nodiscard]] ExprType type() const noexcept override
    {
      return ExprType::Literal;
    }

    /**
     * @brief Get the raw literal value.
     *
     * @return Raw literal text.
     */
    [[nodiscard]] const std::string &value() const noexcept
    {
      return value_;
    }

  private:
    /**
     * @brief Raw literal text.
     */
    std::string value_;
  };

  /**
   * @brief Member access expression.
   *
   * Example:
   * - user.name
   * - order.total
   */
  class MemberExpression final : public Expression
  {
  public:
    /**
     * @brief Construct a member access expression.
     *
     * @param object Object expression.
     * @param member Member name.
     */
    MemberExpression(ExprPtr object, std::string member)
        : object_(std::move(object)),
          member_(std::move(member))
    {
    }

    /**
     * @brief Get the expression type.
     *
     * @return ExprType::Member
     */
    [[nodiscard]] ExprType type() const noexcept override
    {
      return ExprType::Member;
    }

    /**
     * @brief Get the object expression.
     *
     * @return Object expression.
     */
    [[nodiscard]] const Expression &object() const noexcept
    {
      return *object_;
    }

    /**
     * @brief Get the object expression pointer.
     *
     * @return Object expression pointer.
     */
    [[nodiscard]] const ExprPtr &object_ptr() const noexcept
    {
      return object_;
    }

    /**
     * @brief Get the member name.
     *
     * @return Member name.
     */
    [[nodiscard]] const std::string &member() const noexcept
    {
      return member_;
    }

  private:
    /**
     * @brief Object being accessed.
     */
    ExprPtr object_;

    /**
     * @brief Member name.
     */
    std::string member_;
  };

  /**
   * @brief Unary expression.
   *
   * Examples:
   * - -price
   * - !enabled
   */
  class UnaryExpression final : public Expression
  {
  public:
    /**
     * @brief Construct a unary expression.
     *
     * @param op Unary operator.
     * @param operand Operand expression.
     */
    UnaryExpression(UnaryOperator op, ExprPtr operand)
        : op_(op),
          operand_(std::move(operand))
    {
    }

    /**
     * @brief Get the expression type.
     *
     * @return ExprType::Unary
     */
    [[nodiscard]] ExprType type() const noexcept override
    {
      return ExprType::Unary;
    }

    /**
     * @brief Get the unary operator.
     *
     * @return Unary operator.
     */
    [[nodiscard]] UnaryOperator op() const noexcept
    {
      return op_;
    }

    /**
     * @brief Get the operand expression.
     *
     * @return Operand expression.
     */
    [[nodiscard]] const Expression &operand() const noexcept
    {
      return *operand_;
    }

    /**
     * @brief Get the operand expression pointer.
     *
     * @return Operand expression pointer.
     */
    [[nodiscard]] const ExprPtr &operand_ptr() const noexcept
    {
      return operand_;
    }

  private:
    /**
     * @brief Unary operator.
     */
    UnaryOperator op_;

    /**
     * @brief Operand expression.
     */
    ExprPtr operand_;
  };

  /**
   * @brief Binary expression.
   *
   * Examples:
   * - price * quantity
   * - a == b
   * - total >= limit
   */
  class BinaryExpression final : public Expression
  {
  public:
    /**
     * @brief Construct a binary expression.
     *
     * @param left Left operand.
     * @param op Binary operator.
     * @param right Right operand.
     */
    BinaryExpression(ExprPtr left, BinaryOperator op, ExprPtr right)
        : left_(std::move(left)),
          op_(op),
          right_(std::move(right))
    {
    }

    /**
     * @brief Get the expression type.
     *
     * @return ExprType::Binary
     */
    [[nodiscard]] ExprType type() const noexcept override
    {
      return ExprType::Binary;
    }

    /**
     * @brief Get the left operand.
     *
     * @return Left operand.
     */
    [[nodiscard]] const Expression &left() const noexcept
    {
      return *left_;
    }

    /**
     * @brief Get the left operand pointer.
     *
     * @return Left operand pointer.
     */
    [[nodiscard]] const ExprPtr &left_ptr() const noexcept
    {
      return left_;
    }

    /**
     * @brief Get the binary operator.
     *
     * @return Binary operator.
     */
    [[nodiscard]] BinaryOperator op() const noexcept
    {
      return op_;
    }

    /**
     * @brief Get the right operand.
     *
     * @return Right operand.
     */
    [[nodiscard]] const Expression &right() const noexcept
    {
      return *right_;
    }

    /**
     * @brief Get the right operand pointer.
     *
     * @return Right operand pointer.
     */
    [[nodiscard]] const ExprPtr &right_ptr() const noexcept
    {
      return right_;
    }

  private:
    /**
     * @brief Left operand.
     */
    ExprPtr left_;

    /**
     * @brief Binary operator.
     */
    BinaryOperator op_;

    /**
     * @brief Right operand.
     */
    ExprPtr right_;
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
   * {{ user.name }}
   * {{ user.name | upper }}
   * {{ price * quantity }}
   * {{ a == b }}
   */
  class VariableNode final : public Node
  {
  public:
    /**
     * @brief Construct a variable node without filters.
     *
     * @param expression Interpolation expression.
     */
    explicit VariableNode(ExprPtr expression)
        : expression_(std::move(expression))
    {
    }

    /**
     * @brief Construct a variable node with filters.
     *
     * @param expression Interpolation expression.
     * @param filters Filter pipeline.
     */
    VariableNode(ExprPtr expression, std::vector<FilterNode> filters)
        : expression_(std::move(expression)),
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
     * @brief Get the interpolation expression.
     *
     * @return Expression.
     */
    [[nodiscard]] const Expression &expression() const noexcept
    {
      return *expression_;
    }

    /**
     * @brief Get the interpolation expression pointer.
     *
     * @return Expression pointer.
     */
    [[nodiscard]] const ExprPtr &expression_ptr() const noexcept
    {
      return expression_;
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
     * @brief Expression to evaluate during rendering.
     */
    ExprPtr expression_;

    /**
     * @brief Ordered filter pipeline.
     */
    std::vector<FilterNode> filters_;
  };

  /**
   * @brief Conditional node.
   *
   * Example:
   * {% if user %}
   * {% if price * quantity > 0 %}
   */
  class IfNode final : public Node
  {
  public:
    /**
     * @brief Construct a conditional node.
     *
     * @param condition Condition expression.
     * @param body Child nodes rendered when the condition is truthy.
     */
    IfNode(ExprPtr condition, NodeList body)
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
     * @brief Get the condition expression.
     *
     * @return Condition expression.
     */
    [[nodiscard]] const Expression &condition() const noexcept
    {
      return *condition_;
    }

    /**
     * @brief Get the condition expression pointer.
     *
     * @return Condition expression pointer.
     */
    [[nodiscard]] const ExprPtr &condition_ptr() const noexcept
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
     * @brief Condition expression.
     */
    ExprPtr condition_;

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
   *
   * V5 keeps loop iterables simple and still stores the iterable as a name.
   * Expression support is introduced first for interpolation and conditions.
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
    /**
     * @brief Construct an extends node.
     *
     * @param template_name Parent template name.
     */
    explicit ExtendsNode(std::string template_name)
        : template_name_(std::move(template_name))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Extends
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Extends;
    }

    /**
     * @brief Get the parent template name.
     *
     * @return Parent template name.
     */
    [[nodiscard]] const std::string &template_name() const noexcept
    {
      return template_name_;
    }

  private:
    /**
     * @brief Parent template logical name.
     */
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
    /**
     * @brief Construct a block node.
     *
     * @param name Block name.
     * @param body Block body.
     */
    BlockNode(std::string name, NodeList body)
        : name_(std::move(name)),
          body_(std::move(body))
    {
    }

    /**
     * @brief Get the node type.
     *
     * @return NodeType::Block
     */
    [[nodiscard]] NodeType type() const noexcept override
    {
      return NodeType::Block;
    }

    /**
     * @brief Get the block name.
     *
     * @return Block name.
     */
    [[nodiscard]] const std::string &name() const noexcept
    {
      return name_;
    }

    /**
     * @brief Get the block body.
     *
     * @return Immutable body.
     */
    [[nodiscard]] const NodeList &body() const noexcept
    {
      return body_;
    }

    /**
     * @brief Get the block body.
     *
     * @return Mutable body.
     */
    [[nodiscard]] NodeList &body() noexcept
    {
      return body_;
    }

  private:
    /**
     * @brief Block name.
     */
    std::string name_;

    /**
     * @brief Block child nodes.
     */
    NodeList body_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_AST_HPP

/**
 *
 *  @file Optimizer.hpp
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
#ifndef VIX_TEMPLATE_OPTIMIZER_HPP
#define VIX_TEMPLATE_OPTIMIZER_HPP

#include <vix/template/AST.hpp>

namespace vix::template_
{
  /**
   * @brief AST optimizer used before compilation.
   *
   * Optimizer is responsible for transforming a parsed AST into a more
   * compact and execution-friendly form before the Compiler generates the
   * final ExecutionPlan.
   *
   * In V6, the optimizer should remain conservative and safe.
   * Its primary responsibilities are:
   * - normalize node sequences
   * - merge adjacent text nodes
   * - remove empty text nodes
   * - prepare the tree for faster compilation
   *
   * It must not change the semantic meaning of the template.
   *
   * This class is intentionally kept separate from Compiler so that:
   * - optimization logic stays isolated
   * - new optimization passes can be added progressively
   * - the compilation pipeline remains easier to test
   */
  class Optimizer
  {
  public:
    /**
     * @brief Construct an optimizer.
     */
    Optimizer() = default;

    /**
     * @brief Optimize a root AST node.
     *
     * This method applies all currently enabled optimization passes and
     * returns a new optimized root node.
     *
     * @param root Parsed AST root.
     * @return Optimized AST root.
     */
    [[nodiscard]] RootNode optimize(RootNode root) const;

  private:
    /**
     * @brief Optimize a generic node.
     *
     * This method dispatches optimization according to the concrete node
     * type and returns the optimized result.
     *
     * @param node Input node.
     * @return Optimized node.
     */
    [[nodiscard]] NodePtr optimize_node(NodePtr node) const;

    /**
     * @brief Optimize the children of a root node.
     *
     * @param root Root node to normalize.
     */
    void optimize_root(RootNode &root) const;

    /**
     * @brief Optimize the body of an if node.
     *
     * @param node If node to normalize.
     */
    void optimize_if(IfNode &node) const;

    /**
     * @brief Optimize the body of a for node.
     *
     * @param node For node to normalize.
     */
    void optimize_for(ForNode &node) const;

    /**
     * @brief Optimize the children of a block node.
     *
     * @param node Block node to normalize.
     */
    void optimize_block(BlockNode &node) const;

    /**
     * @brief Normalize a list of child nodes.
     *
     * This pass is responsible for:
     * - recursively optimizing children
     * - removing null nodes
     * - removing empty text nodes
     * - merging adjacent text nodes
     *
     * @param children Mutable list of child nodes.
     */
    void normalize_children(NodeList &children) const;

    /**
     * @brief Check whether a node is an empty text node.
     *
     * @param node Node to inspect.
     * @return True if the node is a text node with empty content.
     */
    [[nodiscard]] bool is_empty_text_node(const NodePtr &node) const noexcept;

    /**
     * @brief Check whether two nodes are mergeable text nodes.
     *
     * @param left First node.
     * @param right Second node.
     * @return True if both nodes are text nodes and can be merged.
     */
    [[nodiscard]] bool can_merge_text_nodes(
        const NodePtr &left,
        const NodePtr &right) const noexcept;

    /**
     * @brief Merge the right text node into the left text node.
     *
     * This helper assumes both nodes are valid text nodes and mergeable.
     *
     * @param left Destination text node.
     * @param right Source text node.
     */
    void merge_text_nodes(
        const NodePtr &left,
        const NodePtr &right) const;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_OPTIMIZER_HPP

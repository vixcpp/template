/**
 *
 *  @file ExecutionPlan.hpp
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
#ifndef VIX_TEMPLATE_EXECUTION_PLAN_HPP
#define VIX_TEMPLATE_EXECUTION_PLAN_HPP

#include <cstddef>
#include <utility>
#include <vector>
#include <string>
#include <type_traits>
#include <variant>

#include <vix/template/Instruction.hpp>

namespace vix::template_
{
  class Renderer;
  class Context;

  /**
   * @brief Compiled linear execution plan for a template.
   *
   * ExecutionPlan stores a flat sequence of instructions produced by the
   * Compiler after optional AST optimization and normalization.
   *
   * This representation is intended to be:
   * - easier to execute than a recursive AST
   * - more cache-friendly during rendering
   * - stable enough to support future compiler optimizations
   *
   * The Renderer consumes this plan sequentially and applies jumps when
   * control-flow instructions require it.
   *
   * In V7, ExecutionPlan is also the primary compiled artifact stored in
   * the template cache through the owning Template object.
   */
  class ExecutionPlan
  {
  public:
    /**
     * @brief Construct an empty execution plan.
     */
    ExecutionPlan() = default;

    /**
     * @brief Construct a plan from an existing instruction list.
     *
     * @param instructions Instruction sequence to own.
     */
    explicit ExecutionPlan(std::vector<Instruction> instructions)
        : instructions_(std::move(instructions))
    {
    }

    /**
     * @brief Append an instruction to the plan.
     *
     * @param instruction Instruction to append.
     */
    void add(Instruction instruction)
    {
      instructions_.push_back(std::move(instruction));
    }

    /**
     * @brief Construct and append an instruction in place.
     *
     * @tparam Args Constructor argument types.
     * @param args Arguments forwarded to Instruction constructor.
     * @return Reference to the appended instruction.
     */
    template <typename... Args>
    Instruction &emplace(Args &&...args)
    {
      instructions_.emplace_back(std::forward<Args>(args)...);
      return instructions_.back();
    }

    /**
     * @brief Access all instructions.
     *
     * @return Immutable instruction list.
     */
    [[nodiscard]] const std::vector<Instruction> &instructions() const noexcept
    {
      return instructions_;
    }

    /**
     * @brief Access all instructions.
     *
     * @return Mutable instruction list.
     */
    [[nodiscard]] std::vector<Instruction> &instructions() noexcept
    {
      return instructions_;
    }

    /**
     * @brief Access an instruction by index.
     *
     * @param index Instruction index.
     * @return Immutable instruction reference.
     */
    [[nodiscard]] const Instruction &at(std::size_t index) const
    {
      return instructions_.at(index);
    }

    /**
     * @brief Access an instruction by index.
     *
     * @param index Instruction index.
     * @return Mutable instruction reference.
     */
    [[nodiscard]] Instruction &at(std::size_t index)
    {
      return instructions_.at(index);
    }

    /**
     * @brief Access an instruction without bounds checking.
     *
     * @param index Instruction index.
     * @return Immutable instruction reference.
     */
    [[nodiscard]] const Instruction &operator[](std::size_t index) const noexcept
    {
      return instructions_[index];
    }

    /**
     * @brief Access an instruction without bounds checking.
     *
     * @param index Instruction index.
     * @return Mutable instruction reference.
     */
    [[nodiscard]] Instruction &operator[](std::size_t index) noexcept
    {
      return instructions_[index];
    }

    /**
     * @brief Get the first instruction.
     *
     * @return Immutable first instruction.
     */
    [[nodiscard]] const Instruction &front() const
    {
      return instructions_.front();
    }

    /**
     * @brief Get the first instruction.
     *
     * @return Mutable first instruction.
     */
    [[nodiscard]] Instruction &front()
    {
      return instructions_.front();
    }

    /**
     * @brief Get the last instruction.
     *
     * @return Immutable last instruction.
     */
    [[nodiscard]] const Instruction &back() const
    {
      return instructions_.back();
    }

    /**
     * @brief Get the last instruction.
     *
     * @return Mutable last instruction.
     */
    [[nodiscard]] Instruction &back()
    {
      return instructions_.back();
    }

    /**
     * @brief Check whether the plan is empty.
     *
     * @return True if there are no instructions.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return instructions_.empty();
    }

    /**
     * @brief Get the number of instructions.
     *
     * @return Instruction count.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return instructions_.size();
    }

    /**
     * @brief Get the current reserved capacity.
     *
     * @return Underlying vector capacity.
     */
    [[nodiscard]] std::size_t capacity() const noexcept
    {
      return instructions_.capacity();
    }

    /**
     * @brief Reserve storage for instructions.
     *
     * @param count Desired capacity.
     */
    void reserve(std::size_t count)
    {
      instructions_.reserve(count);
    }

    /**
     * @brief Shrink capacity to fit the current size.
     *
     * Useful after compilation when the plan becomes immutable in practice.
     */
    void shrink_to_fit()
    {
      instructions_.shrink_to_fit();
    }

    /**
     * @brief Remove all instructions from the plan.
     */
    void clear() noexcept
    {
      instructions_.clear();
    }

    /**
     * @brief Execute the plan in streaming mode.
     *
     * This method renders the compiled execution plan incrementally and writes
     * output to the provided sink.
     *
     * The renderer is used as the execution backend so that streaming rendering
     * stays fully consistent with classic buffered rendering, including:
     * - expression evaluation
     * - filter application
     * - HTML escaping
     * - includes and control flow
     *
     * @tparam Output Output sink type. Must provide a `write(const std::string&)`
     * method.
     *
     * @param renderer Renderer used to execute the plan.
     * @param context Runtime rendering context.
     * @param out Output sink receiving rendered chunks incrementally.
     */
    template <typename Output>
    void execute_stream(
        const Renderer &renderer,
        const Context &context,
        Output &out) const
    {
      std::string buffer;
      renderer.execute_plan(*this, context, buffer);
      out.write(buffer);
    }

  private:
    /**
     * @brief Linear instruction storage.
     */
    std::vector<Instruction> instructions_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_EXECUTION_PLAN_HPP

/**
 *
 *  @file Instruction.hpp
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
#ifndef VIX_TEMPLATE_INSTRUCTION_HPP
#define VIX_TEMPLATE_INSTRUCTION_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <vix/template/AST.hpp>

namespace vix::template_
{
  /**
   * @brief Shared compiled expression pointer used by the execution plan.
   *
   * The execution plan must keep expressions in compiled form so the renderer
   * can evaluate them directly at runtime without reparsing expression text.
   *
   * Shared ownership is used here to keep Instruction copyable and movable
   * while still storing precompiled expression trees.
   */
  using CompiledExprPtr = std::shared_ptr<const Expression>;

  /**
   * @brief Enumeration of all instruction types used by the execution plan.
   *
   * Instructions represent a low-level, linear form of the template,
   * produced by the Compiler and executed by the Renderer.
   *
   * This abstraction replaces recursive AST traversal with a flat,
   * cache-friendly execution model.
   */
  enum class OpCode : std::uint8_t
  {
    /**
     * @brief Emit raw text directly to output.
     */
    EmitText,

    /**
     * @brief Emit a variable from the context.
     */
    EmitVariable,

    /**
     * @brief Conditional jump if expression is false.
     *
     * Used for implementing `if` statements.
     */
    JumpIfFalse,

    /**
     * @brief Unconditional jump.
     *
     * Used for control flow (if/else, loops).
     */
    Jump,

    /**
     * @brief Begin a loop (for).
     */
    ForEachBegin,

    /**
     * @brief End a loop (for).
     */
    ForEachEnd,

    /**
     * @brief Include another template.
     */
    Include
  };

  /**
   * @brief Payload for EmitText instruction.
   */
  struct TextInstr
  {
    /**
     * @brief Literal text emitted directly to output.
     */
    std::string text;
  };

  /**
   * @brief Payload for EmitVariable instruction.
   *
   * Stores the already compiled expression to evaluate at runtime.
   * This avoids reparsing the variable expression during rendering.
   */
  struct VariableInstr
  {
    /**
     * @brief Precompiled expression to evaluate.
     */
    CompiledExprPtr expression;

    /**
     * @brief Filter pipeline applied after expression evaluation.
     */
    std::vector<FilterNode> filters;
  };

  /**
   * @brief Payload for jump instructions.
   *
   * Target represents the index to jump to in the instruction list.
   */
  struct JumpInstr
  {
    /**
     * @brief Destination instruction index.
     */
    std::size_t target{0};
  };

  /**
   * @brief Payload for loop instructions.
   *
   * Represents iteration over a collection variable.
   */
  struct ForEachInstr
  {
    /**
     * @brief Iterable variable name resolved at runtime.
     */
    std::string iterable_name;

    /**
     * @brief Loop item variable name bound for each iteration.
     */
    std::string item_name;

    /**
     * @brief Instruction index to jump to when the loop ends.
     */
    std::size_t jump_end{0};
  };

  /**
   * @brief Payload for conditional jump instructions.
   *
   * The condition is stored in compiled form and evaluated directly
   * at runtime. If the result is falsy, execution jumps to target.
   */
  struct JumpIfFalseInstr
  {
    /**
     * @brief Precompiled condition expression.
     */
    CompiledExprPtr expression;

    /**
     * @brief Destination instruction index when condition is false.
     */
    std::size_t target{0};
  };

  /**
   * @brief Payload for include instruction.
   */
  struct IncludeInstr
  {
    /**
     * @brief Logical template name to include.
     */
    std::string template_name;
  };

  /**
   * @brief Variant holding instruction-specific data.
   */
  using InstructionData = std::variant<
      std::monostate,
      TextInstr,
      VariableInstr,
      JumpInstr,
      JumpIfFalseInstr,
      ForEachInstr,
      IncludeInstr>;

  /**
   * @brief Single executable instruction.
   *
   * Each instruction contains:
   * - an opcode describing the operation
   * - optional associated data
   *
   * This structure is designed to be lightweight and cache-friendly.
   */
  struct Instruction
  {
    /**
     * @brief Operation type.
     */
    OpCode op{OpCode::EmitText};

    /**
     * @brief Associated instruction data.
     */
    InstructionData data{};

    /**
     * @brief Default constructor.
     */
    Instruction() = default;

    /**
     * @brief Construct an instruction with opcode only.
     *
     * @param opcode Instruction type.
     */
    explicit Instruction(OpCode opcode)
        : op(opcode)
    {
    }

    /**
     * @brief Construct an instruction with opcode and data.
     *
     * @param opcode Instruction type.
     * @param payload Associated data.
     */
    Instruction(OpCode opcode, InstructionData payload)
        : op(opcode),
          data(std::move(payload))
    {
    }
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_INSTRUCTION_HPP

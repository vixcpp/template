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

#include <string>
#include <variant>
#include <cstdint>
#include <vector>
#include <vix/template/AST.hpp>

namespace vix::template_
{
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
    std::string text;
  };

  /**
   * @brief Payload for EmitVariable instruction.
   *
   * Stores the variable name to resolve at runtime.
   */
  struct VariableInstr
  {
    std::string expression;
    std::vector<FilterNode> filters;
  };

  /**
   * @brief Payload for jump instructions.
   *
   * Offset represents the index to jump to in the instruction list.
   */
  struct JumpInstr
  {
    std::size_t target{0};
  };

  /**
   * @brief Payload for loop instructions.
   *
   * Represents iteration over a collection variable.
   */
  struct ForEachInstr
  {
    std::string iterable_name;
    std::string item_name;
    std::size_t jump_end{0};
  };

  /**
   * @brief Payload for conditional jump instructions.
   *
   * The expression is evaluated at runtime.
   * If the result is falsy, execution jumps to target.
   */
  struct JumpIfFalseInstr
  {
    std::string expression;
    std::size_t target{0};
  };

  /**
   * @brief Payload for include instruction.
   */
  struct IncludeInstr
  {
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

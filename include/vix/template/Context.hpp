/**
 *
 *  @file Context.hpp
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
#ifndef VIX_TEMPLATE_CONTEXT_HPP
#define VIX_TEMPLATE_CONTEXT_HPP

#include <optional>
#include <string>

#include <vix/template/Value.hpp>

namespace vix::template_
{
  /**
   * @brief Runtime context used during template rendering.
   *
   * Context stores the named values accessible from templates.
   * It is the main data container passed to the renderer and engine.
   */
  class Context
  {
  public:
    /**
     * @brief Construct an empty context.
     */
    Context() = default;

    /**
     * @brief Construct a context from an object.
     *
     * @param values Initial named values.
     */
    explicit Context(Object values);

    /**
     * @brief Insert or replace a named value.
     *
     * @param key Variable name.
     * @param value Variable value.
     */
    void set(std::string key, Value value);

    /**
     * @brief Check whether a key exists in the context.
     *
     * @param key Variable name.
     * @return True if present.
     */
    [[nodiscard]] bool has(const std::string &key) const;

    /**
     * @brief Retrieve a value by key.
     *
     * @param key Variable name.
     * @return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] const Value *get(const std::string &key) const noexcept;

    /**
     * @brief Retrieve a mutable value by key.
     *
     * @param key Variable name.
     * @return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] Value *get(const std::string &key) noexcept;

    /**
     * @brief Get a value by key or return a fallback.
     *
     * @param key Variable name.
     * @param fallback Fallback value if the key does not exist.
     * @return Found value or fallback.
     */
    [[nodiscard]] Value get_or(
        const std::string &key,
        Value fallback) const;

    /**
     * @brief Remove a value from the context.
     *
     * @param key Variable name.
     * @return True if a value was removed.
     */
    bool erase(const std::string &key);

    /**
     * @brief Remove all values from the context.
     */
    void clear() noexcept;

    /**
     * @brief Check whether the context is empty.
     *
     * @return True if empty.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Get the number of stored values.
     *
     * @return Number of values.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Access the underlying object storage.
     *
     * @return Immutable object view.
     */
    [[nodiscard]] const Object &values() const noexcept;

    /**
     * @brief Access the underlying object storage.
     *
     * @return Mutable object view.
     */
    [[nodiscard]] Object &values() noexcept;

  private:
    /**
     * @brief Stored named values.
     */
    Object values_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_CONTEXT_HPP

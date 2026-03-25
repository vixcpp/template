/**
 *
 *  @file Value.hpp
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
#ifndef VIX_TEMPLATE_VALUE_HPP
#define VIX_TEMPLATE_VALUE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace vix::template_
{
  class Value;

  /**
   * @brief Ordered list of template values.
   */
  using Array = std::vector<Value>;

  /**
   * @brief Named object of template values.
   *
   * std::map is used here for deterministic iteration order.
   */
  using Object = std::map<std::string, Value>;

  /**
   * @brief Dynamic value container used by the template engine.
   *
   * Value represents the runtime data passed to templates:
   * - null
   * - boolean
   * - integer
   * - floating-point
   * - string
   * - array
   * - object
   *
   * It is the fundamental data type used by Context, Renderer and filters.
   */
  class Value
  {
  public:
    /**
     * @brief Underlying variant storage type.
     */
    using Storage = std::variant<
        std::nullptr_t,
        bool,
        std::int64_t,
        double,
        std::string,
        Array,
        Object>;

    /**
     * @brief Construct a null value.
     */
    Value();

    /**
     * @brief Construct from null.
     *
     * @param value Null literal.
     */
    Value(std::nullptr_t value);

    /**
     * @brief Construct from boolean.
     *
     * @param value Boolean value.
     */
    Value(bool value);

    /**
     * @brief Construct from signed integer.
     *
     * @param value Integer value.
     */
    Value(int value);

    /**
     * @brief Construct from signed 64-bit integer.
     *
     * @param value Integer value.
     */
    Value(std::int64_t value);

    /**
     * @brief Construct from floating-point value.
     *
     * @param value Floating-point value.
     */
    Value(double value);

    /**
     * @brief Construct from C string.
     *
     * @param value String value.
     */
    Value(const char *value);

    /**
     * @brief Construct from string.
     *
     * @param value String value.
     */
    Value(std::string value);

    /**
     * @brief Construct from array.
     *
     * @param value Array value.
     */
    Value(Array value);

    /**
     * @brief Construct from object.
     *
     * @param value Object value.
     */
    Value(Object value);

    /**
     * @brief Check whether the value is null.
     *
     * @return True if null.
     */
    [[nodiscard]] bool is_null() const noexcept;

    /**
     * @brief Check whether the value is a boolean.
     *
     * @return True if boolean.
     */
    [[nodiscard]] bool is_bool() const noexcept;

    /**
     * @brief Check whether the value is an integer.
     *
     * @return True if integer.
     */
    [[nodiscard]] bool is_int() const noexcept;

    /**
     * @brief Check whether the value is a floating-point value.
     *
     * @return True if floating-point.
     */
    [[nodiscard]] bool is_double() const noexcept;

    /**
     * @brief Check whether the value is a string.
     *
     * @return True if string.
     */
    [[nodiscard]] bool is_string() const noexcept;

    /**
     * @brief Check whether the value is an array.
     *
     * @return True if array.
     */
    [[nodiscard]] bool is_array() const noexcept;

    /**
     * @brief Check whether the value is an object.
     *
     * @return True if object.
     */
    [[nodiscard]] bool is_object() const noexcept;

    /**
     * @brief Access the value as boolean.
     *
     * @return Boolean value.
     */
    [[nodiscard]] const bool &as_bool() const;

    /**
     * @brief Access the value as integer.
     *
     * @return Integer value.
     */
    [[nodiscard]] const std::int64_t &as_int() const;

    /**
     * @brief Access the value as floating-point.
     *
     * @return Floating-point value.
     */
    [[nodiscard]] const double &as_double() const;

    /**
     * @brief Access the value as string.
     *
     * @return String value.
     */
    [[nodiscard]] const std::string &as_string() const;

    /**
     * @brief Access the value as array.
     *
     * @return Array value.
     */
    [[nodiscard]] const Array &as_array() const;

    /**
     * @brief Access the value as object.
     *
     * @return Object value.
     */
    [[nodiscard]] const Object &as_object() const;

    /**
     * @brief Mutable access to the value as array.
     *
     * @return Array value.
     */
    [[nodiscard]] Array &as_array();

    /**
     * @brief Mutable access to the value as object.
     *
     * @return Object value.
     */
    [[nodiscard]] Object &as_object();

    /**
     * @brief Convert the value to a human-readable string.
     *
     * This is primarily used by the renderer for variable interpolation.
     *
     * @return String representation.
     */
    [[nodiscard]] std::string to_string() const;

    /**
     * @brief Evaluate truthiness for template conditions.
     *
     * Rules are intentionally simple and deterministic:
     * - null => false
     * - false => false
     * - 0 => false
     * - 0.0 => false
     * - empty string => false
     * - empty array => false
     * - empty object => false
     * - everything else => true
     *
     * @return Truthiness value.
     */
    [[nodiscard]] bool truthy() const noexcept;

    /**
     * @brief Get immutable access to the raw storage.
     *
     * @return Underlying variant storage.
     */
    [[nodiscard]] const Storage &storage() const noexcept;

  private:
    /**
     * @brief Underlying storage for the dynamic value.
     */
    Storage storage_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_VALUE_HPP

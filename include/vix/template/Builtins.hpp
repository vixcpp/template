/**
 *
 *  @file Builtins.hpp
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
#ifndef VIX_TEMPLATE_BUILTINS_HPP
#define VIX_TEMPLATE_BUILTINS_HPP

#include <functional>
#include <string>
#include <unordered_map>

#include <vix/template/Value.hpp>

namespace vix::template_
{
  /**
   * @brief Built-in filter function type.
   *
   * A filter takes a Value and returns a transformed Value.
   */
  using Filter = std::function<Value(const Value &)>;

  /**
   * @brief Built-in filters registry.
   *
   * Builtins provides a set of standard filters available to templates,
   * such as:
   * - upper
   * - lower
   * - length
   * - default
   */
  class Builtins
  {
  public:
    /**
     * @brief Get the default set of built-in filters.
     *
     * @return Map of filter name to filter function.
     */
    [[nodiscard]] static std::unordered_map<std::string, Filter> filters();
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_BUILTINS_HPP

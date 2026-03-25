/**
 *
 *  @file Escape.hpp
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
#ifndef VIX_TEMPLATE_ESCAPE_HPP
#define VIX_TEMPLATE_ESCAPE_HPP

#include <string>
#include <string_view>

namespace vix::template_
{
  /**
   * @brief Escape utilities for template rendering.
   *
   * This helper provides deterministic escaping functions used by the
   * renderer to safely emit HTML text.
   */
  class Escape
  {
  public:
    /**
     * @brief Escape a string for safe HTML output.
     *
     * The following characters are escaped:
     * - '&'  -> "&amp;"
     * - '<'  -> "&lt;"
     * - '>'  -> "&gt;"
     * - '"'  -> "&quot;"
     * - '\'' -> "&#39;"
     *
     * @param input Raw input string.
     * @return Escaped HTML string.
     */
    [[nodiscard]] static std::string html(std::string_view input);

    /**
     * @brief Check whether a character requires HTML escaping.
     *
     * @param c Character to inspect.
     * @return True if the character must be escaped.
     */
    [[nodiscard]] static bool needs_html_escape(char c) noexcept;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_ESCAPE_HPP

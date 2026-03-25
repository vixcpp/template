/**
 *
 *  @file RenderResult.hpp
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
#ifndef VIX_TEMPLATE_RENDER_RESULT_HPP
#define VIX_TEMPLATE_RENDER_RESULT_HPP

#include <string>

namespace vix::template_
{
  /**
   * @brief Result of a template rendering operation.
   *
   * This structure stores the final rendered output and a few useful
   * execution metadata flags for higher-level integrations.
   */
  struct RenderResult
  {
    /**
     * @brief Final rendered text output.
     */
    std::string output;

    /**
     * @brief Whether the rendered result came from cache.
     */
    bool from_cache{false};

    /**
     * @brief Whether HTML escaping was applied during rendering.
     */
    bool escaped{false};

    /**
     * @brief Whether the render completed successfully.
     */
    bool success{true};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_RENDER_RESULT_HPP

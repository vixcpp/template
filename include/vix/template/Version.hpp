/**
 *
 *  @file Version.hpp
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
#ifndef VIX_TEMPLATE_VERSION_HPP
#define VIX_TEMPLATE_VERSION_HPP

namespace vix::template_
{
  /**
   * @brief Major version of the template module.
   */
  inline constexpr int VERSION_MAJOR = TEMPLATE_VERSION_MAJOR;

  /**
   * @brief Minor version of the template module.
   */
  inline constexpr int VERSION_MINOR = TEMPLATE_VERSION_MINOR;

  /**
   * @brief Patch version of the template module.
   */
  inline constexpr int VERSION_PATCH = TEMPLATE_VERSION_PATCH;

  /**
   * @brief Full version string.
   */
  inline constexpr const char *VERSION = "0.1.0";
}

#endif // VIX_TEMPLATE_VERSION_HPP

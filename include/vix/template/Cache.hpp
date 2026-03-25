/**
 *
 *  @file Cache.hpp
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
#ifndef VIX_TEMPLATE_CACHE_HPP
#define VIX_TEMPLATE_CACHE_HPP

#include <memory>
#include <optional>
#include <string>

#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief Template cache interface.
   *
   * Cache stores compiled templates to avoid re-parsing and recompilation.
   *
   * It is intentionally simple and deterministic:
   * - no implicit expiration
   * - no background eviction
   * - explicit control by the caller
   */
  class Cache
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Cache() = default;

    /**
     * @brief Retrieve a cached template.
     *
     * @param name Template name.
     * @return Cached template if present.
     */
    [[nodiscard]] virtual std::optional<Template> get(
        const std::string &name) const = 0;

    /**
     * @brief Store a compiled template.
     *
     * @param name Template name.
     * @param tpl Compiled template.
     */
    virtual void put(
        const std::string &name,
        const Template &tpl) = 0;

    /**
     * @brief Remove a template from cache.
     *
     * @param name Template name.
     * @return True if removed.
     */
    [[nodiscard]] virtual bool erase(
        const std::string &name) = 0;

    /**
     * @brief Clear all cached templates.
     */
    virtual void clear() noexcept = 0;

    /**
     * @brief Check whether a template exists in cache.
     *
     * @param name Template name.
     * @return True if present.
     */
    [[nodiscard]] virtual bool has(
        const std::string &name) const = 0;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_CACHE_HPP

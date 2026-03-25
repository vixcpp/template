/**
 *
 *  @file Engine.hpp
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
#ifndef VIX_TEMPLATE_ENGINE_HPP
#define VIX_TEMPLATE_ENGINE_HPP

#include <memory>
#include <string>

#include <vix/template/Context.hpp>
#include <vix/template/Environment.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/RenderResult.hpp>
#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief High-level template engine facade.
   *
   * Engine provides the simplest public API for:
   * - loading templates by name
   * - rendering templates directly
   * - configuring common behavior such as HTML auto-escaping
   *
   * It wraps Environment and exposes a more ergonomic interface for
   * higher-level integrations such as HTTP handlers.
   */
  class Engine
  {
  public:
    /**
     * @brief Construct an engine from a loader.
     *
     * @param loader Template loader implementation.
     */
    explicit Engine(std::shared_ptr<Loader> loader);

    /**
     * @brief Set HTML auto-escaping behavior.
     *
     * @param enabled Whether auto-escaping is enabled.
     */
    void set_auto_escape(bool enabled) noexcept;

    /**
     * @brief Check whether HTML auto-escaping is enabled.
     *
     * @return True if enabled.
     */
    [[nodiscard]] bool auto_escape() const noexcept;

    /**
     * @brief Load and compile a template by name.
     *
     * @param name Template name.
     * @return Compiled template.
     */
    [[nodiscard]] Template load(const std::string &name) const;

    /**
     * @brief Render a template by name with a context.
     *
     * @param name Template name.
     * @param context Runtime rendering context.
     * @return Render result.
     */
    [[nodiscard]] RenderResult render(
        const std::string &name,
        const Context &context) const;

    /**
     * @brief Render a template by name from a raw object context.
     *
     * @param name Template name.
     * @param values Runtime rendering values.
     * @return Render result.
     */
    [[nodiscard]] RenderResult render(
        const std::string &name,
        const Object &values) const;

    /**
     * @brief Access the underlying environment.
     *
     * @return Immutable environment reference.
     */
    [[nodiscard]] const Environment &environment() const noexcept;

    /**
     * @brief Access the underlying environment.
     *
     * @return Mutable environment reference.
     */
    [[nodiscard]] Environment &environment() noexcept;

  private:
    /**
     * @brief Underlying template environment.
     */
    Environment environment_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_ENGINE_HPP

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

#include <vix/template/Cache.hpp>
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
   * - compiling templates
   * - caching compiled templates
   * - rendering templates directly
   * - configuring common behavior such as HTML auto-escaping
   *
   * In V7, Engine becomes the main orchestration layer for:
   * - Loader
   * - Environment
   * - compiled template cache
   *
   * The goal is to avoid paying the parse/compile cost repeatedly for
   * templates that are rendered frequently in production.
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
     * @brief Construct an engine from a loader and cache policy.
     *
     * @param loader Template loader implementation.
     * @param cache_policy Cache policy for compiled templates.
     */
    Engine(
        std::shared_ptr<Loader> loader,
        CachePolicy cache_policy);

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
     * @brief Enable or disable compiled template caching.
     *
     * @param enabled Whether cache usage is enabled.
     */
    void set_cache_enabled(bool enabled) noexcept;

    /**
     * @brief Check whether compiled template caching is enabled.
     *
     * @return True if cache is enabled.
     */
    [[nodiscard]] bool cache_enabled() const noexcept;

    /**
     * @brief Access the compiled template cache.
     *
     * @return Immutable cache reference.
     */
    [[nodiscard]] const Cache &cache() const noexcept;

    /**
     * @brief Access the compiled template cache.
     *
     * @return Mutable cache reference.
     */
    [[nodiscard]] Cache &cache() noexcept;

    /**
     * @brief Invalidate one cached template by name.
     *
     * @param name Template name.
     * @return True if a cache entry was removed.
     */
    bool invalidate(const std::string &name);

    /**
     * @brief Clear the entire compiled template cache.
     */
    void clear_cache() noexcept;

    /**
     * @brief Load and compile a template by name.
     *
     * If cache is enabled and a valid compiled template already exists,
     * it may be reused internally to avoid recompilation.
     *
     * @param name Template name.
     * @return Compiled template.
     */
    [[nodiscard]] Template load(const std::string &name);

    /**
     * @brief Load and compile a template by name as a shared compiled object.
     *
     * This is the preferred internal API for V7 because the cache stores
     * compiled templates as shared objects.
     *
     * @param name Template name.
     * @return Shared compiled template.
     */
    [[nodiscard]] std::shared_ptr<const Template> load_shared(
        const std::string &name);

    /**
     * @brief Render a template by name with a context.
     *
     * @param name Template name.
     * @param context Runtime rendering context.
     * @return Render result.
     */
    [[nodiscard]] RenderResult render(
        const std::string &name,
        const Context &context);

    /**
     * @brief Render a template by name from a raw object context.
     *
     * @param name Template name.
     * @param values Runtime rendering values.
     * @return Render result.
     */
    [[nodiscard]] RenderResult render(
        const std::string &name,
        const Object &values);

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
     * @brief Compute a freshness signature for a template source.
     *
     * In V7 this token is used to validate cache entries.
     * The initial implementation may simply return an empty string if
     * the loader cannot provide source versioning yet.
     *
     * @param name Template name.
     * @return Source signature, possibly empty.
     */
    [[nodiscard]] std::string make_source_signature(
        const std::string &name) const;

  private:
    /**
     * @brief Underlying template environment.
     */
    Environment environment_;

    /**
     * @brief Compiled template cache.
     */
    Cache cache_;

    /**
     * @brief Whether compiled template cache is enabled.
     */
    bool cache_enabled_{true};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_ENGINE_HPP

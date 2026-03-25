/**
 *
 *  @file Environment.hpp
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
#ifndef VIX_TEMPLATE_ENVIRONMENT_HPP
#define VIX_TEMPLATE_ENVIRONMENT_HPP

#include <memory>
#include <string>

#include <vix/template/Loader.hpp>
#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief Global template environment configuration.
   *
   * Environment coordinates:
   * - template loading (via Loader)
   * - template compilation (Lexer + Parser)
   * - rendering defaults (escaping, etc.)
   *
   * It is the main entry point for loading templates by name.
   */
  class Environment
  {
  public:
    /**
     * @brief Construct an environment.
     *
     * @param loader Template loader implementation.
     */
    explicit Environment(std::shared_ptr<Loader> loader);

    /**
     * @brief Set HTML auto-escaping behavior.
     *
     * @param enabled Whether auto-escaping is enabled.
     */
    void set_auto_escape(bool enabled) noexcept;

    /**
     * @brief Check whether auto-escaping is enabled.
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
     * @brief Get the underlying loader.
     *
     * @return Loader instance.
     */
    [[nodiscard]] const std::shared_ptr<Loader> &loader() const noexcept;

  private:
    /**
     * @brief Template loader.
     */
    std::shared_ptr<Loader> loader_;

    /**
     * @brief Default HTML auto-escaping setting.
     */
    bool auto_escape_{true};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_ENVIRONMENT_HPP

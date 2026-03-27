/**
 *
 *  @file StringLoader.hpp
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
#ifndef VIX_TEMPLATE_STRING_LOADER_HPP
#define VIX_TEMPLATE_STRING_LOADER_HPP

#include <map>
#include <string>

#include <vix/template/Loader.hpp>

namespace vix::template_
{
  /**
   * @brief In-memory template loader.
   *
   * StringLoader stores templates in memory and resolves them by name.
   * It is primarily used for:
   * - tests
   * - embedded templates
   * - simple runtime usage without filesystem dependency
   *
   * In V7, it also exposes a source signature so cache invalidation works
   * correctly when templates are updated at runtime.
   */
  class StringLoader : public Loader
  {
  public:
    /**
     * @brief Construct an empty loader.
     */
    StringLoader() = default;

    /**
     * @brief Add or replace a template.
     *
     * @param name Template name.
     * @param content Template content.
     */
    void set(std::string name, std::string content);

    /**
     * @brief Load a template by name.
     *
     * @param name Template name.
     * @return Template content.
     *
     * @throws LoaderError if the template does not exist.
     */
    [[nodiscard]] std::string load(const std::string &name) const override;

    /**
     * @brief Check whether a template exists.
     *
     * @param name Template name.
     * @return True if present.
     */
    [[nodiscard]] bool exists(const std::string &name) const override;

    /**
     * @brief Compute a source signature for an in-memory template.
     *
     * This is based on the template content itself. When content changes,
     * the signature changes → cache invalidation works automatically.
     *
     * @param name Template name.
     * @return Content-based signature, or empty if not found.
     */
    [[nodiscard]] std::string source_signature(
        const std::string &name) const override;

  private:
    /**
     * @brief Stored templates.
     */
    std::map<std::string, std::string> templates_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_STRING_LOADER_HPP

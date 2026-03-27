/**
 *
 *  @file FileSystemLoader.hpp
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
#ifndef VIX_TEMPLATE_FILESYSTEM_LOADER_HPP
#define VIX_TEMPLATE_FILESYSTEM_LOADER_HPP

#include <string>

#include <vix/template/Loader.hpp>

namespace vix::template_
{
  /**
   * @brief File system-based template loader.
   *
   * FileSystemLoader resolves template names to files on disk.
   * It is typically used in production to load templates from a directory.
   *
   * In V7, it can also expose a source signature for cache invalidation.
   */
  class FileSystemLoader : public Loader
  {
  public:
    /**
     * @brief Construct a loader with a root directory.
     *
     * @param root Root directory for template files.
     */
    explicit FileSystemLoader(std::string root);

    /**
     * @brief Load a template by name.
     *
     * @param name Template name or relative path.
     * @return Template content.
     *
     * @throws LoaderError if the file cannot be read.
     */
    [[nodiscard]] std::string load(const std::string &name) const override;

    /**
     * @brief Check whether a template exists.
     *
     * @param name Template name or relative path.
     * @return True if the file exists.
     */
    [[nodiscard]] bool exists(const std::string &name) const override;

    /**
     * @brief Compute a freshness signature for a template file.
     *
     * The returned string is intended for compiled template cache
     * invalidation. A typical implementation may use the file last write
     * timestamp, file size, or both.
     *
     * @param name Template name or relative path.
     * @return Source signature, or empty string if unavailable.
     */
    [[nodiscard]] std::string source_signature(
        const std::string &name) const override;

    /**
     * @brief Get the root directory.
     *
     * @return Root directory path.
     */
    [[nodiscard]] const std::string &root() const noexcept;

  private:
    /**
     * @brief Root directory for templates.
     */
    std::string root_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_FILESYSTEM_LOADER_HPP

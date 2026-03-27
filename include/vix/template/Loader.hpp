/**
 *
 *  @file Loader.hpp
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
#ifndef VIX_TEMPLATE_LOADER_HPP
#define VIX_TEMPLATE_LOADER_HPP

#include <string>

namespace vix::template_
{
  /**
   * @brief Abstract interface for template loading.
   *
   * A loader is responsible for resolving a template name and returning
   * its raw textual content.
   *
   * Implementations may load templates from:
   * - in-memory strings
   * - the file system
   * - embedded assets
   * - any custom backend
   *
   * In V7, Loader may also expose a source signature used by the cache
   * layer to detect stale compiled templates.
   */
  class Loader
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Loader() = default;

    /**
     * @brief Load a template by name.
     *
     * @param name Logical template name or path.
     * @return Raw template source.
     */
    [[nodiscard]] virtual std::string load(const std::string &name) const = 0;

    /**
     * @brief Check whether a template exists.
     *
     * @param name Logical template name or path.
     * @return True if the template can be loaded.
     */
    [[nodiscard]] virtual bool exists(const std::string &name) const = 0;

    /**
     * @brief Compute a source freshness signature for a template.
     *
     * This value is used by the compiled template cache to determine
     * whether a cached template is still valid.
     *
     * Implementations may return:
     * - a last-modified timestamp
     * - a content hash
     * - a revision token
     * - any stable freshness identifier
     *
     * The default implementation returns an empty string, which means
     * "no freshness information available".
     *
     * @param name Logical template name or path.
     * @return Source signature, possibly empty.
     */
    [[nodiscard]] virtual std::string source_signature(
        const std::string &name) const
    {
      (void)name;
      return {};
    }
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_LOADER_HPP

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

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <mutex>

namespace vix::template_
{
  class Template;

  /**
   * @brief Cache policy for compiled templates.
   *
   * V7 introduces a production-ready cache layer that stores already
   * compiled templates in memory in order to avoid:
   * - re-loading
   * - re-parsing
   * - re-optimizing
   * - re-compiling
   *
   * TTL is optional:
   * - no TTL means "keep until explicitly invalidated"
   * - TTL set means entries expire automatically when checked
   */
  struct CachePolicy
  {
    /**
     * @brief Optional time-to-live for each cache entry.
     *
     * When std::nullopt, entries do not expire by time.
     */
    std::optional<std::chrono::milliseconds> ttl{std::nullopt};

    /**
     * @brief Whether cached templates should refresh their access time
     * on successful reads.
     *
     * Useful if later you want LRU-like behavior or cache observability.
     */
    bool touch_on_read{true};
  };

  /**
   * @brief Single compiled template cache entry.
   *
   * Each entry stores:
   * - the compiled template object
   * - the source signature used to validate freshness
   * - creation timestamp
   * - last access timestamp
   */
  struct CacheEntry
  {
    /**
     * @brief Cached compiled template.
     */
    std::shared_ptr<const Template> value;

    /**
     * @brief Source signature associated with this compiled template.
     *
     * This can be:
     * - a file modification fingerprint
     * - a content hash
     * - a revision string
     * - any caller-defined freshness token
     *
     * Empty means "no source signature provided".
     */
    std::string source_signature;

    /**
     * @brief Entry creation time.
     */
    std::chrono::steady_clock::time_point created_at{
        std::chrono::steady_clock::now()};

    /**
     * @brief Last successful access time.
     */
    std::chrono::steady_clock::time_point last_access_at{
        created_at};

    /**
     * @brief Check whether this entry is expired for the provided policy.
     *
     * @param policy Active cache policy.
     * @param now Current time point.
     * @return True if expired.
     */
    [[nodiscard]] bool expired(
        const CachePolicy &policy,
        std::chrono::steady_clock::time_point now) const noexcept
    {
      if (!policy.ttl.has_value())
      {
        return false;
      }

      return (now - created_at) >= *policy.ttl;
    }

    /**
     * @brief Check whether this entry still matches a given source signature.
     *
     * Rules:
     * - if expected signature is empty, freshness check is considered valid
     * - if cached signature is empty but expected is not, entry is stale
     * - otherwise exact match is required
     *
     * @param expected_signature Caller-provided freshness token.
     * @return True if the entry matches the expected signature.
     */
    [[nodiscard]] bool matches_signature(
        const std::string &expected_signature) const noexcept
    {
      if (expected_signature.empty())
      {
        return true;
      }

      if (source_signature.empty())
      {
        return false;
      }

      return source_signature == expected_signature;
    }
  };

  /**
   * @brief Thread-safe in-memory cache for compiled templates.
   *
   * This cache is intentionally focused on one responsibility:
   * store and retrieve already compiled Template instances.
   *
   * Design goals:
   * - zero re-parse cost on cache hit
   * - explicit invalidation support
   * - optional TTL expiration
   * - thread-safe reads and writes
   * - simple integration with Engine / Template APIs
   */
  class Cache
  {
  public:
    using clock = std::chrono::steady_clock;

    /**
     * @brief Construct a cache with the default policy.
     */
    Cache() = default;

    /**
     * @brief Construct a cache with a custom policy.
     *
     * @param policy Initial cache policy.
     */
    explicit Cache(CachePolicy policy)
        : policy_(std::move(policy))
    {
    }

    /**
     * @brief Get the active cache policy.
     *
     * @return Immutable cache policy.
     */
    [[nodiscard]] const CachePolicy &policy() const noexcept
    {
      return policy_;
    }

    /**
     * @brief Replace the active cache policy.
     *
     * @param policy New cache policy.
     */
    void set_policy(CachePolicy policy)
    {
      std::unique_lock lock(mutex_);
      policy_ = std::move(policy);
    }

    /**
     * @brief Set or clear the default TTL.
     *
     * Passing std::nullopt disables time-based expiration.
     *
     * @param ttl New TTL.
     */
    void set_ttl(std::optional<std::chrono::milliseconds> ttl)
    {
      std::unique_lock lock(mutex_);
      policy_.ttl = ttl;
    }

    /**
     * @brief Check whether the cache is empty.
     *
     * @return True if there are no cached templates.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      std::shared_lock lock(mutex_);
      return entries_.empty();
    }

    /**
     * @brief Get the number of cached templates.
     *
     * @return Cache entry count.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      std::shared_lock lock(mutex_);
      return entries_.size();
    }

    /**
     * @brief Check whether a non-stale entry exists for a template name.
     *
     * This method also validates TTL and optional source signature.
     *
     * @param name Logical template name.
     * @param source_signature Optional freshness token.
     * @return True if a valid entry exists.
     */
    [[nodiscard]] bool contains(
        const std::string &name,
        const std::string &source_signature = "") const
    {
      std::shared_lock lock(mutex_);

      const auto it = entries_.find(name);
      if (it == entries_.end())
      {
        return false;
      }

      const auto now = clock::now();
      const CacheEntry &entry = it->second;

      if (entry.expired(policy_, now))
      {
        return false;
      }

      return entry.matches_signature(source_signature);
    }

    /**
     * @brief Retrieve a compiled template from the cache.
     *
     * Returns nullptr if:
     * - the entry does not exist
     * - the entry is expired
     * - the source signature no longer matches
     *
     * When touch_on_read is enabled, last_access_at is updated on hit.
     *
     * @param name Logical template name.
     * @param source_signature Optional freshness token.
     * @return Cached compiled template, or nullptr on miss.
     */
    [[nodiscard]] std::shared_ptr<const Template> get(
        const std::string &name,
        const std::string &source_signature = "")
    {
      std::unique_lock lock(mutex_);

      const auto it = entries_.find(name);
      if (it == entries_.end())
      {
        return nullptr;
      }

      const auto now = clock::now();
      CacheEntry &entry = it->second;

      if (entry.expired(policy_, now))
      {
        entries_.erase(it);
        return nullptr;
      }

      if (!entry.matches_signature(source_signature))
      {
        return nullptr;
      }

      if (policy_.touch_on_read)
      {
        entry.last_access_at = now;
      }

      return entry.value;
    }

    /**
     * @brief Insert or replace a compiled template in the cache.
     *
     * @param name Logical template name.
     * @param value Compiled template instance.
     * @param source_signature Optional freshness token.
     */
    void put(
        std::string name,
        std::shared_ptr<const Template> value,
        std::string source_signature = "")
    {
      CacheEntry entry;
      entry.value = std::move(value);
      entry.source_signature = std::move(source_signature);
      entry.created_at = clock::now();
      entry.last_access_at = entry.created_at;

      std::unique_lock lock(mutex_);
      entries_[std::move(name)] = std::move(entry);
    }

    /**
     * @brief Remove a cached template by name.
     *
     * @param name Logical template name.
     * @return True if an entry was removed.
     */
    bool invalidate(const std::string &name)
    {
      std::unique_lock lock(mutex_);
      return entries_.erase(name) > 0;
    }

    /**
     * @brief Remove all expired entries.
     *
     * @return Number of removed entries.
     */
    std::size_t invalidate_expired()
    {
      std::unique_lock lock(mutex_);

      const auto now = clock::now();
      std::size_t removed = 0;

      for (auto it = entries_.begin(); it != entries_.end();)
      {
        if (it->second.expired(policy_, now))
        {
          it = entries_.erase(it);
          ++removed;
        }
        else
        {
          ++it;
        }
      }

      return removed;
    }

    /**
     * @brief Remove all entries from the cache.
     */
    void clear() noexcept
    {
      std::unique_lock lock(mutex_);
      entries_.clear();
    }

  private:
    /**
     * @brief Cache policy.
     */
    CachePolicy policy_{};

    /**
     * @brief Stored compiled templates indexed by logical name.
     */
    std::unordered_map<std::string, CacheEntry> entries_;

    /**
     * @brief Read/write lock protecting the cache.
     */
    mutable std::shared_mutex mutex_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_CACHE_HPP

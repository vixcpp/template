/**
 *
 *  @file Context.cpp
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
#include <vix/template/Context.hpp>

#include <utility>

namespace vix::template_
{
  Context::Context(Object values)
      : values_(std::move(values))
  {
  }

  void Context::set(std::string key, Value value)
  {
    values_[std::move(key)] = std::move(value);
  }

  bool Context::has(const std::string &key) const
  {
    return values_.find(key) != values_.end();
  }

  const Value *Context::get(const std::string &key) const noexcept
  {
    const auto it = values_.find(key);
    if (it == values_.end())
    {
      return nullptr;
    }

    return &it->second;
  }

  Value *Context::get(const std::string &key) noexcept
  {
    const auto it = values_.find(key);
    if (it == values_.end())
    {
      return nullptr;
    }

    return &it->second;
  }

  Value Context::get_or(const std::string &key, Value fallback) const
  {
    const auto it = values_.find(key);
    if (it == values_.end())
    {
      return fallback;
    }

    return it->second;
  }

  bool Context::erase(const std::string &key)
  {
    return values_.erase(key) > 0;
  }

  void Context::clear() noexcept
  {
    values_.clear();
  }

  bool Context::empty() const noexcept
  {
    return values_.empty();
  }

  std::size_t Context::size() const noexcept
  {
    return values_.size();
  }

  const Object &Context::values() const noexcept
  {
    return values_;
  }

  Object &Context::values() noexcept
  {
    return values_;
  }

} // namespace vix::template_

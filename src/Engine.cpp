/**
 *
 *  @file Engine.cpp
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
#include <vix/template/Engine.hpp>

#include <utility>

namespace vix::template_
{
  Engine::Engine(std::shared_ptr<Loader> loader)
      : environment_(std::move(loader))
  {
  }

  Engine::Engine(
      std::shared_ptr<Loader> loader,
      CachePolicy cache_policy)
      : environment_(std::move(loader)),
        cache_(std::move(cache_policy))
  {
  }

  void Engine::set_auto_escape(bool enabled) noexcept
  {
    environment_.set_auto_escape(enabled);
  }

  bool Engine::auto_escape() const noexcept
  {
    return environment_.auto_escape();
  }

  void Engine::set_cache_enabled(bool enabled) noexcept
  {
    cache_enabled_ = enabled;
  }

  bool Engine::cache_enabled() const noexcept
  {
    return cache_enabled_;
  }

  const Cache &Engine::cache() const noexcept
  {
    return cache_;
  }

  Cache &Engine::cache() noexcept
  {
    return cache_;
  }

  bool Engine::invalidate(const std::string &name)
  {
    return cache_.invalidate(name);
  }

  void Engine::clear_cache() noexcept
  {
    cache_.clear();
  }

  Template Engine::load(const std::string &name)
  {
    Template compiled = environment_.load(name);
    compiled.set_source_signature(make_source_signature(name));
    return compiled;
  }

  std::shared_ptr<const Template> Engine::load_shared(const std::string &name)
  {
    const std::string source_signature = make_source_signature(name);

    if (cache_enabled_)
    {
      if (auto cached = cache_.get(name, source_signature))
      {
        return cached;
      }
    }

    Template compiled = environment_.load(name);
    compiled.set_source_signature(source_signature);

    auto shared_compiled =
        std::make_shared<Template>(std::move(compiled));

    if (cache_enabled_)
    {
      cache_.put(name, shared_compiled, source_signature);
    }

    return shared_compiled;
  }

  RenderResult Engine::render(
      const std::string &name,
      const Context &context)
  {
    const std::string source_signature = make_source_signature(name);
    bool from_cache = false;

    std::shared_ptr<const Template> tpl;

    if (cache_enabled_)
    {
      tpl = cache_.get(name, source_signature);
      if (tpl)
      {
        from_cache = true;
      }
    }

    if (!tpl)
    {
      Template compiled = environment_.load(name);
      compiled.set_source_signature(source_signature);

      auto shared_compiled =
          std::make_shared<Template>(std::move(compiled));

      if (cache_enabled_)
      {
        cache_.put(name, shared_compiled, source_signature);
      }

      tpl = std::move(shared_compiled);
    }

    if (!tpl)
    {
      return RenderResult{
          .output = "",
          .from_cache = false,
          .escaped = auto_escape(),
          .success = false};
    }

    RenderResult result = tpl->render(context, auto_escape());
    result.from_cache = from_cache;
    return result;
  }

  RenderResult Engine::render(
      const std::string &name,
      const Object &values)
  {
    return render(name, Context(values));
  }

  const Environment &Engine::environment() const noexcept
  {
    return environment_;
  }

  Environment &Engine::environment() noexcept
  {
    return environment_;
  }

  std::string Engine::make_source_signature(const std::string &name) const
  {
    if (const auto &loader = environment_.loader())
    {
      return loader->source_signature(name);
    }

    return {};
  }

} // namespace vix::template_

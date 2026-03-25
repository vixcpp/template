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

  void Engine::set_auto_escape(bool enabled) noexcept
  {
    environment_.set_auto_escape(enabled);
  }

  bool Engine::auto_escape() const noexcept
  {
    return environment_.auto_escape();
  }

  Template Engine::load(const std::string &name) const
  {
    return environment_.load(name);
  }

  RenderResult Engine::render(
      const std::string &name,
      const Context &context) const
  {
    Template tpl = load(name);
    return tpl.render(context, auto_escape());
  }

  RenderResult Engine::render(
      const std::string &name,
      const Object &values) const
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

} // namespace vix::template_

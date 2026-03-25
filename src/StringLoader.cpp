/**
 *
 *  @file StringLoader.cpp
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
#include <vix/template/StringLoader.hpp>
#include <vix/template/Error.hpp>

#include <utility>

namespace vix::template_
{
  void StringLoader::set(std::string name, std::string content)
  {
    templates_[std::move(name)] = std::move(content);
  }

  std::string StringLoader::load(const std::string &name) const
  {
    const auto it = templates_.find(name);
    if (it == templates_.end())
    {
      throw LoaderError("template not found: " + name);
    }

    return it->second;
  }

  bool StringLoader::exists(const std::string &name) const
  {
    return templates_.find(name) != templates_.end();
  }

} // namespace vix::template_

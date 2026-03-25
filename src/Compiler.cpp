/**
 *
 *  @file Compiler.cpp
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
#include <vix/template/Compiler.hpp>

#include <utility>

namespace vix::template_
{
  Template Compiler::compile(
      std::string name,
      RootNode root) const
  {
    // V1: direct pass-through (no optimization yet)
    return Template(std::move(name), std::move(root));
  }

} // namespace vix::template_

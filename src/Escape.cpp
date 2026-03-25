/**
 *
 *  @file Escape.cpp
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
#include <vix/template/Escape.hpp>

namespace vix::template_
{
  bool Escape::needs_html_escape(char c) noexcept
  {
    switch (c)
    {
    case '&':
    case '<':
    case '>':
    case '"':
    case '\'':
      return true;
    default:
      return false;
    }
  }

  std::string Escape::html(std::string_view input)
  {
    std::string output;
    output.reserve(input.size());

    for (char c : input)
    {
      switch (c)
      {
      case '&':
        output += "&amp;";
        break;
      case '<':
        output += "&lt;";
        break;
      case '>':
        output += "&gt;";
        break;
      case '"':
        output += "&quot;";
        break;
      case '\'':
        output += "&#39;";
        break;
      default:
        output.push_back(c);
        break;
      }
    }

    return output;
  }

} // namespace vix::template_

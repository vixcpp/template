/**
 *
 *  @file Builtins.cpp
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
#include <vix/template/Builtins.hpp>

#include <algorithm>
#include <cctype>

namespace vix::template_
{
  namespace
  {
    Value filter_upper(const Value &v)
    {
      if (!v.is_string())
      {
        return v;
      }

      std::string s = v.as_string();
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                     { return static_cast<char>(std::toupper(c)); });

      return Value(std::move(s));
    }

    Value filter_lower(const Value &v)
    {
      if (!v.is_string())
      {
        return v;
      }

      std::string s = v.as_string();
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                     { return static_cast<char>(std::tolower(c)); });

      return Value(std::move(s));
    }

    Value filter_length(const Value &v)
    {
      if (v.is_string())
      {
        return Value(static_cast<std::int64_t>(v.as_string().size()));
      }

      if (v.is_array())
      {
        return Value(static_cast<std::int64_t>(v.as_array().size()));
      }

      if (v.is_object())
      {
        return Value(static_cast<std::int64_t>(v.as_object().size()));
      }

      return Value(0);
    }

    Value filter_default(const Value &v)
    {
      if (v.truthy())
      {
        return v;
      }

      return Value(""); // V1: default empty
    }
  } // namespace

  std::unordered_map<std::string, Filter> Builtins::filters()
  {
    return {
        {"upper", filter_upper},
        {"lower", filter_lower},
        {"length", filter_length},
        {"default", filter_default},
    };
  }

} // namespace vix::template_

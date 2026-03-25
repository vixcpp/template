/**
 *
 *  @file Value.cpp
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
#include <vix/template/Value.hpp>

#include <sstream>
#include <stdexcept>

namespace vix::template_
{
  Value::Value()
      : storage_(nullptr)
  {
  }

  Value::Value(std::nullptr_t value)
      : storage_(value)
  {
  }

  Value::Value(bool value)
      : storage_(value)
  {
  }

  Value::Value(int value)
      : storage_(static_cast<std::int64_t>(value))
  {
  }

  Value::Value(std::int64_t value)
      : storage_(value)
  {
  }

  Value::Value(double value)
      : storage_(value)
  {
  }

  Value::Value(const char *value)
      : storage_(value == nullptr ? std::string{} : std::string(value))
  {
  }

  Value::Value(std::string value)
      : storage_(std::move(value))
  {
  }

  Value::Value(Array value)
      : storage_(std::move(value))
  {
  }

  Value::Value(Object value)
      : storage_(std::move(value))
  {
  }

  bool Value::is_null() const noexcept
  {
    return std::holds_alternative<std::nullptr_t>(storage_);
  }

  bool Value::is_bool() const noexcept
  {
    return std::holds_alternative<bool>(storage_);
  }

  bool Value::is_int() const noexcept
  {
    return std::holds_alternative<std::int64_t>(storage_);
  }

  bool Value::is_double() const noexcept
  {
    return std::holds_alternative<double>(storage_);
  }

  bool Value::is_string() const noexcept
  {
    return std::holds_alternative<std::string>(storage_);
  }

  bool Value::is_array() const noexcept
  {
    return std::holds_alternative<Array>(storage_);
  }

  bool Value::is_object() const noexcept
  {
    return std::holds_alternative<Object>(storage_);
  }

  const bool &Value::as_bool() const
  {
    return std::get<bool>(storage_);
  }

  const std::int64_t &Value::as_int() const
  {
    return std::get<std::int64_t>(storage_);
  }

  const double &Value::as_double() const
  {
    return std::get<double>(storage_);
  }

  const std::string &Value::as_string() const
  {
    return std::get<std::string>(storage_);
  }

  const Array &Value::as_array() const
  {
    return std::get<Array>(storage_);
  }

  const Object &Value::as_object() const
  {
    return std::get<Object>(storage_);
  }

  Array &Value::as_array()
  {
    return std::get<Array>(storage_);
  }

  Object &Value::as_object()
  {
    return std::get<Object>(storage_);
  }

  std::string Value::to_string() const
  {
    if (is_null())
    {
      return "";
    }

    if (is_bool())
    {
      return as_bool() ? "true" : "false";
    }

    if (is_int())
    {
      return std::to_string(as_int());
    }

    if (is_double())
    {
      std::ostringstream oss;
      oss << as_double();
      return oss.str();
    }

    if (is_string())
    {
      return as_string();
    }

    if (is_array())
    {
      return "[array]";
    }

    if (is_object())
    {
      return "[object]";
    }

    return "";
  }

  bool Value::truthy() const noexcept
  {
    if (is_null())
    {
      return false;
    }

    if (is_bool())
    {
      return as_bool();
    }

    if (is_int())
    {
      return as_int() != 0;
    }

    if (is_double())
    {
      return as_double() != 0.0;
    }

    if (is_string())
    {
      return !as_string().empty();
    }

    if (is_array())
    {
      return !as_array().empty();
    }

    if (is_object())
    {
      return !as_object().empty();
    }

    return false;
  }

  const Value::Storage &Value::storage() const noexcept
  {
    return storage_;
  }

} // namespace vix::template_

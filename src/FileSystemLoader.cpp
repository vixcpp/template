/**
 *
 *  @file FileSystemLoader.cpp
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
#include <vix/template/FileSystemLoader.hpp>
#include <vix/template/Error.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace vix::template_
{
  namespace
  {
    std::string join_path(const std::string &root, const std::string &name)
    {
      if (root.empty())
      {
        return name;
      }

      if (root.back() == '/')
      {
        return root + name;
      }

      return root + "/" + name;
    }
  } // namespace

  FileSystemLoader::FileSystemLoader(std::string root)
      : root_(std::move(root))
  {
  }

  std::string FileSystemLoader::load(const std::string &name) const
  {
    const std::string path = join_path(root_, name);

    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
    {
      throw LoaderError("failed to open template file: " + path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
  }

  bool FileSystemLoader::exists(const std::string &name) const
  {
    const std::string path = join_path(root_, name);

    std::ifstream file(path, std::ios::in | std::ios::binary);
    return file.good();
  }

  std::string FileSystemLoader::source_signature(
      const std::string &name) const
  {
    namespace fs = std::filesystem;

    const std::string path = join_path(root_, name);

    std::error_code ec;
    const fs::file_time_type last_write = fs::last_write_time(path, ec);

    if (ec)
    {
      return {};
    }

    const std::int64_t since_epoch =
        static_cast<std::int64_t>(last_write.time_since_epoch().count());

    return std::to_string(since_epoch);
  }

  const std::string &FileSystemLoader::root() const noexcept
  {
    return root_;
  }

} // namespace vix::template_

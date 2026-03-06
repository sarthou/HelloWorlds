#include "hello_worlds/Utils/AssetResolver.h"

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace hws {

  void AssetResolver::registerPackage(const std::string& name, const std::string& path)
  {
    if(*(path.rbegin()) == '/')
      package_map_[name] = path.substr(0, path.size() - 1);
    else
      package_map_[name] = path;
  }

  void AssetResolver::addSearchDirectory(const std::string& path)
  {
    search_dirs_.push_back(path);
  }

  void AssetResolver::clear()
  {
    package_map_.clear();
    search_dirs_.clear();
    package_cache_.clear();
  }

  std::string AssetResolver::getFullPath(const std::string& uri)
  {
    const std::string prefix = "package://";
    if(uri.compare(0, prefix.size(), prefix) != 0)
      return uri;

    size_t first_slash = uri.find('/', prefix.size());
    if(first_slash == std::string::npos)
      return "";

    std::string pkg_name = uri.substr(prefix.size(), first_slash - prefix.size());
    std::string relative_path = uri.substr(first_slash);

    if(package_map_.contains(pkg_name))
      return package_map_[pkg_name] + relative_path;

    std::string searched_path = findPackageRoot(pkg_name);
    if(searched_path.empty() == false)
    {
      package_cache_[pkg_name] = searched_path;
      return searched_path + relative_path;
    }
    /*for(const auto& dir : search_dirs_)
    {
      fs::path p = fs::path(dir) / pkg_name;
      if(fs::exists(p))
      {
        package_cache_[pkg_name] = p.string();
        return p.string() + relative_path;
      }
    }*/

    std::string env_path = findInEnv(pkg_name);
    if(!env_path.empty())
    {
      package_cache_[pkg_name] = env_path;
      return env_path + relative_path;
    }

    return "";
  }

  std::string AssetResolver::findPackageRoot(const std::string& pkg_name)
  {
    for(const auto& dir : search_dirs_)
    {
      if(!fs::exists(dir))
        continue;

      fs::path shallow = fs::path(dir) / pkg_name;
      if(fs::exists(shallow) && fs::is_directory(shallow))
      {
        return shallow.string();
      }

      try
      {
        for(const auto& entry : fs::recursive_directory_iterator(dir,
                                                                 fs::directory_options::skip_permission_denied |
                                                                   fs::directory_options::follow_directory_symlink))
        {
          if(entry.is_directory() && entry.path().filename() == pkg_name)
          {
            return entry.path().string();
          }
        }
      }
      catch(...)
      {
        continue;
      }
    }

    return "";
  }

  std::string AssetResolver::findInEnv(const std::string& pkg_name)
  {
    // Try ROS 2 Ament Paths (looks in <prefix>/share/<pkg_name>)
    if(const char* ament_ptr = std::getenv("AMENT_PREFIX_PATH"))
    {
      std::string path = searchInPathList(ament_ptr, pkg_name, "/share/");
      if(!path.empty())
        return path;
    }

    // Try ROS 1 Package Paths (looks directly in <path>/<pkg_name>)
    if(const char* ros_ptr = std::getenv("ROS_PACKAGE_PATH"))
    {
      std::string path = searchInPathList(ros_ptr, pkg_name, "");
      if(!path.empty())
        return path;
    }

    return "";
  }

  std::string AssetResolver::searchInPathList(const std::string& list, const std::string& pkg, const std::string& suffix)
  {
    std::stringstream ss(list);
    std::string item;
    while(std::getline(ss, item, ':'))
    {
      fs::path potential = fs::path(item).string() + suffix + pkg;
      if(fs::exists(potential))
        return potential.string();
    }
    return "";
  }

} // namespace hws
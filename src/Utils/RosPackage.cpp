#include "hello_worlds/Utils/RosPackage.h"

#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

namespace hws {

  inline std::vector<std::string> split(const std::string& text, const std::string& delim)
  {
    std::vector<std::string> res;
    std::string tmp_text = text;
    while(tmp_text.find(delim) != std::string::npos)
    {
      size_t pos = tmp_text.find(delim);
      std::string part = tmp_text.substr(0, pos);
      tmp_text = tmp_text.substr(pos + delim.size(), tmp_text.size() - pos - delim.size());
      if(part.empty() == false)
        res.emplace_back(part);
    }
    res.emplace_back(tmp_text);
    return res;
  }

  std::string execCmd(std::string cmd)
  {
    std::array<char, 128> buffer;
    std::string result;
    cmd.append(" 2>&1");
    FILE* pipe = popen(cmd.c_str(), "r");
    if(pipe == nullptr)
      throw std::runtime_error("popen() failed!");
    try
    {
      while(fgets(buffer.data(), sizeof buffer, pipe) != nullptr)
        result += buffer.data();
    }
    catch(...)
    {
      pclose(pipe);
      return "";
    }

    pclose(pipe);
    if(result.find("not found") != std::string::npos)
      return "";

    return result;
  }

  std::string findPackageRos1(const std::string& pkg_name)
  {
    std::string results = execCmd("rospack find " + pkg_name);
    if(results.empty() == false)
    {
      auto split_res = split(results, "\n");
      return split_res.front();
    }
    else
      return "";
  }

  std::string findPackageRos2(const std::string& pkg_name)
  {
    std::string results = execCmd("ros2 pkg prefix " + pkg_name);
    if(results.empty() == false)
    {
      auto split_res = split(results, "\n");
      return split_res.front() + "/share/" + pkg_name;
    }
    else
      return "";
  }

  std::string findPackage(const std::string& pkg_name)
  {
    std::string res = findPackageRos1(pkg_name);
    if(res.empty())
      res = findPackageRos2(pkg_name);
    return res;
  }

  std::string getFullPath(const std::string& file_name)
  {
    std::string package_pattern = "package://";

    if(file_name.find(package_pattern) != std::string::npos)
    {
      size_t pose = file_name.find(package_pattern);
      size_t pose_end_of_name = file_name.find('/', pose + package_pattern.size());
      std::string full_package = file_name.substr(pose, pose_end_of_name - pose);
      std::string package_name = file_name.substr(pose + package_pattern.size(), pose_end_of_name - pose - package_pattern.size());
      std::string package_path = findPackage(package_name);

      std::string full_file_name = file_name;
      full_file_name.replace(pose, full_package.size(), package_path);
      return full_file_name;
    }
    else
      return file_name;
  }

} // namespace hws
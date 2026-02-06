#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>

namespace hws {

  class AssetResolver
  {
  public:
    static AssetResolver& instance()
    {
      static AssetResolver instance_;
      return instance_;
    }

    AssetResolver(const AssetResolver&) = delete;
    void operator=(const AssetResolver&) = delete;

    void registerPackage(const std::string& name, const std::string& path);
    void addSearchDirectory(const std::string& path);
    void clear();

    std::string getFullPath(const std::string& uri);

  private:
    AssetResolver() = default;
    std::unordered_map<std::string, std::string> package_map_;
    std::unordered_map<std::string, std::string> package_cache_;
    std::vector<std::string> search_dirs_;

    std::string findPackageRoot(const std::string& pkg_name);
    std::string findInEnv(const std::string& pkg_name);

    std::string searchInPathList(const std::string& list, const std::string& pkg, const std::string& suffix);
  };

} // namespace hws
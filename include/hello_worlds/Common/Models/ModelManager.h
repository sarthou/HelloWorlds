#ifndef HWS_COMMON_MODELMANAGER_H
#define HWS_COMMON_MODELMANAGER_H

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "hello_worlds/Common/Models/Loaders/ModelLoader.h"

namespace hws {
  class Model;

  class ModelManager
  {
    ModelManager();

  public:
    ~ModelManager() noexcept;
    ModelManager(const ModelManager& other) = delete;
    ModelManager(ModelManager&& other) = delete;

    static ModelManager& get();

    hws::Model& load(const std::filesystem::path& path);

  protected:
    ModelLoader model_loader_;
    std::unordered_map<std::string, std::unique_ptr<hws::Model>> models_;
    std::mutex mut_;
  };
} // namespace hws

#endif // HWS_COMMON_MODELMANAGER_H

#ifndef HWS_COMMON_MODELMANAGER_H
#define HWS_COMMON_MODELMANAGER_H

#include <cstddef>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "hello_worlds/Common/Models/Loaders/ModelLoader.h"
#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"

namespace hws {

  class ModelManager
  {
  public:
    static ModelManager& get();

    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    size_t load(const std::filesystem::path& path);
    Model const* loadAndGet(const std::filesystem::path& path);

    // Returns nullptr if not found
    Model const* getModel(size_t id) const;
    Mesh const* getMesh(size_t id) const;

    size_t getLoadedModelCount() const;
    bool isModelLoaded(const std::filesystem::path& path) const;

  private:
    ModelManager();
    ~ModelManager() = default;

    mutable std::mutex mut_;
    ModelLoader model_loader_;
    std::unordered_map<std::string, size_t> ids_;
    std::vector<std::unique_ptr<Model>> models_;

    std::unordered_map<std::string, size_t> meshes_ids_;
    std::vector<Mesh> meshes_;
  };
} // namespace hws

#endif // HWS_COMMON_MODELMANAGER_H

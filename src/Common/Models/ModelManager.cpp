#include "hello_worlds/Common/Models/ModelManager.h"

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hello_worlds/Common/Models/Loaders/ModelLoader.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Utils/ShellDisplay.h"

namespace hws {
  ModelManager::ModelManager()
  {
    // Reserve index 0 for "Null Model"
    models_.emplace_back(nullptr);
  }

  ModelManager& ModelManager::get()
  {
    static ModelManager instance;
    return instance;
  }

  size_t ModelManager::load(const std::filesystem::path& path)
  {
    std::string key = std::filesystem::absolute(path).string();

    {
      std::lock_guard<std::mutex> lock(mut_);
      if(ids_.contains(key))
      {
        return ids_[key];
      }
    }

    auto model = model_loader_.load(path);
    if(model == nullptr)
    {
      ShellDisplay::error("Fail to load model: " + key);
      return 0;
    }

    std::lock_guard<std::mutex> lock(mut_);

    if(ids_.contains(key))
    {
      return ids_[key];
    }

    size_t new_id = models_.size();
    models_.emplace_back(std::move(model));
    ids_[key] = new_id;

    return new_id;
  }

  Model const* ModelManager::loadAndGet(const std::filesystem::path& path)
  {
    size_t id = load(path);
    return getModel(id);
  }

  Model const* ModelManager::getModel(size_t id) const
  {
    std::lock_guard<std::mutex> lock(mut_);
    return (id < models_.size()) ? models_[id].get() : nullptr;
  }

  bool ModelManager::isModelLoaded(const std::filesystem::path& path) const
  {
    std::string key = std::filesystem::absolute(path).string();
    std::lock_guard<std::mutex> lock(mut_);
    return ids_.contains(key);
  }

  size_t ModelManager::getLoadedModelCount() const
  {
    std::lock_guard<std::mutex> lock(mut_);
    return models_.size() - 1; // Subtract the null model at index 0
  }

} // namespace hws
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
#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Utils/ShellDisplay.h"

namespace hws {
  ModelManager::ModelManager()
  {
    // Reserve index 0 for "Null Model"
    models_.emplace_back(nullptr);
    meshes_.emplace_back(0);
  }

  ModelManager& ModelManager::get()
  {
    static ModelManager instance_;
    return instance_;
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

    auto raw_model = model_loader_.load(path);
    if(raw_model == nullptr)
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
    models_.emplace_back(new Model(new_id));
    models_.back()->source_path_ = std::move(raw_model->source_path_);
    models_.back()->meshes_.reserve(raw_model->meshes_.size());

    for(auto& mesh : raw_model->meshes_)
    {
      if(mesh.name_.empty())
        mesh.name_ = models_.back()->source_path_;

      meshes_ids_.emplace(mesh.name_, meshes_.size());
      meshes_.emplace_back(meshes_.size());
      meshes_.back().indices_ = std::move(mesh.indices_);
      meshes_.back().vertices_ = std::move(mesh.vertices_);
      meshes_.back().name_ = std::move(mesh.name_);
      meshes_.back().material_ = std::move(mesh.material_);
      meshes_.back().finalize();

      models_.back()->meshes_.push_back(meshes_.back().getId());
    }
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

  Mesh const* ModelManager::getMesh(size_t id) const
  {
    std::lock_guard<std::mutex> lock(mut_);
    return (id < meshes_.size()) ? &meshes_[id] : nullptr;
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
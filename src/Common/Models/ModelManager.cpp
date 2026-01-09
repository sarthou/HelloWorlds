#include "hello_worlds/Common/Models/ModelManager.h"

#include <cassert>
#include <filesystem>
#include <utility>

#include "hello_worlds/Common/Models/Loaders/ModelLoader.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Utils/ShellDisplay.h"

namespace hws {
  ModelManager::ModelManager() = default;
  ModelManager::~ModelManager() noexcept = default;

  ModelManager& ModelManager::get()
  {
    static ModelManager mgr_{};
    return mgr_;
  }

  hws::Model& ModelManager::load(const std::filesystem::path& path)
  {
    const auto absolute_path_str = path.string();

    mut_.lock();
    if(models_.contains(absolute_path_str) == false)
    {
      auto model = model_loader_.load(path);

      if(model == nullptr)
        ShellDisplay::error("Fail to load model: " + path.string());
      assert(model && "Failed to load model");

      models_[absolute_path_str] = std::move(model);
    }

    hws::Model& res = *models_[absolute_path_str];
    mut_.unlock();

    return res;
  }
} // namespace hws
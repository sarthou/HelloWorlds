#ifndef HWS_COMMON_MODELLOADER_H
#define HWS_COMMON_MODELLOADER_H

#include <filesystem>
#include <memory>

#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"

namespace hws {

  class ModelLoader
  {
  public:
    std::unique_ptr<hws::Model> load(const std::filesystem::path& path) const;

  private:
    void computeTangentSpace(const std::unique_ptr<hws::Model>& model) const;
    void computeTangentSpace(Mesh& mesh) const;
  };

} // namespace hws

#endif // HWS_COMMON_MODELLOADER_H

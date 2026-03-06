#ifndef HWS_COMMON_MODELLOADER_H
#define HWS_COMMON_MODELLOADER_H

#include <filesystem>
#include <memory>

#include "hello_worlds/Common/Models/RawModel.h"

namespace hws {

  class ModelLoader
  {
  public:
    std::unique_ptr<RawModel_t> load(const std::filesystem::path& path) const;

  private:
    void computeTangentSpace(const std::unique_ptr<RawModel_t>& model) const;
    void computeTangentSpace(RawMesh_t& mesh) const;
  };

} // namespace hws

#endif // HWS_COMMON_MODELLOADER_H

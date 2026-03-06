#ifndef HWS_COMMON_BASE_RAWMODEL_H
#define HWS_COMMON_BASE_RAWMODEL_H

#include <array>
#include <string>
#include <vector>

#include "hello_worlds/Common/Models/Material.h"
#include "hello_worlds/Common/Models/Vertex.h"

namespace hws {

  struct RawMesh_t
  {
    std::string name_;
    std::vector<hws::Vertex> vertices_;
    std::vector<std::uint32_t> indices_;
    Material material_;
  };

  struct RawModel_t
  {
    std::string source_path_;
    std::vector<RawMesh_t> meshes_;
  };

} // namespace hws

#endif // HWS_COMMON_BASE_RAWMODEL_H

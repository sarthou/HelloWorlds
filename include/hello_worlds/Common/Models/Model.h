#ifndef HWS_COMMON_BASE_MODEL_H
#define HWS_COMMON_BASE_MODEL_H

#include <array>
#include <string>

#include "hello_worlds/Common/Models/Material.h"
#include "hello_worlds/Common/Models/Mesh.h"

namespace hws {
  class Model
  {
  public:
    explicit Model(size_t id);

    size_t getId() const { return id_; }

    std::string source_path_;
    std::vector<size_t> meshes_;

  private:
    size_t id_; // Each model is associated with a non-zero, unique id.
  };
} // namespace hws

#endif // HWS_COMMON_BASE_MODEL_H

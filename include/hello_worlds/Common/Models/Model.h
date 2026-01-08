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
    enum class Id_e : std::size_t
    {
    };

    static Model create();

    explicit Model(Id_e id);

    // As for actors, each model is associated with a non-zero, unique id.
    const Id_e id_;

    std::string source_path_;
    std::vector<hws::Mesh> meshes_;
    std::array<float, 3> scale_;
  };
} // namespace hws

#endif // HWS_COMMON_BASE_MODEL_H

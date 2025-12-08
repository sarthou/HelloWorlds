#ifndef HWS_COMMON_SKYBOX_H
#define HWS_COMMON_SKYBOX_H

#include <memory>

#include "glm/matrix.hpp"
#include "hello_worlds/Common/Models/Material.h"

namespace hws {
  class Model;

  class ShapeSkybox
  {
  public:
    hws::Material material_;
    std::reference_wrapper<hws::Model> skybox_model_;
    glm::mat4 shape_transform_{1.};
  };
} // namespace hws

#endif // HWS_COMMON_SKYBOX_H
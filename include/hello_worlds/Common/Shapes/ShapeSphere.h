#ifndef HWS_COMMON_SPHERE_H
#define HWS_COMMON_SPHERE_H

#include <memory>

#include "glm/matrix.hpp"
#include "hello_worlds/Common/Models/Color.h"

namespace hws {
  class Model;

  class ShapeSphere
  {
  public:
    float radius_{};
    hws::Color diffuse_color_;
    std::reference_wrapper<hws::Model> sphere_model_;
    glm::mat4 shape_transform_{1.};
  };
} // namespace hws

#endif // HWS_COMMON_SPHERE_H

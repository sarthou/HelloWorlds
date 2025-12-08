#ifndef HWS_COMMON_CAPSULE_H
#define HWS_COMMON_CAPSULE_H

#include <memory>

#include "glm/matrix.hpp"
#include "hello_worlds/Common/Models/Color.h"

namespace hws {
  class Model;

  /**
   * todo: Turns out we don't need this I think
   */
  class ShapeCapsule
  {
  public:
    float radius_{};
    float height_{};
    hws::Color diffuse_color_;
    std::reference_wrapper<hws::Model> cylinder_model_;
    std::reference_wrapper<hws::Model> sphere_model_;
    glm::mat4 shape_transform_{1.};
  };
} // namespace hws

#endif // HWS_COMMON_CAPSULE_H

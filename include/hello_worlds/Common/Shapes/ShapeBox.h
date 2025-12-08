#ifndef HWS_COMMON_BOX_H
#define HWS_COMMON_BOX_H

#include <array>
#include <memory>

#include "glm/matrix.hpp"
#include "hello_worlds/Common/Models/Color.h"

namespace hws {
  class Model;

  class ShapeBox
  {
  public:
    std::array<float, 3> half_extents_{};
    hws::Color diffuse_color_;
    std::reference_wrapper<hws::Model> box_model_;
    glm::mat4 shape_transform_{1.};
  };
} // namespace hws

#endif // HWS_COMMON_BOX_H
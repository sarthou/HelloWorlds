#ifndef HWS_COMMON_CUSTOMMESH_H
#define HWS_COMMON_CUSTOMMESH_H

#include <array>
#include <glm/vec3.hpp>
#include <memory>

#include "glm/matrix.hpp"
#include "hello_worlds/Common/Models/Material.h"

namespace hws {
  class Model;

  /**
   * WORK IN PROGRESS
   */
  class ShapeCustomMesh
  {
  public:
    glm::vec3 scale_ = {1.f, 1.f, 1.f};
    hws::Material material_;
    std::reference_wrapper<hws::Model> custom_model_;
    glm::mat4 shape_transform_{1.};
  };
} // namespace hws

#endif // HWS_COMMON_CUSTOMMESH_H

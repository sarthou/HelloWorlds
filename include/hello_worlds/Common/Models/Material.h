#ifndef HWS_COMMON_BASE_MATERIAL_H
#define HWS_COMMON_BASE_MATERIAL_H

#include <string>

#include "hello_worlds/Common/Models/Color.h"

namespace hws {
  class Material
  {
  public:
    std::string name_;
    hws::Color diffuse_color_;
    hws::Color specular_color_;
    float shininess_ = -1.;
    std::string diffuse_texture_;
    std::string specular_texture_;
    std::string normal_texture_;
  };
} // namespace hws

#endif // HWS_COMMON_BASE_MATERIAL_H

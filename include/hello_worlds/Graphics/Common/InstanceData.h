#ifndef HWS_GRAPHICS_COMMON_INSTANCEDATA_H
#define HWS_GRAPHICS_COMMON_INSTANCEDATA_H

#include <glm/matrix.hpp>

#include "hello_worlds/Common/Models/Color.h"

namespace hws {
  class InstanceData
  {
  public:
    glm::mat4 mvp_;
    hws::Color rgba_;
  };
} // namespace hws

#endif // HWS_GRAPHICS_COMMON_INSTANCEDATA_H

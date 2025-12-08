#ifndef HWS_DEBUGTEXT_H
#define HWS_DEBUGTEXT_H

#include <glm/vec3.hpp>
#include <string>

namespace hws {

  class Actor;

  struct DebugText_t
  {
    std::string text;
    bool centered = false;
    glm::vec3 position;
    glm::vec3 color;
    float height = 0.2;
    double remaining_time = 0;
    Actor* linked_actor = nullptr;
  };

} // namespace hws

#endif // HWS_DEBUGTEXT_H
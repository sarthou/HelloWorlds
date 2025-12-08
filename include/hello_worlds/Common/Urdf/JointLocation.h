#ifndef HWS_COMMON_JOINTLOCATION_H
#define HWS_COMMON_JOINTLOCATION_H

#include <array>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace hws {
  class Actor;
  class JointLocation
  {
  public:
    JointLocation(
      hws::Actor& parent,
      const glm::vec3& origin_position,
      const glm::quat& origin_orientation,
      hws::Actor& child,
      const glm::vec3& joint_position,
      const glm::quat& joint_orientation)
      : parent_(parent),
        origin_position_(origin_position),
        origin_orientation_(origin_orientation),
        child_(child),
        joint_position_(joint_position),
        joint_orientation_(joint_orientation)
    {}

    hws::Actor& parent_;
    glm::vec3 origin_position_;
    glm::quat origin_orientation_;
    hws::Actor& child_;
    glm::vec3 joint_position_;
    glm::quat joint_orientation_;
  };
} // namespace hws

#endif // HWS_COMMON_JOINTLOCATION_H

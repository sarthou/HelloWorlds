#ifndef HWS_COMMON_JOINT_H
#define HWS_COMMON_JOINT_H

#include <cstddef>

#include "hello_worlds/Common/Urdf/UrdfLoader.h"

namespace hws {

  class Joint
  {
  protected:
    Joint(urdf::JointType_e type, int axis, int direction = 1);

  public:
    virtual ~Joint() noexcept = default;

    void setLimits(double low, double high);

    virtual void setPosition(double position) = 0;

    virtual void setVelocity(double velocity) = 0;

    // Each actor is associated with a non-zero, unique id.
    const std::size_t unique_id_{};
    urdf::JointType_e type_;
    int axis_;      // 0 = x, 1 = y, 2 = z
    int direction_; // 1 or -1
    bool has_limits_;
    double limit_low_;
    double limit_high_;
  };
} // namespace hws

#endif // HWS_COMMON_JOINT_H

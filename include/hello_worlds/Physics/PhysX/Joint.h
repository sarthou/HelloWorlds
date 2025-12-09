#ifndef OWDS_PHYSICS_PHYSX_JOINT_H
#define OWDS_PHYSICS_PHYSX_JOINT_H

#include "hello_worlds/Common/Urdf/Joint.h"
#include "hello_worlds/Common/Urdf/UrdfLoader.h"
#include "hello_worlds/Physics/PhysX/API.h"

namespace hws::physx {

  class Context;

  class Joint : public hws::Joint
  {
  public:
    Joint(hws::physx::Context& ctx,
          ::physx::PxArticulationJointReducedCoordinate* px_joint,
          urdf::JointType_e type, int axis, int direction = 1);

    ~Joint() noexcept override = default;

    Joint(const Joint& other) = delete;
    Joint& operator=(const Joint& other) = delete;

    Joint(Joint&& other) noexcept = delete;
    Joint& operator=(Joint&& other) = delete;

    void setPosition(double position) override;
    void setVelocity(double velocity) override;

  protected:
    hws::physx::Context& ctx_;

    ::physx::PxArticulationAxis::Enum px_axis_;

    PxPtr<::physx::PxArticulationJointReducedCoordinate> px_base_;
  };
} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_JOINT_H
#ifndef OWDS_PHYSICS_PHYSX_DYNAMICACTOR_H
#define OWDS_PHYSICS_PHYSX_DYNAMICACTOR_H

#include <array>
#include <memory>
#include <vector>

#include "hello_worlds/Physics/PhysX/API.h"
#include "hello_worlds/Physics/PhysX/Actors/Actor.h"

namespace hws::physx {

  class DynamicActor final : public hws::physx::Actor
  {
  public:
    DynamicActor(
      hws::physx::Context& ctx,
      const hws::Shape& collision_shape,
      const std::vector<hws::Shape>& visual_shapes);

    ~DynamicActor() noexcept override;

    DynamicActor(const DynamicActor& other) = delete;
    DynamicActor& operator=(const DynamicActor& other) = delete;

    DynamicActor(DynamicActor&& other) noexcept = delete;
    DynamicActor& operator=(DynamicActor&& other) = delete;

    void setup(const std::array<double, 3>& position,
               const std::array<double, 4>& orientation) override;
    void remove() override;

    void setMass(float mass_kg) override;

    void setMode(hws::ActorMode_e mode) override;

    void setPositionAndOrientation() override;
    void setPositionAndOrientation(const std::array<double, 3>& position, const std::array<double, 4>& orientation) override;
    void stepPose() override;
    void resetSubsteping() override;
    void setVelocity(const std::array<double, 3>& linear_velocity, const std::array<double, 3>& angular_velocity) override;

  private:
    PxPtr<::physx::PxRigidDynamic> px_actor_;

    static constexpr int RECOVERY_STEPS = 10; // Number of sub-steps to finish a transition

    ::physx::PxTransform goal_pose_;
    ::physx::PxTransform interpolation_start_;

    int catchup_remaining_steps_;
    bool is_kinematic_;
    bool was_kinematic_;
    bool has_first_pose_;
    bool pending_data_gap_;
  };

} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_DYNAMICACTOR_H
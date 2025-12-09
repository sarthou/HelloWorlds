#ifndef OWDS_PHYSICS_PHYSX_STATICACTOR_H
#define OWDS_PHYSICS_PHYSX_STATICACTOR_H

#include <array>
#include <memory>
#include <vector>

#include "hello_worlds/Physics/PhysX/API.h"
#include "hello_worlds/Physics/PhysX/Actors/Actor.h"

namespace hws::physx {

  class StaticActor final : public hws::physx::Actor
  {
  public:
    StaticActor(
      hws::physx::Context& ctx,
      const hws::Shape& collision_shape,
      const std::vector<hws::Shape>& visual_shapes);

    ~StaticActor() noexcept override;

    StaticActor(const StaticActor& other) = delete;
    StaticActor& operator=(const StaticActor& other) = delete;

    StaticActor(StaticActor&& other) noexcept = delete;
    StaticActor& operator=(StaticActor&& other) = delete;

    void setup(const std::array<double, 3>& position,
               const std::array<double, 4>& orientation) override;
    void remove() override;

    void setMass(float mass_kg) override;

    void setPhysicsEnabled(bool enabled) override;
    void setSimulationEnabled(bool enabled) override;

    void setPositionAndOrientation(const std::array<double, 3>& position, const std::array<double, 4>& orientation) override;
    void setVelocity(const std::array<double, 3>& linear_velocity, const std::array<double, 3>& angular_velocity) override;

  private:
    PxPtr<::physx::PxRigidStatic> px_actor_;
  };

} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_STATICACTOR_H
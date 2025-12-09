#ifndef OWDS_PHYSICS_PHYSX_LINKACTOR_H
#define OWDS_PHYSICS_PHYSX_LINKACTOR_H

#include <array>
#include <memory>
#include <vector>

#include "hello_worlds/Physics/PhysX/API.h"
#include "hello_worlds/Physics/PhysX/Actors/Actor.h"

namespace hws::physx {

  class Urdf;

  class LinkActor final : public hws::physx::Actor
  {
    friend Urdf;

  public:
    LinkActor(
      hws::physx::Context& ctx,
      ::physx::PxArticulationLink* px_link,
      size_t body_id,
      const hws::Shape& collision_shape,
      const std::vector<hws::Shape>& visual_shapes);

    ~LinkActor() noexcept override;

    LinkActor(const LinkActor& other) = delete;
    LinkActor& operator=(const LinkActor& other) = delete;

    LinkActor(LinkActor&& other) noexcept = delete;
    LinkActor& operator=(LinkActor&& other) = delete;

    void setup(const std::array<double, 3>& position,
               const std::array<double, 4>& orientation) override;
    void remove() override;

    void setMass(float mass_kg) override;

    void setPhysicsEnabled(bool enabled) override;
    void setSimulationEnabled(bool enabled) override;

    void setPositionAndOrientation(const std::array<double, 3>& position, const std::array<double, 4>& orientation) override;
    void setVelocity(const std::array<double, 3>& linear_velocity, const std::array<double, 3>& angular_velocity) override;

  private:
    PxPtr<::physx::PxArticulationLink> px_link_;
    size_t body_id_;
  };

} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_LINKACTOR_H
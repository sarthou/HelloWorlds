#include "hello_worlds/Physics/PhysX/Actors/DynamicActor.h"

#include <algorithm>
#include <array>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Common/Shapes/Shape.h"
#include "hello_worlds/Physics/PhysX/Actors/Actor.h"
#include "hello_worlds/Physics/PhysX/Context.h"
#include "hello_worlds/Physics/PhysX/SharedContext.h"

namespace hws::physx {

  // Helper for Linear Interpolation of PxTransform
  ::physx::PxTransform interpolateTransform(const ::physx::PxTransform& start, const ::physx::PxTransform& end, float t)
  {
    t = std::clamp(t, 0.0f, 1.0f);
    return ::physx::PxTransform(
      start.p * (1.0f - t) + end.p * t,
      ::physx::PxSlerp(t, start.q, end.q));
  }

  DynamicActor::DynamicActor(hws::physx::Context& ctx,
                             const hws::Shape& collision_shape,
                             const std::vector<hws::Shape>& visual_shapes) : hws::physx::Actor(ctx, collision_shape, visual_shapes),
                                                                             catchup_remaining_steps_(0),
                                                                             is_kinematic_(false),
                                                                             was_kinematic_(false),
                                                                             has_first_pose_(false),
                                                                             pending_data_gap_(false)
  {}

  DynamicActor::~DynamicActor() noexcept = default;

  void DynamicActor::setup(const std::array<double, 3>& position,
                           const std::array<double, 4>& orientation)
  {
    ctx_.physx_mutex_.lock();
    const auto& sdk = hws::physx::Context::createContext()->px_physics_;

    px_material_ = sdk->createMaterial(0.5f, 0.5f, 0.5f);

    std::visit(([this](auto& elem) { setupPhysicsShape(elem); }), collision_shape_);

    px_actor_ = sdk->createRigidDynamic(::physx::PxTransform(::physx::PxVec3(0, 0, 0)));
    px_base_ = px_actor_.get();

    for(const auto& px_geometry : px_geometries_)
    {
      px_shapes_.emplace_back(sdk->createShape(
        *px_geometry,
        *px_material_,
        false,
        ::physx::PxShapeFlag::eSCENE_QUERY_SHAPE | ::physx::PxShapeFlag::eSIMULATION_SHAPE));
      px_shapes_.back()->setContactOffset(0.05); // default is 0.02
      px_shapes_.back()->setRestOffset(0.0);     // default is 0.0
      px_actor_->attachShape(*px_shapes_.back());
    }

    px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eENABLE_GYROSCOPIC_FORCES, true);
    // px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eENABLE_CCD, true);
    px_actor_->setSolverIterationCounts(8, 4);

    ActorData_t* data = new ActorData_t();
    data->actor_id = unique_id_;
    data->body_id = -1;
    px_actor_->userData = data;

    ctx_.px_scene_->addActor(*px_actor_);
    ctx_.physx_mutex_.unlock();

    setPositionAndOrientation(position, orientation);

    setMode(hws::ActorMode_e::kinematic_mode);
  }

  void DynamicActor::setMode(hws::ActorMode_e mode)
  {
    std::lock_guard<std::mutex> lock(ctx_.physx_mutex_);

    px_actor_->setLinearVelocity(::physx::PxVec3(0, 0, 0));
    px_actor_->setAngularVelocity(::physx::PxVec3(0, 0, 0));
    if(is_kinematic_ == false)
    {
      px_actor_->clearForce();
      px_actor_->clearTorque();
    }

    px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_SIMULATION, false);

    switch(mode)
    {
    case hws::ActorMode_e::static_mode:
      px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eKINEMATIC, true);
      px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_GRAVITY, true);
      px_actor_->putToSleep();
      is_kinematic_ = true;
      was_kinematic_ = false; // Forces next move to be a Teleport
      break;

    case hws::ActorMode_e::simulated_mode:
      px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eKINEMATIC, false);
      px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_GRAVITY, false);
      px_actor_->wakeUp();
      is_kinematic_ = false;
      was_kinematic_ = false;
      break;

    case hws::ActorMode_e::kinematic_mode:
      px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eKINEMATIC, true);
      px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_GRAVITY, true);
      px_actor_->wakeUp();
      is_kinematic_ = true;
      was_kinematic_ = true; // Enables interpolation
      break;

    case hws::ActorMode_e::ghost_mode:
      px_actor_->setRigidBodyFlag(::physx::PxRigidBodyFlag::eKINEMATIC, true);
      px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_GRAVITY, true);
      px_actor_->setActorFlag(::physx::PxActorFlag::eDISABLE_SIMULATION, true);
      is_kinematic_ = true;
      was_kinematic_ = false;
      break;
    }

    // Ensure the next position update re-anchors the interpolation
    has_first_pose_ = false;
    catchup_remaining_steps_ = 0;
  }

  void DynamicActor::remove()
  {
    std::lock_guard<std::mutex> lock(ctx_.physx_mutex_);
    ctx_.px_scene_->removeActor(*px_actor_);
  }

  void DynamicActor::setMass(const float mass_kg)
  {
    std::lock_guard<std::mutex> lock(ctx_.physx_mutex_);
    px_actor_->setMass(static_cast<::physx::PxReal>(mass_kg));
  }

  void DynamicActor::setPositionAndOrientation()
  {
    pending_data_gap_ = true;
  }

  void DynamicActor::setPositionAndOrientation(const std::array<double, 3>& position, const std::array<double, 4>& orientation)
  {
    const auto px_transform =
      ::physx::PxTransform(
        ::physx::PxVec3(
          static_cast<::physx::PxReal>(position[0]),
          static_cast<::physx::PxReal>(position[1]),
          static_cast<::physx::PxReal>(position[2])),
        ::physx::PxQuat(
          static_cast<::physx::PxReal>(orientation[0]),
          static_cast<::physx::PxReal>(orientation[1]),
          static_cast<::physx::PxReal>(orientation[2]),
          static_cast<::physx::PxReal>(orientation[3])));

    std::lock_guard<std::mutex> lock(ctx_.physx_mutex_);
    goal_pose_ = px_transform;

    if(is_kinematic_ && was_kinematic_)
    {
      if(!has_first_pose_ || pending_data_gap_)
      {
        interpolation_start_ = px_actor_->getGlobalPose();

        catchup_remaining_steps_ = (RECOVERY_STEPS * (int)ctx_.sub_step_);

        pending_data_gap_ = false;
        has_first_pose_ = true;
      }

      // IMPORTANT: We must set the target for Sub-step 0 immediately.
      // If we are in catch-up mode, we calculate the first t.
      // If not, we just set the goal_pose_.
      if(catchup_remaining_steps_ > 0)
      {
        float t = 1.0f - (static_cast<float>(catchup_remaining_steps_) / (float)(RECOVERY_STEPS * (int)ctx_.sub_step_));
        px_actor_->setKinematicTarget(interpolateTransform(interpolation_start_, goal_pose_, t));
        catchup_remaining_steps_--;
      }
      else
      {
        px_actor_->setKinematicTarget(goal_pose_);
      }
    }
    else // Teleport (Static/Ghost/Snap)
    {
      px_actor_->setGlobalPose(px_transform);
      was_kinematic_ = is_kinematic_;
      has_first_pose_ = true;
      pending_data_gap_ = false;
      catchup_remaining_steps_ = 0;
    }
  }

  void DynamicActor::stepPose()
  {
    if(!is_kinematic_ || !was_kinematic_ || !has_first_pose_)
      return;

    if(catchup_remaining_steps_ > 0) // Handle Interpolation
    {
      // Linear progression from 0.0 to 1.0
      float t = 1.0f - (static_cast<float>(catchup_remaining_steps_) / (float)(RECOVERY_STEPS * (int)ctx_.sub_step_));

      ::physx::PxTransform next_step = interpolateTransform(interpolation_start_, goal_pose_, t);
      px_actor_->setKinematicTarget(next_step);

      catchup_remaining_steps_--;
    }
    else // We have reached the goal, just maintain it
    {
      px_actor_->setKinematicTarget(goal_pose_);
    }
  }

  void DynamicActor::resetSubsteping()
  {
    has_first_pose_ = false;
  }

  void DynamicActor::setVelocity(const std::array<double, 3>& linear_velocity, const std::array<double, 3>& angular_velocity)
  {
    if(is_kinematic_)
      return;

    std::lock_guard<std::mutex> lock(ctx_.physx_mutex_);

    px_actor_->setLinearVelocity(::physx::PxVec3(
      static_cast<::physx::PxReal>(linear_velocity[0]),
      static_cast<::physx::PxReal>(linear_velocity[1]),
      static_cast<::physx::PxReal>(linear_velocity[2])));

    px_actor_->setAngularVelocity(::physx::PxVec3(
      static_cast<::physx::PxReal>(angular_velocity[0]),
      static_cast<::physx::PxReal>(angular_velocity[1]),
      static_cast<::physx::PxReal>(angular_velocity[2])));
  }

} // namespace hws::physx
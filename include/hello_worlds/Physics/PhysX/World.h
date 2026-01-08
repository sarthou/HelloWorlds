#ifndef OWDS_PHYSICS_PHYSX_WORLD_H
#define OWDS_PHYSICS_PHYSX_WORLD_H

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#include "hello_worlds/Common/World.h"

namespace hws::physx {
  class Context;

  class World final : public hws::World
  {
  public:
    explicit World(const std::filesystem::path& base_assets_path);
    ~World() override;

    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    // todo: deal with move ctor / assignment move operator
    World(World&& other) = delete;
    World& operator=(World&& other) = delete;

    [[nodiscard]] std::string getBackendName() const override;
    [[nodiscard]] std::string getFullyQualifiedBackendName() const override;
    void setSubstepping(size_t sub_step) override;

    using hws::World::createActor;
    using hws::World::createStaticActor;
    using hws::World::loadUrdf;

    size_t createActor(const hws::Shape& collision_shape,
                       const std::vector<hws::Shape>& visual_shapes,
                       const std::array<double, 3>& position,
                       const std::array<double, 4>& orientation) override;

    size_t createStaticActor(const hws::Shape& collision_shape,
                             const std::vector<hws::Shape>& visual_shapes,
                             const std::array<double, 3>& position,
                             const std::array<double, 4>& orientation) override;

    hws::Urdf* loadUrdf(const urdf::Urdf_t& model) override;
    void insertUrdf(hws::Urdf* urdf) override;

    void setGravity(const std::array<float, 3>& gravity) override;
    void stepSimulation(float delta = 0) override;

  protected:
    std::unique_ptr<hws::physx::Context> ctx_;

    std::unordered_set<int> getOverlappingActors(::hws::Actor* actor) override;

  private:
    void performRaycastsInParallel(const std::vector<std::array<double, 3>>& origins,
                                   const std::vector<std::array<double, 3>>& destinations,
                                   float max_distance,
                                   std::vector<RaycastHitResult_t>& results) override;
  };
} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_WORLD_H

#ifndef OWDS_PHYSICS_PHYSX_CONTEXT_H
#define OWDS_PHYSICS_PHYSX_CONTEXT_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "hello_worlds/Physics/PhysX/API.h"

namespace hws::physx {

  class SharedContext;

  /**
   * INTERNAL DATA STRUCTURE
   */
  class Context
  {
  public:
    explicit Context(bool minimize_memory_footprint, const std::string& debugger_address = "0.0.0.0");
    ~Context();

    Context(const Context& other) = delete;
    Context& operator=(const Context& other) = delete;

    Context(Context&& other) noexcept = delete;
    Context& operator=(Context&& other) = delete;

    PxPtr<::physx::PxScene> px_scene_;
    std::mutex physx_mutex_;
    size_t sub_step_ = 1;

    bool minimize_memory_footprint_;

    static std::unique_ptr<hws::physx::SharedContext>& createContext();

    std::unique_ptr<hws::physx::SharedContext>& shared_ctx_;
  };

} // namespace hws::physx

#endif // OWDS_PHYSICS_PHYSX_CONTEXT_H

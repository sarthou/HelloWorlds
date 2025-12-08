#include "hello_worlds/Common/Urdf/Actor.h"

#include <cstddef>
#include <vector>

#include "hello_worlds/Common/Shapes/Shape.h"

namespace hws {

  static std::size_t s_global_id_counter = 1;

  Actor::Actor(const hws::Shape& collision_shape,
               const std::vector<hws::Shape>& visual_shapes) : unique_id_(s_global_id_counter++),
                                                               collision_shape_(collision_shape),
                                                               visual_shapes_(visual_shapes)
  {}

  Actor::~Actor() noexcept = default;

} // namespace hws
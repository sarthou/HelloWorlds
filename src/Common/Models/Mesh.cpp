#include "hello_worlds/Common/Models/Mesh.h"

#include <cstddef>

namespace hws {
  static std::size_t s_global_id_counter = 1;

  Mesh Mesh::create()
  {
    return Mesh(static_cast<Id_e>(s_global_id_counter++));
  }

  Mesh::Mesh(const Id_e id) : id_(id)
  {}
} // namespace hws
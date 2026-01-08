#include "hello_worlds/Common/Models/Model.h"

#include <cstddef>

namespace hws {
  static std::size_t s_global_id_counter = 1;

  Model Model::create()
  {
    return Model(static_cast<Id_e>(s_global_id_counter++));
  }

  Model::Model(const Id_e id) : id_(id)
  {}
} // namespace hws
#include "hello_worlds/Common/Models/Model.h"

namespace hws {
  static std::size_t s_global_id_counter = 1;

  Model Model::create()
  {
    return Model(static_cast<Id>(s_global_id_counter++));
  }

  Model::Model(const Id id) : id_(id)
  {}
} // namespace hws
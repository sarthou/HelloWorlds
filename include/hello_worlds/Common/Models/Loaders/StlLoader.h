#ifndef OVERWORLD_STLLOADER_H
#define OVERWORLD_STLLOADER_H

#include <memory>
#include <string>

#include "hello_worlds/Common/Models/Model.h"

namespace hws {

  class StlLoader
  {
  public:
    static std::unique_ptr<hws::Model> read(const std::string& path);
  };

} // namespace hws

#endif // OVERWORLD_STLLOADER_H
#ifndef OVERWORLD_OBJLOADER_H
#define OVERWORLD_OBJLOADER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "hello_worlds/Common/Models/Loaders/TinyObjReader.h"
#include "hello_worlds/Common/Models/Model.h"

namespace hws {

  class ObjLoader : public tinyobj::ObjReader
  {
  public:
    std::unique_ptr<hws::Model> read(const std::string& path);

  private:
    Vertex getVertex(const tinyobj::Attrib_t& attribute,
                     const tinyobj::Index_t& indexes);

    std::unique_ptr<hws::Model> getModel(const tinyobj::Attrib_t& attribute,
                                         std::vector<tinyobj::Mesh_t>& meshes,
                                         std::vector<tinyobj::Material_t>& materials);
  };

} // namespace hws

#endif // OVERWORLD_OBJLOADER_H
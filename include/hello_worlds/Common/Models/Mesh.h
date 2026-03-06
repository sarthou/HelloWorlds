#ifndef HWS_COMMON_BASE_MESH_H
#define HWS_COMMON_BASE_MESH_H

#include <string>
#include <vector>

#include "hello_worlds/Common/Models/Material.h"
#include "hello_worlds/Common/Models/Vertex.h"

namespace hws {

  class Mesh
  {
  public:
    explicit Mesh(size_t id);

    std::string name_;
    std::vector<hws::Vertex> vertices_;
    std::vector<std::uint32_t> indices_;
    Material material_;

    void finalize();

    size_t getId() const { return id_; }

    float getRadius() const { return radius_; }
    glm::vec3 getCenter() const { return center_; }

  private:
    size_t id_; // Each mesh is associated with a non-zero, unique id.

    float radius_;
    glm::vec3 center_; // Local space center
  };
} // namespace hws

#endif // HWS_COMMON_BASE_MESH_H

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
    enum class Id_e : std::size_t
    {
    };

    static Mesh create();

    explicit Mesh(Id_e id);

    // As for actors, each mesh is associated with a non-zero, unique id.
    const Id_e id_;

    std::string name_;
    std::vector<hws::Vertex> vertices_;
    std::vector<std::uint32_t> indices_;
    Material material_;

    void finalize();

    float getRadius() const { return radius_; }
    glm::vec3 getCenter() const { return center_; }

  private:
    float radius_;
    glm::vec3 center_; // Local space center
  };
} // namespace hws

#endif // HWS_COMMON_BASE_MESH_H

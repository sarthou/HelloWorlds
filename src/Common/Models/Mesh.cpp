#include "hello_worlds/Common/Models/Mesh.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace hws {
  Mesh::Mesh(size_t id) : id_(id)
  {}

  void Mesh::finalize()
  {
    glm::vec3 min_bound(1e10f), max_bound(-1e10f);
    for(const auto& v : vertices_)
    {
      min_bound = glm::min(min_bound, v.position_);
      max_bound = glm::max(max_bound, v.position_);
    }
    center_ = (min_bound + max_bound) * 0.5f;

    float max_dist_sq = 0.0f;
    for(const auto& v : vertices_)
    {
      float dist_sq = glm::distance2(center_, v.position_);
      if(dist_sq > max_dist_sq)
        max_dist_sq = dist_sq;
    }
    radius_ = glm::sqrt(max_dist_sq);
  }
} // namespace hws
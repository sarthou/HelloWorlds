#ifndef HWS_GRAPHICS_OPENGL_CUPMAP_H
#define HWS_GRAPHICS_OPENGL_CUPMAP_H

#include <array>
#include <glm/matrix.hpp>
#include <string>
#include <vector>

namespace hws {
  class Shader;

  class Cubemap
  {
  public:
    bool init(const std::string& path);

    void draw(Shader& shader);

    unsigned int getId() const { return texture_id_; }

  private:
    unsigned int texture_id_;
    unsigned int vao_;
    unsigned int vbo_;
    glm::mat4 model_;

    static std::array<float, 108> vertices;
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_CUPMAP_H
#ifndef HWS_GRAPHICS_OPENGL_LINESHANDLE_H
#define HWS_GRAPHICS_OPENGL_LINESHANDLE_H

#include <vector>

#include "hello_worlds/Common/Debug/DebugLine.h"
#include "hello_worlds/Graphics/OpenGL/Shader.h"

namespace hws {
  class Actor;

  class LinesHandle
  {
  public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 color;
    Actor* actor;

    LinesHandle(const DebugLine& lines);

    LinesHandle(const std::vector<glm::vec3>& vertices,
                const std::vector<unsigned int>& indices,
                const glm::vec3& color);

    ~LinesHandle();

    LinesHandle(const LinesHandle& other) = delete;

    void disable()
    {
      enabled_ = false;
    }

    bool isEnabled() const { return enabled_; }

    void draw(const Shader& shader) const;

  private:
    unsigned int vbo_; // vertex buffer object
    unsigned int ebo_; //
    unsigned int vao_; // vertex array object

    bool enabled_;

    // initializes all the buffer objects/arrays
    void setupBuffers();
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_LINESHANDLE_H

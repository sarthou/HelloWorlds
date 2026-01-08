#include "hello_worlds/Graphics/OpenGL/LinesHandle.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "glad/glad.h"
#include "hello_worlds/Common/Debug/DebugLine.h"
#include "hello_worlds/Graphics/OpenGL/Shader.h"

namespace hws {

  LinesHandle::LinesHandle(const DebugLine& lines) : vertices(lines.vertices_),
                                                     indices(lines.indices_),
                                                     color(lines.color_),
                                                     actor(lines.linked_actor_),
                                                     enabled_(true)
  {
    setupBuffers();
  }

  LinesHandle::LinesHandle(const std::vector<glm::vec3>& line_vertices,
                           const std::vector<unsigned int>& line_indices,
                           const glm::vec3& line_color) : vertices(line_vertices),
                                                          indices(line_indices),
                                                          color(line_color),
                                                          actor(nullptr), enabled_(true)
  {
    setupBuffers();
  }

  LinesHandle::~LinesHandle()
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
  }

  void LinesHandle::draw(const Shader& shader) const
  {
    shader.setVec4("color", glm::vec4(color, 1.0));

    glBindVertexArray(vao_);
    glDrawElements(GL_LINES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  void LinesHandle::setupBuffers()
  {
    // create buffers/arrays
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (long)(vertices.size() * 3 * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
  }

} // namespace hws
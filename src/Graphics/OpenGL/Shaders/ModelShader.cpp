#include "hello_worlds/Graphics/OpenGL/Shaders/ModelShader.h"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <string>

#include "glad/glad.h" // include glad to get all the required OpenGL headers

namespace hws {

  ModelShader::ModelShader(const std::string& vertex_path,
                           const std::string& fragment_path,
                           const std::string& geometry_path) : Shader(vertex_path,
                                                                      fragment_path,
                                                                      geometry_path)
  {
    setUniformIds();
  }

  ModelShader::ModelShader(const char* v_shader_code,
                           const char* f_shader_code,
                           const char* g_shader_code) : Shader(v_shader_code,
                                                               f_shader_code,
                                                               g_shader_code)
  {
    setUniformIds();
  }

  void ModelShader::setModel(const glm::mat4& value) const
  {
    glUniformMatrix4fv(model_uniform_id_, 1, GL_FALSE, glm::value_ptr(value));
  }

  void ModelShader::setView(const glm::mat4& value) const
  {
    glUniformMatrix4fv(view_uniform_id_, 1, GL_FALSE, glm::value_ptr(value));
  }

  void ModelShader::setProjection(const glm::mat4& value) const
  {
    glUniformMatrix4fv(projection_uniform_id_, 1, GL_FALSE, glm::value_ptr(value));
  }

  void ModelShader::setUniformIds()
  {
    model_uniform_id_ = glGetUniformLocation(id_, "model");
    view_uniform_id_ = glGetUniformLocation(id_, "view");
    projection_uniform_id_ = glGetUniformLocation(id_, "projection");
  }

} // namespace hws
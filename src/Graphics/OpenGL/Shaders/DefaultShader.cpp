#include "hello_worlds/Graphics/OpenGL/Shaders/DefaultShader.h"

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

  DefaultShader::DefaultShader(const std::string& vertex_path,
                               const std::string& fragment_path,
                               const std::string& geometry_path) : ModelShader(vertex_path,
                                                                               fragment_path,
                                                                               geometry_path)
  {
    setUniformIds();
  }

  DefaultShader::DefaultShader(const char* v_shader_code,
                               const char* f_shader_code,
                               const char* g_shader_code) : ModelShader(v_shader_code,
                                                                        f_shader_code,
                                                                        g_shader_code)
  {
    setUniformIds();
  }

  void DefaultShader::setMaterial(MaterialUBO_t* data) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_material_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialUBO_t), data);
  }

  void DefaultShader::setDirLight(DirLightUBO_t* data) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_dir_light_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightUBO_t), data);
  }

  void DefaultShader::setPointLights(PointLightUBO_t* data, size_t nb) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_point_lights_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightUBO_t) * nb, data);
  }

  void DefaultShader::setUseAmbiantShadow(bool value) const
  {
    glUniform1f(use_ambient_shadows_uniform_id_, value);
  }

  void DefaultShader::setUsePointLight(bool value) const
  {
    glUniform1f(use_point_shadows_uniform_id_, value);
  }

  void DefaultShader::setNbPointLight(float value) const
  {
    glUniform1f(nb_point_lights_uniform_id_, value);
  }

  void DefaultShader::setUniformIds()
  {
    glGenBuffers(1, &ubo_material_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_material_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUBO_t), nullptr, GL_DYNAMIC_DRAW);
    // Bind to the same index as the shader: "binding = 1"
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material_);

    glGenBuffers(1, &ubo_dir_light_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_dir_light_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightUBO_t), nullptr, GL_DYNAMIC_DRAW);
    // Bind to the same index as the shader: "binding = 2"
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_dir_light_);

    glGenBuffers(1, &ubo_point_lights_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_point_lights_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightUBO_t) * 20, nullptr, GL_DYNAMIC_DRAW);
    // Bind to the same index as the shader: "binding = 3"
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo_point_lights_);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    use_ambient_shadows_uniform_id_ = glGetUniformLocation(id_, "use_ambient_shadows");
    use_point_shadows_uniform_id_ = glGetUniformLocation(id_, "use_point_shadows");
    nb_point_lights_uniform_id_ = glGetUniformLocation(id_, "nb_point_lights");
  }

} // namespace hws
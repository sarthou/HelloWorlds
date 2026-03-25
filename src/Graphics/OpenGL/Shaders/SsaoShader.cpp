#include "hello_worlds/Graphics/OpenGL/Shaders/SsaoShader.h"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <string>
#include <vector>

#include "glad/glad.h" // include glad to get all the required OpenGL headers

namespace hws {

  SsaoShader::SsaoShader(const std::string& name,
                         const std::string& vertex_path,
                         const std::string& fragment_path,
                         const std::string& geometry_path) : ModelShader(name,
                                                                         vertex_path,
                                                                         fragment_path,
                                                                         geometry_path)
  {
    setUniformIds();
  }

  SsaoShader::SsaoShader(const std::string& name,
                         const char* v_shader_code,
                         const char* f_shader_code,
                         const char* g_shader_code) : ModelShader(name,
                                                                  v_shader_code,
                                                                  f_shader_code,
                                                                  g_shader_code)
  {
    setUniformIds();
  }

  void SsaoShader::setSamples(const std::vector<glm::vec4>& samples) const
  {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_samples_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4) * samples.size(), samples.data());
  }

  void SsaoShader::setNormalTexture(int texture_id) const
  {
    glUniform1i(normal_uniform_id_, texture_id);
  }

  void SsaoShader::setDepthTexture(int texture_id) const
  {
    glUniform1i(depth_uniform_id_, texture_id);
  }

  void SsaoShader::setNoiseTexture(int texture_id) const
  {
    glUniform1i(tex_noise_uniform_id_, texture_id);
  }

  void SsaoShader::setNoiseScale(const glm::vec2& value) const
  {
    glUniform2fv(noise_scale_uniform_id_, 1, &value[0]);
  }

  void SsaoShader::bindBuffers() const
  {
    // Bind to the same index as the shader: "binding = 5"
    glBindBufferBase(GL_UNIFORM_BUFFER, 5, ubo_samples_);
  }

  void SsaoShader::setUniformIds()
  {
    glGenBuffers(1, &ubo_samples_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_samples_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 32, nullptr, GL_DYNAMIC_DRAW);

    bindBuffers();

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    normal_uniform_id_ = glGetUniformLocation(id_, "gNormal");
    depth_uniform_id_ = glGetUniformLocation(id_, "gDepth");
    tex_noise_uniform_id_ = glGetUniformLocation(id_, "texNoise");
    noise_scale_uniform_id_ = glGetUniformLocation(id_, "noiseScale");
  }

} // namespace hws
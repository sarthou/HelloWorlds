
#ifndef HWS_GRAPHICS_OPENGL_SSAOSHADER_H
#define HWS_GRAPHICS_OPENGL_SSAOSHADER_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "hello_worlds/Graphics/OpenGL/Shaders/ModelShader.h"

namespace hws {

  /*struct MaterialUBO_t
  {
    glm::vec4 color;  // Offset 0
    float shininess;  // Offset 16
    float specular;   // Offset 20
    float use_normal; // Offset 24
    float padding;    // Offset 28 (Manual padding to keep it 16-byte aligned)
  };*/

  class SsaoShader : public ModelShader
  {
  public:
    // constructor reads and builds the shader
    SsaoShader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
    SsaoShader(const std::string& name, const char* v_shader_code, const char* f_shader_code, const char* g_shader_code = nullptr);

    void bindBuffers() const;

    void setSamples(const std::vector<glm::vec4>& samples) const;

    void setNormalTexture(int texture_id) const;
    void setDepthTexture(int texture_id) const;
    void setNoiseTexture(int texture_id) const;
    void setNoiseScale(const glm::vec2& value) const;

  private:
    uint32_t ubo_samples_;

    int normal_uniform_id_;
    int depth_uniform_id_;
    int tex_noise_uniform_id_;
    int noise_scale_uniform_id_;

    void setUniformIds();
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_SSAOSHADER_H
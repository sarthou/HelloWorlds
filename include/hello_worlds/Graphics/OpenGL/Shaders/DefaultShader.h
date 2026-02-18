
#ifndef HWS_GRAPHICS_OPENGL_DEFAULTSHADER_H
#define HWS_GRAPHICS_OPENGL_DEFAULTSHADER_H

#include <glm/glm.hpp>
#include <string>

#include "hello_worlds/Graphics/OpenGL/Shaders/ModelShader.h"

namespace hws {

  struct MaterialUBO_t
  {
    glm::vec4 color;  // Offset 0
    float shininess;  // Offset 16
    float specular;   // Offset 20
    float use_normal; // Offset 24
    float padding;    // Offset 28 (Manual padding to keep it 16-byte aligned)
  };

  class DefaultShader : public ModelShader
  {
  public:
    // constructor reads and builds the shader
    DefaultShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
    DefaultShader(const char* v_shader_code, const char* f_shader_code, const char* g_shader_code = nullptr);

    void setMaterial(MaterialUBO_t* data) const;
    void setUseAmbiantShadow(bool value) const;
    void setUsePointLight(bool value) const;

  private:
    uint32_t ubo_material_;
    int use_ambient_shadows_uniform_id_;
    int use_point_shadows_uniform_id_;

    void setUniformIds();
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_DEFAULTSHADER_H
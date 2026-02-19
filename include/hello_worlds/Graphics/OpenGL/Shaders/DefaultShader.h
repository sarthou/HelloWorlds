
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

  struct DirLightUBO_t
  {
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
  };

  struct PointLightUBO_t
  {
    glm::vec4 position;    // 16 bytes
    glm::vec4 ambient;     // 16 bytes
    glm::vec4 diffuse;     // 16 bytes
    glm::vec4 specular;    // 16 bytes
    glm::vec4 attenuation; // 16 bytes
    float far_plane;       // 4 bytes
    float on_off;          // 4 bytes
    float padding1;        // 4 bytes
    float padding2;        // 4 bytes (Total struct = 96 bytes, which is 16 * 6)
  };

  class DefaultShader : public ModelShader
  {
  public:
    // constructor reads and builds the shader
    DefaultShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
    DefaultShader(const char* v_shader_code, const char* f_shader_code, const char* g_shader_code = nullptr);

    void setMaterial(MaterialUBO_t* data) const;

    void setDirLight(DirLightUBO_t* data) const;
    void setPointLights(PointLightUBO_t* data, size_t nb) const;
    void setUseAmbiantShadow(bool value) const;
    void setUsePointLight(bool value) const;
    void setNbPointLight(float value) const;

  private:
    uint32_t ubo_material_;
    uint32_t ubo_dir_light_;
    uint32_t ubo_point_lights_;
    int use_ambient_shadows_uniform_id_;
    int use_point_shadows_uniform_id_;
    int nb_point_lights_uniform_id_;

    void setUniformIds();
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_DEFAULTSHADER_H
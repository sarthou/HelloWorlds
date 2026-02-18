
#ifndef HWS_GRAPHICS_OPENGL_MODELSHADER_H
#define HWS_GRAPHICS_OPENGL_MODELSHADER_H

#include <glm/glm.hpp>
#include <string>

#include "hello_worlds/Graphics/OpenGL/Shaders/Shader.h"

namespace hws {

  class ModelShader : public Shader
  {
  public:
    // constructor reads and builds the shader
    ModelShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
    ModelShader(const char* v_shader_code, const char* f_shader_code, const char* g_shader_code = nullptr);

    void setModel(const glm::mat4& value) const;
    void setView(const glm::mat4& value) const;
    void setProjection(const glm::mat4& value) const;

  private:
    int model_uniform_id_;
    int view_uniform_id_;
    int projection_uniform_id_;

    void setUniformIds();
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_MODELSHADER_H
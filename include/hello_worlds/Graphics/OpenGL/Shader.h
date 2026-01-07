
#ifndef HWS_GRAPHICS_OPENGL_SHADER_H
#define HWS_GRAPHICS_OPENGL_SHADER_H

#include <glm/glm.hpp>
#include <string>

namespace hws {

  class Shader
  {
  public:
    static std::string shaders_directory;

    // the program ID
    unsigned int id_;

    // constructor reads and builds the shader
    Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
    Shader(const char* v_shader_code, const char* f_shader_code, const char* g_shader_code = nullptr);
    // use/activate the shader
    void use() const;
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

  private:
    void compileShader(const char* v_shader_code, const char* f_shader_code, const char* g_shader_code);
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_SHADER_H
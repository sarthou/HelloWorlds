#ifndef HWS_COMMON_AMBIENT_LIGHT_H
#define HWS_COMMON_AMBIENT_LIGHT_H

#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace hws {

  class AmbientLight
  {
  public:
    AmbientLight(const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f),
                 const glm::vec3& color = glm::vec3(1.0),
                 float ambient_strength = 1.0f,
                 float diffuse_strength = 1.0f,
                 float specular_strength = 1.0f);

    AmbientLight(const std::array<float, 3>& lat_long_alt,
                 const glm::vec3& color = glm::vec3(1.0),
                 float ambient_strength = 1.0f,
                 float diffuse_strength = 1.0f,
                 float specular_strength = 1.0f);

    void setDirection(const glm::vec3& direction);
    void setElevationAndAzimuth(float elevation, float azimuth);

    void setColor(const glm::vec3& color);

    void setAmbientStrength(float strength);
    void setDiffuseStrength(float strength);
    void setSpecularStrength(float strength);

    const glm::vec4& getDirection() const { return direction_; }
    const glm::vec4& getAmbient() const { return ambient_; }
    const glm::vec4& getDiffuse() const { return diffuse_; }
    const glm::vec4& getSpecular() const { return specular_; }
    const glm::vec4& getColor() const { return color_; }

  private:
    glm::vec4 direction_;
    glm::vec4 color_;

    float ambient_strength_;
    float diffuse_strength_;
    float specular_strength_;

    float latitude_;
    float longitude_;
    float altitude_;

    glm::vec4 ambient_;
    glm::vec4 diffuse_;
    glm::vec4 specular_;

    void computeAmbient();
    void computeDiffuse();
    void computeSpecular();
  };

} // namespace hws

#endif // HWS_COMMON_AMBIENT_LIGHT_H
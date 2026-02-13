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
    struct GeographicCoords
    {
      double latitude;
      double longitude;
      double altitude;
    };

    // Constructor 1: Simple Directional Use
    AmbientLight(const glm::vec3& direction = glm::vec3(0., 0., 1.0f),
                 const glm::vec3& color = glm::vec3(1.0f),
                 float intensity = 1.0f);

    // Constructor 2: Geographic Based
    AmbientLight(const GeographicCoords& coords,
                 const glm::vec3& color = glm::vec3(1.0f),
                 float intensity = 1.0f);

    // Update logic - Uses stored coordinates
    void updateTime(time_t current_time = 0);

    // Setters
    void setDirection(const glm::vec3& dir);
    void setBaseColor(const glm::vec3& color)
    {
      base_color_ = color;
      updateVisuals();
    }
    void setIntensity(float intensity) { intensity_ = intensity; }
    void setCoords(const GeographicCoords& coords) { geo_coords_ = coords; }

    // Getters for Shaders
    glm::vec4 getDirection() const { return glm::vec4(direction_, 1.0); }
    glm::vec4 getAmbient() const { return glm::vec4(base_color_ * ambient_factor_ * intensity_, 1.0); }
    glm::vec4 getDiffuse() const { return glm::vec4(current_color_ * diffuse_factor_ * intensity_, 1.0); }
    glm::vec4 getSpecular() const { return glm::vec4(current_color_ * specular_factor_ * intensity_, 1.0); }

    glm::vec3 getSkyColor(glm::vec3 base_color) const;

  private:
    void updateVisuals();

    glm::vec3 direction_;
    glm::vec3 base_color_;
    glm::vec3 current_color_;
    float intensity_;

    GeographicCoords geo_coords_{0, 0, 0};

    float ambient_factor_ = 0.2f;
    float diffuse_factor_ = 0.8f;
    float specular_factor_ = 1.0f;
  };

} // namespace hws

#endif // HWS_COMMON_AMBIENT_LIGHT_H
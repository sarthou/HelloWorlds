#include "hello_worlds/Common/Lights/AmbientLight.h"

#include <array>
#include <cmath>
#include <ctime>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>

#include "SolarAzEl/SolarAzEl.h"

namespace hws {

  // Constructor 1: Manual Direction
  AmbientLight::AmbientLight(const glm::vec3& direction,
                             const glm::vec3& color,
                             float intensity) : direction_(glm::normalize(direction)),
                                                base_color_(color),
                                                current_color_(color),
                                                intensity_(intensity)
  {
    updateVisuals();
  }

  // Constructor 2: Geographic
  AmbientLight::AmbientLight(const GeographicCoords_t& coords,
                             const glm::vec3& color,
                             float intensity) : base_color_(color),
                                                current_color_(color),
                                                intensity_(intensity),
                                                geo_coords_(coords)
  {
    updateTime(0); // Initialize direction based on current time
  }

  void AmbientLight::updateTime(time_t time)
  {
    double az = 0.0, el = 0.0;
    solarAzEl(time == 0 ? std::time(nullptr) : time,
              geo_coords_.latitude, geo_coords_.longitude, geo_coords_.altitude,
              &az, &el);

    // Convert Solar Az/El to Direction Vector
    // Assuming Y-Up: Elevation is angle from XZ plane, Azimuth is rotation around Y
    float phi = static_cast<float>(glm::radians(el));
    float theta = static_cast<float>(glm::radians(az));

    direction_.x = std::cos(phi) * std::sin(theta);
    direction_.y = std::sin(phi);
    direction_.z = std::cos(phi) * std::cos(theta);

    updateVisuals();
  }

  void AmbientLight::setDirection(const glm::vec3& dir)
  {
    direction_ = glm::normalize(dir);
    updateVisuals();
  }

  glm::vec3 AmbientLight::getSkyColor(glm::vec3 base_color) const
  {
    const float h = direction_.y;

    const glm::vec3 day_sky = base_color;
    const glm::vec3 sunset_sky = base_color * glm::vec3(1.0f, 0.40f, 0.15f);
    const glm::vec3 night_sky = base_color * glm::vec3(0.02f, 0.05f, 0.12f);

    if(h > 0.0f)
    {
      // Transition Day -> Sunset
      float t = glm::smoothstep(0.0f, 0.4f, h);
      return glm::mix(sunset_sky, day_sky, t);
    }
    else
    {
      // Transition Sunset -> Night
      float t = glm::smoothstep(-0.4f, 0.0f, h);
      return glm::mix(night_sky, sunset_sky, t);
    }
  }

  void AmbientLight::updateVisuals()
  {
    const float h = direction_.y;

    // 1. Tints
    const glm::vec3 sunset_tint = glm::vec3(1.0f, 0.40f, 0.10f); // Deep orange
    const glm::vec3 moon_tint = glm::vec3(0.20f, 0.35f, 0.65f);  // Cool blue

    // 2. Weights
    float day_w = glm::smoothstep(0.0f, 0.35f, h);
    float sunset_w = glm::smoothstep(0.4f, 0.0f, std::abs(h));
    float night_w = glm::smoothstep(0.1f, -0.2f, h);

    // 3. Interpolations
    // We blend linearly across all phases.
    glm::vec3 day_col = base_color_;
    glm::vec3 sunset_col = base_color_ * sunset_tint;
    glm::vec3 night_col = base_color_ * moon_tint;

    // Weight-based blending
    current_color_ = (day_col * day_w) + (sunset_col * sunset_w * (1.0f - day_w)) + (night_col * night_w);

    // Normalize in case of overlap to keep energy consistent
    float total_w = day_w + (sunset_w * (1.0f - day_w)) + night_w;
    if(total_w > 0.001f)
      current_color_ /= total_w;

    // 4. Intensity blending
    const float daylight_intensity = 0.60f;
    const float moonlight_intensity = 0.45f;

    // Intensity smoothly follows the night factor
    diffuse_factor_ = glm::mix(daylight_intensity, moonlight_intensity, night_w);
    specular_factor_ = glm::mix(1.0f, 0.02f, night_w);

    // Dim the color slightly at absolute midnight
    float midnight_dim = glm::smoothstep(0.0f, -0.8f, h) * 0.4f;
    current_color_ *= (1.0f - midnight_dim);
  }

} // namespace hws
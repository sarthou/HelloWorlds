#include "hello_worlds/Common/Lights/AmbientLight.h"

#include <array>
#include <cmath>
#include <ctime>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <time.h>

#include "SolarAzEl/SolarAzEl.h"

namespace hws {

  AmbientLight::AmbientLight(const glm::vec3& direction,
                             const glm::vec3& color,
                             float ambient_strength,
                             float diffuse_strength,
                             float specular_strength) : direction_(direction, 1.0f),
                                                        color_(glm::vec4(color, 1.0f)),
                                                        ambient_strength_(ambient_strength),
                                                        diffuse_strength_(diffuse_strength),
                                                        specular_strength_(specular_strength)
  {
    computeAmbient();
    computeDiffuse();
    computeSpecular();
  }

  AmbientLight::AmbientLight(const std::array<float, 3>& lat_long_alt,
                             const glm::vec3& color,
                             float ambient_strength,
                             float diffuse_strength,
                             float specular_strength) : color_(glm::vec4(color, 1.0f)),
                                                        ambient_strength_(ambient_strength),
                                                        diffuse_strength_(diffuse_strength),
                                                        specular_strength_(specular_strength),
                                                        latitude_(lat_long_alt[0]),
                                                        longitude_(lat_long_alt[1]),
                                                        altitude_(lat_long_alt[2])

  {
    double el = 0., az = 0.;
    solarAzEl(time(nullptr), latitude_, longitude_, altitude_, &az, &el);
    setElevationAndAzimuth((float)glm::radians(el), (float)glm::radians(az));
    computeAmbient();
    computeDiffuse();
    computeSpecular();
  }

  void AmbientLight::setDirection(const glm::vec3& direction)
  {
    direction_ = glm::vec4(direction, 1.0f);
  }

  void AmbientLight::setElevationAndAzimuth(float elevation, float azimuth)
  {
    float x = std::cos(elevation) * std::sin(azimuth);
    float y = -(std::cos(elevation) * std::cos(azimuth));
    float z = std::sin(elevation);
    setDirection(glm::vec3(-y, -x, -z));

    auto night_color = glm::vec4(0.11, 0.15f, 0.31f, 1.f);
    if(z < -1)
    {
      diffuse_strength_ = 0.;
      specular_strength_ = 0.;
      color_ = night_color;
    }
    else if(z < 0.)
    {
      z = z + 1;
      color_ = z * color_ + night_color * (1.f - z);
      diffuse_strength_ = z;
      specular_strength_ = z;
    }
  }

  void AmbientLight::setColor(const glm::vec3& color)
  {
    color_ = glm::vec4(color, 1.0f);
    computeAmbient();
    computeDiffuse();
    computeSpecular();
  }

  void AmbientLight::setAmbientStrength(float strength)
  {
    ambient_strength_ = strength;
    computeAmbient();
  }

  void AmbientLight::setDiffuseStrength(float strength)
  {
    diffuse_strength_ = strength;
    computeDiffuse();
  }

  void AmbientLight::setSpecularStrength(float strength)
  {
    specular_strength_ = strength;
    computeSpecular();
  }

  void AmbientLight::computeAmbient()
  {
    ambient_ = glm::vec4(color_.x * ambient_strength_,
                         color_.y * ambient_strength_,
                         color_.z * ambient_strength_,
                         color_.w * ambient_strength_);
  }

  void AmbientLight::computeDiffuse()
  {
    diffuse_ = glm::vec4(color_.x * diffuse_strength_,
                         color_.y * diffuse_strength_,
                         color_.z * diffuse_strength_,
                         color_.w * diffuse_strength_);
  }

  void AmbientLight::computeSpecular()
  {
    specular_ = glm::vec4(color_.x * specular_strength_,
                          color_.y * specular_strength_,
                          color_.z * specular_strength_,
                          color_.w * specular_strength_);
  }

} // namespace hws
#include "hello_worlds/Common/Lights/PointLights.h"

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace hws {

  PointLights::PointLights() : nb_lights_(0)
  {
    available_.fill(true);
  }

  std::size_t PointLights::addLight(const glm::vec3& position,
                                    const glm::vec3& color,
                                    float ambient_strength,
                                    float diffuse_strength,
                                    float specular_strength,
                                    const glm::vec3& attenuation)
  {
    size_t id = findAvailableId();
    setPosition(id, position);
    colors_[id] = glm::vec4(color, 1.0);
    setAmbientStrength(id, ambient_strength);
    setDiffuseStrength(id, diffuse_strength);
    setSpecularStrength(id, specular_strength);
    setAttenuation(id, attenuation);
    attenuation_distances_[id] = 20;
    on_off_[id] = true;

    return id;
  }

  std::size_t PointLights::addLight(const glm::vec3& position,
                                    const glm::vec3& color,
                                    float ambient_strength,
                                    float diffuse_strength,
                                    float specular_strength,
                                    float attenuation_radius)
  {
    size_t id = findAvailableId();
    setPosition(id, position);
    colors_[id] = glm::vec4(color, 1.0);
    setAmbientStrength(id, ambient_strength);
    setDiffuseStrength(id, diffuse_strength);
    setSpecularStrength(id, specular_strength);
    setAttenuation(id, attenuation_radius);
    attenuation_distances_[id] = attenuation_radius;
    on_off_[id] = true;

    return id;
  }

  bool PointLights::removeLight(std::size_t id)
  {
    if(removeId(id) == false)
      return false;

    ambient_strengths_[id] = 0.f;
    diffuse_strengths_[id] = 0.f;
    specular_strengths_[id] = 0.f;
    computeAmbient(id);
    computeDiffuse(id);
    computeSpecular(id);
    return true;
  }

  bool PointLights::setColor(std::size_t id, const glm::vec3& color)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    colors_[id] = glm::vec4(color, 1.0f);
    computeAmbient(id);
    computeDiffuse(id);
    computeSpecular(id);

    return true;
  }

  bool PointLights::setPosition(std::size_t id, const glm::vec3& position)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    positions_[id] = glm::vec4(position, 1.0f);
    return true;
  }

  bool PointLights::setOn(size_t id, bool on)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    on_off_[id] = on;
    return true;
  }

  bool PointLights::setAttenuation(std::size_t id, const glm::vec3& attenuation)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    attenuations_[id] = glm::vec4(attenuation, 0.0f);
    return true;
  }

  bool PointLights::setAttenuation(std::size_t id, float radius)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    attenuations_[id] = glm::vec4(1.0f, 2.0 / radius, 1.0 / (radius /* * radius*/), 0.0f);
    return true;
  }

  bool PointLights::setAmbientStrength(std::size_t id, float strength)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    ambient_strengths_[id] = strength;
    computeAmbient(id);
    return true;
  }

  bool PointLights::setDiffuseStrength(std::size_t id, float strength)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    diffuse_strengths_[id] = strength;
    computeDiffuse(id);
    return true;
  }

  bool PointLights::setSpecularStrength(std::size_t id, float strength)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    specular_strengths_[id] = strength;
    computeSpecular(id);
    return true;
  }

  bool PointLights::removeId(std::size_t id)
  {
    if((id >= available_.size()) || (available_[id] == true))
      return false;

    available_[id] = true;
    nb_lights_ = 0;
    for(std::size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
      if(available_[i])
        nb_lights_ = i + 1;
    }
    return true;
  }

  std::size_t PointLights::findAvailableId()
  {
    std::size_t id = MAX_POINT_LIGHTS;
    for(std::size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
      if(available_[i])
      {
        id = i;
        break;
      }
    }

    if(id >= nb_lights_)
      nb_lights_ = id + 1;

    available_[id] = false;
    return id;
  }

  void PointLights::computeAmbient(std::size_t id)
  {
    ambients_[id] = glm::vec4(colors_[id].x * ambient_strengths_[id],
                              colors_[id].y * ambient_strengths_[id],
                              colors_[id].z * ambient_strengths_[id],
                              colors_[id].w * ambient_strengths_[id]);
  }

  void PointLights::computeDiffuse(std::size_t id)
  {
    diffuses_[id] = glm::vec4(colors_[id].x * diffuse_strengths_[id],
                              colors_[id].y * diffuse_strengths_[id],
                              colors_[id].z * diffuse_strengths_[id],
                              colors_[id].w * diffuse_strengths_[id]);
  }

  void PointLights::computeSpecular(std::size_t id)
  {
    speculars_[id] = glm::vec4(colors_[id].x * specular_strengths_[id],
                               colors_[id].y * specular_strengths_[id],
                               colors_[id].z * specular_strengths_[id],
                               colors_[id].w * specular_strengths_[id]);
  }

} // namespace hws
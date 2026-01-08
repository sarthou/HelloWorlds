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

    return id;
  }

  void PointLights::removeLight(std::size_t id)
  {
    removeId(id);
    ambient_strengths_[id] = 0.f;
    diffuse_strengths_[id] = 0.f;
    specular_strengths_[id] = 0.f;
    computeAmbient(id);
    computeDiffuse(id);
    computeSpecular(id);
  }

  void PointLights::setColor(std::size_t id, const glm::vec3& color)
  {
    colors_[id] = glm::vec4(color, 1.0f);
    computeAmbient(id);
    computeDiffuse(id);
    computeSpecular(id);
  }

  void PointLights::setPosition(std::size_t id, const glm::vec3& position)
  {
    positions_[id] = glm::vec4(position, 1.0f);
  }

  void PointLights::setAttenuation(std::size_t id, const glm::vec3& attenuation)
  {
    attenuations_[id] = glm::vec4(attenuation, 0.0f);
  }

  void PointLights::setAttenuation(std::size_t id, float radius)
  {
    attenuations_[id] = glm::vec4(1.0f, 2.0 / radius, 1.0 / (radius /* * radius*/), 0.0f);
  }

  void PointLights::setAmbientStrength(std::size_t id, float strength)
  {
    ambient_strengths_[id] = strength;
    computeAmbient(id);
  }

  void PointLights::setDiffuseStrength(std::size_t id, float strength)
  {
    diffuse_strengths_[id] = strength;
    computeDiffuse(id);
  }

  void PointLights::setSpecularStrength(std::size_t id, float strength)
  {
    specular_strengths_[id] = strength;
    computeSpecular(id);
  }

  void PointLights::removeId(std::size_t id)
  {
    available_[id] = true;
    nb_lights_ = 0;
    for(std::size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
      if(available_[i])
        nb_lights_ = i + 1;
    }
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
#ifndef HWS_COMMON_POINT_LIGHTS_H
#define HWS_COMMON_POINT_LIGHTS_H

#include <array>
#include <cstddef>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace hws {

  class PointLights
  {
  public:
    const static std::size_t MAX_POINT_LIGHTS = 20; // MAX_POINTLIGHT

    PointLights();

    std::size_t addLight(const glm::vec3& position,
                         const glm::vec3& color = glm::vec3(1.0),
                         float ambient_strength = 1.0f,
                         float diffuse_strength = 1.0f,
                         float specular_strength = 1.0f,
                         const glm::vec3& attenuation = glm::vec3(1.0f, 0.35f, 0.44f));

    std::size_t addLight(const glm::vec3& position,
                         const glm::vec3& color = glm::vec3(1.0),
                         float ambient_strength = 1.0f,
                         float diffuse_strength = 1.0f,
                         float specular_strength = 1.0f,
                         float attenuation_radius = 10.f);

    void removeLight(std::size_t id);

    void setColor(std::size_t id, const glm::vec3& color);
    void setPosition(std::size_t id, const glm::vec3& position);

    void setAttenuation(std::size_t id, const glm::vec3& attenuation);
    void setAttenuation(std::size_t id, float radius);
    void setAmbientStrength(std::size_t id, float strength);
    void setDiffuseStrength(std::size_t id, float strength);
    void setSpecularStrength(std::size_t id, float strength);

    const std::array<glm::vec4, MAX_POINT_LIGHTS>& getAmbients() const { return ambients_; }
    const std::array<glm::vec4, MAX_POINT_LIGHTS>& getDiffuses() const { return diffuses_; }
    const std::array<glm::vec4, MAX_POINT_LIGHTS>& getSpeculars() const { return speculars_; }
    const std::array<glm::vec4, MAX_POINT_LIGHTS>& getPositions() const { return positions_; }
    const std::array<glm::vec4, MAX_POINT_LIGHTS>& getAttenuations() const { return attenuations_; }
    glm::vec4 getNbLights() const { return glm::vec4((float)nb_lights_); }

    bool isUsed(std::size_t id) const { return !available_[id]; }
    const glm::vec4& getAmbient(std::size_t id) const { return ambients_[id]; }
    const glm::vec4& getDiffuse(std::size_t id) const { return diffuses_[id]; }
    const glm::vec4& getSpecular(std::size_t id) const { return speculars_[id]; }
    const glm::vec4& getPosition(std::size_t id) const { return positions_[id]; }
    const glm::vec4& getAttenuation(std::size_t id) const { return attenuations_[id]; }
    float getAttenuationDistance(std::size_t id) const { return attenuation_distances_[id]; }
    float getNbLightsFloat() const { return (float)nb_lights_; }
    size_t getNbLightsSize() const { return nb_lights_; }

  private:
    std::array<bool, MAX_POINT_LIGHTS> available_;
    std::size_t nb_lights_;

    std::array<glm::vec4, MAX_POINT_LIGHTS> colors_;
    std::array<glm::vec4, MAX_POINT_LIGHTS> positions_;

    std::array<float, MAX_POINT_LIGHTS> ambient_strengths_;
    std::array<float, MAX_POINT_LIGHTS> diffuse_strengths_;
    std::array<float, MAX_POINT_LIGHTS> specular_strengths_;

    std::array<glm::vec4, MAX_POINT_LIGHTS> ambients_;
    std::array<glm::vec4, MAX_POINT_LIGHTS> diffuses_;
    std::array<glm::vec4, MAX_POINT_LIGHTS> speculars_;
    std::array<glm::vec4, MAX_POINT_LIGHTS> attenuations_;
    std::array<float, MAX_POINT_LIGHTS> attenuation_distances_;

    void removeId(std::size_t id);
    std::size_t findAvailableId();

    void computeAmbient(std::size_t id);
    void computeDiffuse(std::size_t id);
    void computeSpecular(std::size_t id);
  };

} // namespace hws

#endif // HWS_COMMON_POINT_LIGHTS_H
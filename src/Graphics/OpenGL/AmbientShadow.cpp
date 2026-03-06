#include "hello_worlds/Graphics/OpenGL/AmbientShadow.h"

#include <algorithm>
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <vector>

#include "glad/glad.h"
#include "hello_worlds/Graphics/OpenGL/Shaders/Shader.h"

namespace hws {

  void AmbientShadow::init(float near_plane, float far_plane)
  {
    near_plane_ = near_plane;
    far_plane_ = far_plane;
    shadow_cascade_levels_ = {far_plane_ / 25.0f, far_plane_ / 10.f, far_plane_ / 4.0f, far_plane_ / 2.0f};
    // shadow_cascade_levels_ = {far_plane_ / 2.0f};

    glGenBuffers(1, &matrices_uniform_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_uniform_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * MAX_CASCADE, nullptr, GL_STATIC_DRAW);
    // Bind to the same index as the shader: "binding = 0"
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrices_uniform_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenFramebuffers(1, &depth_framebuffer_);

    glGenTextures(1, &depth_maps_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_maps_);
    glTexImage3D(
      GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16, (int)resolution_, (int)resolution_, int(shadow_cascade_levels_.size()) + 1,
      0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr std::array<float, 4> bordercolor{1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor.data());

    glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_maps_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
      throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void AmbientShadow::bindFrameBuffer() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer_);
    glViewport(0, 0, (int)resolution_, (int)resolution_);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  void AmbientShadow::setUniforms(const Shader& shader, unsigned int texture_offset) const
  {
    for(size_t i = 0; i < shadow_cascade_levels_.size(); ++i)
      shader.setFloat("cascade_planes_distances[" + std::to_string(i) + "]", shadow_cascade_levels_[i]);

    glActiveTexture(GL_TEXTURE0 + texture_offset);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_maps_);
    shader.setInt("shadow_maps", (int)texture_offset);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
  }

  void AmbientShadow::setLightMatrices()
  {
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_uniform_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, (long)lightspace_matrices_.size() * sizeof(glm::mat4x4), lightspace_matrices_.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void AmbientShadow::computeLightSpaceMatrices(Camera ref_cam, const glm::vec3& light_dir)
  {
    CameraUpdater camera(&ref_cam);
    camera.finalize();
    lightspace_matrices_.clear();
    for(size_t i = 0; i < shadow_cascade_levels_.size() + 1; ++i)
    {
      if(i == 0)
        lightspace_matrices_.push_back(getLightSpaceMatrix(camera, light_dir, near_plane_, shadow_cascade_levels_[i]));
      else if(i < shadow_cascade_levels_.size())
        lightspace_matrices_.push_back(getLightSpaceMatrix(camera, light_dir, shadow_cascade_levels_[i - 1], shadow_cascade_levels_[i]));
      else
      {
        lightspace_matrices_.push_back(getLightSpaceMatrix(camera, light_dir, shadow_cascade_levels_[i - 1], far_plane_));
      }
    }

    camera.setPlanes({near_plane_, far_plane_});
    master_frustum_ = ref_cam.getFrustum();
  }

  glm::mat4 AmbientShadow::getLightSpaceMatrix(CameraUpdater& ref_cam, const glm::vec3& light_dir, float near_plane, float far_plane)
  {
    ref_cam.setPlanes({near_plane, far_plane});
    const auto corners = ref_cam.getCamera()->getFrustumCornersWorldSpace();

    glm::vec3 center = glm::vec3(0, 0, 0);
    for(const auto& v : corners)
      center += glm::vec3(v);
    center /= corners.size();

    glm::vec3 world_up(0.0f, 0.0f, 1.0f);
    const auto light_view = glm::lookAt(center + light_dir, center, world_up);

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();

    for(const auto& v : corners)
    {
      const auto trf = light_view * v;
      min_x = std::min(min_x, trf.x);
      max_x = std::max(max_x, trf.x);
      min_y = std::min(min_y, trf.y);
      max_y = std::max(max_y, trf.y);
      min_z = std::min(min_z, trf.z);
      max_z = std::max(max_z, trf.z);
    }

    const glm::mat4 light_projection = glm::ortho(min_x, max_x, min_y, max_y, min_z - 50, max_z + 50);
    return light_projection * light_view;
  }

} // namespace hws
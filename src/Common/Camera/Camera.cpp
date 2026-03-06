#include "hello_worlds/Common/Camera/Camera.h"

// first
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "hello_worlds/Common/Camera/CameraProjection.h"
#include "hello_worlds/Common/Camera/CameraView.h"
#include "hello_worlds/Common/Camera/ViewAntiAliasing.h"
#include "hello_worlds/Utils/GlmMath.h"

namespace hws {

  void Camera::updateViewMatrix()
  {
    view_matrix_ = glm::lookAt(world_eye_position_,
                               world_eye_position_ + world_eye_front_,
                               world_eye_up_);
    updateFrustum();
  }

  glm::vec3 Camera::getLocalFront() const
  {
    switch(convention_)
    {
    case CameraConvention_e::x_forward_z_up: return {1.0f, 0.0f, 0.0f};
    case CameraConvention_e::z_forward_y_down: return {0.0f, 0.0f, 1.0f};
    default: return {0.0f, 0.0f, -1.0f};
    }
  }

  glm::vec3 Camera::getLocalUp() const
  {
    switch(convention_)
    {
    case CameraConvention_e::x_forward_z_up: return {0.0f, 0.0f, 1.0f};
    case CameraConvention_e::z_forward_y_down: return {0.0f, -1.0f, 0.0f};
    default: return {0.0f, 1.0f, 0.0f};
    }
  }

  void Camera::updateProjectionMatrix()
  {
    switch(projection_type_)
    {
    case hws::CameraProjection_e::perspective:
    {
      auto near = planes_[0];
      auto far = planes_[1];
      const auto aspect_ratio = view_dimensions_[0] / view_dimensions_[1];

      proj_matrix_ = glm::perspective(field_of_view_, aspect_ratio, near, far);

      break;
    }
    case hws::CameraProjection_e::orthographic:
    {
      auto near = planes_[0];
      auto far = planes_[1];
      proj_matrix_ = glm::ortho(-view_dimensions_[0] / 2, view_dimensions_[0] / 2,
                                -view_dimensions_[1] / 2, view_dimensions_[1] / 2,
                                near, far);
      break;
    }
    }
  }

  void Camera::orthogonalize()
  {
    world_eye_front_ = glm::normalize(world_eye_front_);

    glm::vec3 world_up = (convention_ == CameraConvention_e::x_forward_z_up) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    world_eye_right_ = glm::normalize(glm::cross(world_eye_front_, world_up));

    world_eye_up_ = glm::cross(world_eye_right_, world_eye_front_);
  }

  void Camera::updateFrustum()
  {
    glm::mat4 m = proj_matrix_ * view_matrix_;

    // Extraction using Gribb-Hartmann (Column-major indexing)
    // Left
    frustum_.planes[0].normal.x = m[0][3] + m[0][0];
    frustum_.planes[0].normal.y = m[1][3] + m[1][0];
    frustum_.planes[0].normal.z = m[2][3] + m[2][0];
    frustum_.planes[0].distance = m[3][3] + m[3][0];

    // Right
    frustum_.planes[1].normal.x = m[0][3] - m[0][0];
    frustum_.planes[1].normal.y = m[1][3] - m[1][0];
    frustum_.planes[1].normal.z = m[2][3] - m[2][0];
    frustum_.planes[1].distance = m[3][3] - m[3][0];

    // Bottom
    frustum_.planes[2].normal.x = m[0][3] + m[0][1];
    frustum_.planes[2].normal.y = m[1][3] + m[1][1];
    frustum_.planes[2].normal.z = m[2][3] + m[2][1];
    frustum_.planes[2].distance = m[3][3] + m[3][1];

    // Top
    frustum_.planes[3].normal.x = m[0][3] - m[0][1];
    frustum_.planes[3].normal.y = m[1][3] - m[1][1];
    frustum_.planes[3].normal.z = m[2][3] - m[2][1];
    frustum_.planes[3].distance = m[3][3] - m[3][1];

    // Near
    frustum_.planes[4].normal.x = m[0][3] + m[0][2];
    frustum_.planes[4].normal.y = m[1][3] + m[1][2];
    frustum_.planes[4].normal.z = m[2][3] + m[2][2];
    frustum_.planes[4].distance = m[3][3] + m[3][2];

    // Far
    frustum_.planes[5].normal.x = m[0][3] - m[0][2];
    frustum_.planes[5].normal.y = m[1][3] - m[1][2];
    frustum_.planes[5].normal.z = m[2][3] - m[2][2];
    frustum_.planes[5].distance = m[3][3] - m[3][2];

    for(auto& plane : frustum_.planes)
      plane.normalize();
  }

  std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace()
  {
    const auto inv = glm::inverse(proj_matrix_ * view_matrix_);

    std::vector<glm::vec4> frustum_corners;
    for(unsigned int x = 0; x < 2; ++x)
    {
      for(unsigned int y = 0; y < 2; ++y)
      {
        for(unsigned int z = 0; z < 2; ++z)
        {
          const glm::vec4 pt = inv * glm::vec4((2.0f * (float)x) - 1.0f,
                                               (2.0f * (float)y) - 1.0f,
                                               (2.0f * (float)z) - 1.0f,
                                               1.0f);
          frustum_corners.push_back(pt / pt.w);
        }
      }
    }

    return frustum_corners;
  }

} // namespace hws
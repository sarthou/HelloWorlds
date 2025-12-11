#include "hello_worlds/Common/Camera/Camera.h"

// first
#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>

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

  void Camera::updateMatrices()
  {
    updateViewMatrix();
    updateProjectionMatrix();
  }

  void Camera::recomputeDirectionVector()
  {
    glm::vec3 world_up(0.0f, 0.0f, 1.0f);

    glm::vec3 front;
    front.x = (float)(cos(glm::radians(view_angles_.x)) * cos(glm::radians(view_angles_.y)));
    front.y = (float)(sin(glm::radians(view_angles_.x)) * cos(glm::radians(view_angles_.y)));
    front.z = (float)sin(glm::radians(view_angles_.y));

    world_eye_front_ = glm::normalize(front);
    world_eye_right_ = glm::normalize(glm::cross(world_eye_front_, world_up)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    world_eye_up_ = glm::normalize(glm::cross(world_eye_right_, world_eye_front_));
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
          const glm::vec4 pt = inv * glm::vec4(2.0f * (float)x - 1.0f,
                                               2.0f * (float)y - 1.0f,
                                               2.0f * (float)z - 1.0f,
                                               1.0f);
          frustum_corners.push_back(pt / pt.w);
        }
      }
    }

    return frustum_corners;
  }

} // namespace hws
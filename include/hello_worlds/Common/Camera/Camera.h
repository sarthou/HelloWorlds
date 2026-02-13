#ifndef HWS_CAMERA_H
#define HWS_CAMERA_H

#include <array>
#include <cstdint>
#include <functional>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "hello_worlds/Common/Camera/CameraProjection.h"
#include "hello_worlds/Common/Camera/CameraView.h"
#include "hello_worlds/Common/Camera/ViewAntiAliasing.h"

namespace hws {

  class CameraUpdater;

  enum class CameraConvention_e
  {
    x_forward_z_up,    // Common in ROS / Robotics
    z_forward_y_down,  // ROS Optical / Computer Vision
    neg_z_forward_y_up // Standard OpenGL / Game Engines
  };

  struct Frustum_t
  {
    struct Plane_t
    {
      glm::vec3 normal = {0.f, 1.f, 0.f};
      float distance = 0.f;

      void normalize()
      {
        float length = glm::length(normal);
        normal /= length;
        distance /= length;
      }
    };

    Plane_t planes[6]; // 0:Left, 1:Right, 2:Bottom, 3:Top, 4:Near, 5:Far

    bool isSphereVisible(const glm::vec3& center, float radius) const
    {
      for(int i = 0; i < 6; i++)
      {
        // If the sphere is completely behind any one plane, it's culled
        if(glm::dot(planes[i].normal, center) + planes[i].distance < -radius)
        {
          return false;
        }
      }
      return true;
    }
  };

  class Camera
  {
    friend CameraUpdater;

  private:
    hws::CameraView_e view_type_{};
    hws::CameraProjection_e projection_type_{};
    hws::CameraConvention_e convention_ = CameraConvention_e::x_forward_z_up;
    hws::ViewAntiAliasing_e aa_setting_;

    float field_of_view_ = 0.785398; // 45 deg
    glm::vec3 world_eye_position_{0.f};
    glm::vec3 world_eye_front_{1.0f, 0.0f, 0.0f};
    glm::vec3 world_eye_up_ = {0.f, 0., 1.f};

    glm::vec3 world_eye_right_{};

    glm::vec2 planes_{0.1, 100.};
    Frustum_t frustum_;

    bool render_debug_ = true;
    bool render_all_debug_ = false;
    bool render_collision_models_ = false;
    bool render_shadows_ = true;

    std::array<float, 2> view_dimensions_{};
    bool has_changed_size_ = false;

  public:
    Camera() = default;
    Camera(const Camera& other) = default;

    glm::vec3 getPosition() const { return world_eye_position_; }
    glm::vec3 getFrontPose() const { return world_eye_position_ + world_eye_front_; }

    glm::mat4 getViewMatrix() const { return view_matrix_; }
    glm::mat4 getProjectionMatrix() const { return proj_matrix_; }
    ViewAntiAliasing_e getAASetting() const { return aa_setting_; }
    CameraView_e getViewType() const { return view_type_; }
    glm::vec3 getLocalFront() const;
    glm::vec3 getLocalUp() const;
    float getFov() const { return field_of_view_; }

    float getNearPlane() const { return planes_[0]; }
    float getFarPlane() const { return planes_[1]; }

    float getWidth() const { return view_dimensions_.at(0); }
    float getHeight() const { return view_dimensions_.at(1); }
    bool sizeHasChanged() const { return has_changed_size_; }

    bool shouldRenderDebug() const { return render_debug_; }
    bool shouldRenderAllDebug() const { return render_all_debug_; }
    bool shouldRendercollisionModels() const { return render_collision_models_; }
    bool shouldShadows() const { return render_shadows_; }

    std::vector<glm::vec4> getFrustumCornersWorldSpace();
    const Frustum_t& getFrustum() { return frustum_; }

  private:
    glm::mat4 view_matrix_;
    glm::mat4 proj_matrix_;

    void updateViewMatrix();
    void updateProjectionMatrix();
    void orthogonalize();

    void updateFrustum();
  };
} // namespace hws

#endif // HWS_CAMERA_H

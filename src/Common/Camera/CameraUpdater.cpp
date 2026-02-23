#include "hello_worlds/Common/Camera/CameraUpdater.h"

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hello_worlds/Common/Camera/Camera.h"
#include "hello_worlds/Common/Camera/CameraProjection.h"
#include "hello_worlds/Common/Camera/CameraView.h"
#include "hello_worlds/Common/Camera/ViewAntiAliasing.h"
#include "hello_worlds/Utils/GlmMath.h"

namespace hws {

  CameraUpdater::CameraUpdater(Camera* camera)
  {
    if(camera == nullptr)
      camera_ = new Camera;
    else
      camera_ = camera;

    is_internal_camera_ = (camera == nullptr);
  }

  CameraUpdater::~CameraUpdater()
  {
    if(is_internal_camera_)
      delete camera_;
  }

  void CameraUpdater::processUserKeyboardInput(const float delta_time, Key_e key, bool is_down)
  {
    if(camera_ == nullptr)
      return;

    switch(key)
    {
    case Key_e::key_f2: camera_->render_collision_models_ = is_down; break;
    case Key_e::key_f5: camera_->render_all_debug_ = is_down; break;
    case Key_e::key_f3:
    {
      if(is_down == false && key_state_debug_)
        camera_->render_debug_ = !camera_->render_debug_;
      key_state_debug_ = is_down;
      break;
    }
    case Key_e::key_f4:
    {
      if(is_down == false && key_state_shadows_)
        camera_->render_shadows_ = !camera_->render_shadows_;
      key_state_shadows_ = is_down;
      break;
    }
    case Key_e::key_left_control: key_speed_ = is_down; break;
    case Key_e::key_w:
    case Key_e::key_up: key_state_front_ = is_down; break;
    case Key_e::key_s:
    case Key_e::key_down: key_state_back_ = is_down; break;
    case Key_e::key_a:
    case Key_e::key_left: key_state_left_ = is_down; break;
    case Key_e::key_d:
    case Key_e::key_right: key_state_right_ = is_down; break;
    case Key_e::key_space: key_state_up_ = is_down; break;
    case Key_e::key_left_shift: key_state_down_ = is_down; break;
    default: break;
    }

    (void)delta_time;
    (void)key;
    (void)is_down;
  }

  void CameraUpdater::processUserMouseBtnInput(const float delta_time, int btn, bool is_down)
  {
    if(camera_ == nullptr)
      return;

    mouse_btn_states_.set(btn, is_down);

    switch(btn)
    {
    case 1: // right
    case 0: // left
    {
      if(is_down)
      {
        mouse_drag_start_position_ = mouse_current_position_;
      }

      is_dragging_mouse_ = is_down;
      break;
    }
    case 2: // middle
    {
      if(is_down)
      {
        mouse_drag_start_position_ = mouse_current_position_;
      }

      is_dragging_mouse_ = is_down;
      break;
    }
    default:;
    }

    (void)delta_time;
  }

  void CameraUpdater::processUserMouseInput(const float delta_time, const float x, const float y)
  {
    if(camera_ == nullptr)
      return;

    (void)delta_time;
    mouse_current_position_ = {x, y};
    const auto delta = (mouse_current_position_ - mouse_drag_start_position_);
    mouse_drag_start_position_ = mouse_current_position_;

    if(mouse_btn_states_[1] || mouse_btn_states_[0])
    {
      float yaw_angle = -delta.x * mouse_rotation_sensitivity_;
      float pitch_angle = -delta.y * mouse_rotation_sensitivity_;

      glm::vec3 world_up = (camera_->convention_ == CameraConvention_e::x_forward_z_up) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

      glm::quat yaw_quat = glm::angleAxis(glm::radians(yaw_angle), world_up);
      glm::quat pitch_quat = glm::angleAxis(glm::radians(pitch_angle), camera_->world_eye_right_);
      glm::quat combined_rotation = yaw_quat * pitch_quat;

      camera_->world_eye_front_ = glm::normalize(combined_rotation * camera_->world_eye_front_);
      camera_->world_eye_right_ = glm::normalize(glm::cross(camera_->world_eye_front_, world_up));
      camera_->world_eye_up_ = glm::cross(camera_->world_eye_right_, camera_->world_eye_front_);

      camera_->updateViewMatrix();
    }
    else if(mouse_btn_states_[2])
    {
      const float speed = mouse_translate_sensitivity_ * 0.5f;

      camera_->world_eye_position_ -= camera_->world_eye_right_ * (delta.x * speed);
      camera_->world_eye_position_ += camera_->world_eye_up_ * (delta.y * speed);

      camera_->updateViewMatrix();
    }
  }

  void CameraUpdater::processUserMouseScroll(float delta_time, float xoffset, float yoffset)
  {
    if(camera_ == nullptr)
      return;

    (void)delta_time;
    (void)xoffset;
    camera_->world_eye_position_ += camera_->world_eye_front_ * (yoffset * mouse_scroll_sensitivity_);
  }

  void CameraUpdater::update()
  {
    if(camera_ == nullptr)
      return;

    std::chrono::system_clock::time_point current_frame = std::chrono::high_resolution_clock::now();
    delta_time_ = current_frame - last_frame_;
    last_frame_ = current_frame;

    float delta_seconds = std::chrono::duration_cast<std::chrono::microseconds>(delta_time_).count() / 1000000.f;
    float speed = key_speed_ ? 2.9 : 1.4; // m/s
    float dist = speed * delta_seconds;

    if(key_state_front_)
      camera_->world_eye_position_ += camera_->world_eye_front_ * dist;
    if(key_state_back_)
      camera_->world_eye_position_ -= camera_->world_eye_front_ * dist;
    if(key_state_right_)
      camera_->world_eye_position_ += camera_->world_eye_right_ * dist;
    if(key_state_left_)
      camera_->world_eye_position_ -= camera_->world_eye_right_ * dist;
    if(key_state_up_)
      camera_->world_eye_position_ += camera_->world_eye_up_ * dist;
    if(key_state_down_)
      camera_->world_eye_position_ -= camera_->world_eye_up_ * dist;
  }

  void CameraUpdater::setCameraView(const hws::CameraView_e view_type)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->view_type_ = view_type;
  }

  void CameraUpdater::setConvention(hws::CameraConvention_e convention)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->convention_ = convention;
  }

  void CameraUpdater::setProjection(const hws::CameraProjection_e proj)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->projection_type_ = proj;
  }

  void CameraUpdater::setFieldOfView(const float fov)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->field_of_view_ = glm::radians(fov);
  }

  void CameraUpdater::setFieldOfViewRad(float fov)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->field_of_view_ = fov;
  }

  void CameraUpdater::setOutputAA(const hws::ViewAntiAliasing_e aa_setting)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->aa_setting_ = aa_setting;
  }

  void CameraUpdater::setOutputFPS(const std::uint64_t desired_target_fps)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    (void)desired_target_fps; // todo
  }

  void CameraUpdater::setOutputResolution(const std::array<float, 2>& resolution)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->view_dimensions_[0] = resolution[0];
    camera_->view_dimensions_[1] = resolution[1];
    camera_->has_changed_size_ = true;
  }

  void CameraUpdater::setPlanes(const std::array<float, 2>& near_far_planes)
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->planes_[0] = near_far_planes[0];
    camera_->planes_[1] = near_far_planes[1];
    camera_->updateProjectionMatrix();
  }

  void CameraUpdater::setPositionAndLookAt(const std::array<double, 3>& eye_position, const std::array<double, 3>& dst_position)
  {
    assert(camera_ && "CameraUpdater works on null pointer");

    camera_->world_eye_position_ = toGlmV3(eye_position);

    glm::vec3 dst = toGlmV3(dst_position);
    glm::vec3 direction = dst - camera_->world_eye_position_;

    if(glm::length(direction) < 0.0001f)
      camera_->world_eye_front_ = camera_->getLocalFront();
    else
      camera_->world_eye_front_ = glm::normalize(direction);

    camera_->orthogonalize();
    camera_->updateViewMatrix();
  }

  void CameraUpdater::setPositionAndDirection(const std::array<double, 3>& eye_position, const std::array<double, 3>& eye_direction)
  {
    assert(camera_ && "CameraUpdater work on null pointer");

    camera_->world_eye_position_ = toGlmV3(eye_position);
    camera_->world_eye_front_ = glm::normalize(toGlmV3(eye_direction));

    camera_->orthogonalize();
    camera_->updateViewMatrix();
  }

  void CameraUpdater::setPositionAndOrientation(const std::array<double, 3>& eye_position,
                                                const std::array<double, 4>& orientation)
  {
    assert(camera_ && "CameraUpdater works on null pointer");

    camera_->world_eye_position_ = toGlmV3(eye_position);

    glm::quat quat = toGlmQuat(orientation);

    camera_->world_eye_front_ = quat * camera_->getLocalFront();
    camera_->world_eye_up_ = quat * camera_->getLocalUp();

    camera_->updateViewMatrix();
  }

  void CameraUpdater::finalize()
  {
    assert(camera_ && "CameraUpdater work on null pointer");
    camera_->updateViewMatrix();
    camera_->updateProjectionMatrix();
  }

} // namespace hws
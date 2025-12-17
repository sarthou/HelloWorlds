#include "hello_worlds/Graphics/GLFW/Window.h"

#include "hello_worlds/Utils/RosPackage.h"
// should be first

#include <memory>
#include <string>

#include "hello_worlds/Common/Camera/Camera.h"
#include "hello_worlds/Graphics/Common/WindowPlatformData.h"
#include "hello_worlds/Graphics/OpenGL/Renderer.h"
#include "hws_stb_image.h"

// Should be last
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace hws {

  void Window::init()
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
  }

  void Window::release()
  {
    glfwTerminate();
  }

  void Window::pollEvent()
  {
    glfwPollEvents();
  }

  Window::Window(const std::string& name,
                 const std::string& icon_path) : glfw_window_(glfwCreateWindow(640, 480, name.c_str(), nullptr, nullptr))
  {
    // glfwWindowHint(GLFW_SAMPLES, 4);

    if(icon_path.empty() == false)
    {
      GLFWimage icon;
      icon.pixels = stbi_load(icon_path.c_str(), &icon.width, &icon.height, nullptr, 4); // rgba channels
      glfwSetWindowIcon(glfw_window_, 1, &icon);
      stbi_image_free(icon.pixels);
    }

    glfwSetWindowUserPointer(glfw_window_, this);
    glfwSetWindowSizeCallback(glfw_window_, [](GLFWwindow* glfw_window, const int width, const int height) {
      auto* const window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
      window->cam_mutex_.lock();
      window->camera_.setOutputResolution({(float)width, (float)height});
      window->cam_mutex_.unlock();
    });

    glfwSetKeyCallback(glfw_window_, [](GLFWwindow* glfw_window, const int key, const int scancode, const int action, const int mods) {
      (void)scancode;
      (void)mods;

      if(action == GLFW_REPEAT)
        return;

      auto* const window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
      Key_e hws_key = Key_e::key_unknown;
      switch(key)
      {
      case GLFW_KEY_F1: hws_key = Key_e::key_f1; break;
      case GLFW_KEY_F2: hws_key = Key_e::key_f2; break;
      case GLFW_KEY_F3: hws_key = Key_e::key_f3; break;
      case GLFW_KEY_F4: hws_key = Key_e::key_f4; break;
      case GLFW_KEY_F5: hws_key = Key_e::key_f5; break;
      case GLFW_KEY_F6: hws_key = Key_e::key_f6; break;
      case GLFW_KEY_F7: hws_key = Key_e::key_f7; break;
      case GLFW_KEY_F8: hws_key = Key_e::key_f8; break;
      case GLFW_KEY_F9: hws_key = Key_e::key_f9; break;
      case GLFW_KEY_F10: hws_key = Key_e::key_f10; break;
      case GLFW_KEY_F11: hws_key = Key_e::key_f11; break;
      case GLFW_KEY_F12: hws_key = Key_e::key_f12; break;
      case GLFW_KEY_UP: hws_key = Key_e::key_up; break;
      case GLFW_KEY_DOWN: hws_key = Key_e::key_down; break;
      case GLFW_KEY_LEFT: hws_key = Key_e::key_left; break;
      case GLFW_KEY_RIGHT: hws_key = Key_e::key_right; break;
      case GLFW_KEY_ESCAPE: hws_key = Key_e::key_esc; break;
      case GLFW_KEY_SPACE: hws_key = Key_e::key_space; break;
      case GLFW_KEY_LEFT_SHIFT: hws_key = Key_e::key_left_shift; break;
      case GLFW_KEY_MINUS: hws_key = Key_e::key_minus; break;
      case GLFW_KEY_ENTER: hws_key = Key_e::key_enter; break;
      case GLFW_KEY_A: hws_key = Key_e::key_a; break;
      case GLFW_KEY_B: hws_key = Key_e::key_b; break;
      case GLFW_KEY_C: hws_key = Key_e::key_c; break;
      case GLFW_KEY_D: hws_key = Key_e::key_d; break;
      case GLFW_KEY_E: hws_key = Key_e::key_e; break;
      case GLFW_KEY_F: hws_key = Key_e::key_f; break;
      case GLFW_KEY_G: hws_key = Key_e::key_g; break;
      case GLFW_KEY_H: hws_key = Key_e::key_h; break;
      case GLFW_KEY_I: hws_key = Key_e::key_i; break;
      case GLFW_KEY_J: hws_key = Key_e::key_j; break;
      case GLFW_KEY_K: hws_key = Key_e::key_k; break;
      case GLFW_KEY_L: hws_key = Key_e::key_l; break;
      case GLFW_KEY_M: hws_key = Key_e::key_m; break;
      case GLFW_KEY_N: hws_key = Key_e::key_n; break;
      case GLFW_KEY_O: hws_key = Key_e::key_o; break;
      case GLFW_KEY_P: hws_key = Key_e::key_p; break;
      case GLFW_KEY_Q: hws_key = Key_e::key_q; break;
      case GLFW_KEY_R: hws_key = Key_e::key_r; break;
      case GLFW_KEY_S: hws_key = Key_e::key_s; break;
      case GLFW_KEY_T: hws_key = Key_e::key_t; break;
      case GLFW_KEY_U: hws_key = Key_e::key_u; break;
      case GLFW_KEY_V: hws_key = Key_e::key_v; break;
      case GLFW_KEY_W: hws_key = Key_e::key_w; break;
      case GLFW_KEY_X: hws_key = Key_e::key_x; break;
      case GLFW_KEY_Y: hws_key = Key_e::key_y; break;
      case GLFW_KEY_Z: hws_key = Key_e::key_z; break;

      default:
        break;
      }
      window->cam_mutex_.lock();
      window->camera_.processUserKeyboardInput(0.f, hws_key, action == GLFW_PRESS);
      window->cam_mutex_.unlock();
      window->key_callback_(hws_key, action == GLFW_PRESS);
    });

    glfwSetCursorPosCallback(glfw_window_, [](GLFWwindow* glfw_window, const double xpos, const double ypos) {
      auto* const window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
      window->cam_mutex_.lock();
      window->camera_.processUserMouseInput(0.f, static_cast<float>(xpos), static_cast<float>(ypos));
      window->cam_mutex_.unlock();
    });

    glfwSetScrollCallback(glfw_window_, [](GLFWwindow* glfw_window, double xoffset, double yoffset) {
      auto* const window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
      window->cam_mutex_.lock();
      window->camera_.processUserMouseScroll(0.f, static_cast<float>(xoffset), static_cast<float>(yoffset));
      window->cam_mutex_.unlock();
    });

    glfwSetMouseButtonCallback(glfw_window_, [](GLFWwindow* glfw_window, const int button, const int action, int mods) {
      (void)mods;

      if(action == GLFW_REPEAT)
        return;

      auto* const window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
      window->cam_mutex_.lock();
      window->camera_.processUserMouseBtnInput(0.f, button, action == GLFW_PRESS);
      window->cam_mutex_.unlock();
    });
  }

  Window::~Window()
  {
    glfwDestroyWindow(glfw_window_);
  }

  void Window::makeCurrentContext() const
  {
    glfwMakeContextCurrent(glfw_window_);
  }

  WindowPlatformData Window::getPlatformData() const
  {
    return {
      reinterpret_cast<void*>(glfwGetX11Display()),
      reinterpret_cast<void*>(glfwGetX11Window(glfw_window_))};
  }

  void Window::doPollEvents(Renderer& renderer)
  {
    cam_mutex_.lock();
    camera_.update();
    camera_.finalize();
    renderer.setRenderCamera(camera_.getCamera());
    camera_.clearChanges();
    cam_mutex_.unlock();
  }

  bool Window::isCloseRequested() const
  {
    return glfwWindowShouldClose(glfw_window_) != 0;
  }

  void Window::swapBuffer()
  {
    glfwSwapBuffers(glfw_window_);
  }

} // namespace hws
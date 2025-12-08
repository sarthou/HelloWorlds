#ifndef HWS_GRAPHICS_GLFW3_WINDOW_H
#define HWS_GRAPHICS_GLFW3_WINDOW_H

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>

#include "hello_worlds/Common/Camera/CameraUpdater.h"
#include "hello_worlds/Graphics/Common/WindowPlatformData.h"

class GLFWwindow;

namespace hws {
  class Renderer;

  class Window
  {
  public:
    static void init();
    static void release();
    static void pollEvent();

    Window(const std::string& name = "Overworld");
    ~Window();

    void makeCurrentContext() const;

    WindowPlatformData getPlatformData() const;

    void doPollEvents(hws::Renderer& renderer);
    bool isCloseRequested() const;
    void swapBuffer();

    CameraUpdater& getCamera() { return camera_; }
    Camera* getUpdatedCamera() { return camera_.getCamera(); }

    void setKeyCallback(const std::function<void(Key_e, bool)>& callback) { key_callback_ = callback; }

  protected:
    std::uint32_t width_ = 640;
    std::uint32_t height_ = 480;
    bool has_size_changed_ = false;
    GLFWwindow* glfw_window_ = nullptr;
    CameraUpdater camera_;
    std::mutex cam_mutex_;
    std::function<void(Key_e, bool)> key_callback_;

    void defaultKeyCallback(Key_e& key, bool pressed)
    {
      (void)key;
      (void)pressed;
    }
  };
} // namespace hws

#endif // HWS_GRAPHICS_GLFW3_WINDOW_H

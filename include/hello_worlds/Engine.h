#ifndef HWS_ENGINE_H
#define HWS_ENGINE_H

#include <array>
#include <string>
#include <thread>

#include "hello_worlds/Common/Camera/Camera.h"
#include "hello_worlds/Common/Camera/CameraView.h"
#include "hello_worlds/Graphics/OpenGL/Renderer.h"

// should be after glad
#include "hello_worlds/Graphics/GLFW/Window.h"
#include "hello_worlds/Physics/PhysX/World.h"
namespace hws {
  using WorldEngine = hws::physx::World;
}

// Should be last
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace hws {

  class Engine
  {
  public:
    Engine(Window* window, bool simulate = true) : window_(window),
                                                   run_(true),
                                                   simulate_(simulate)
    {}

    WorldEngine world;

    void setVizualizerCamera(const std::array<double, 3>& position, const std::array<double, 3>& target);

    void stop();
    bool isRunning() { return run_; }

    void runDetached(float max_fps = 30);
    void run(float max_fps = 30);

    void setKeyCallback(const std::function<void(Key_e, bool)>& callback) { window_->setKeyCallback(callback); }

    WorldEngine* getWorld() { return &world; }

  private:
    Renderer renderer_;
    Window* window_;
    std::atomic<bool> run_;
    bool simulate_;
    bool detached_ = false;
    std::thread engin_thread_;
  };

} // namespace hws

#endif // HWS_ENGINE_H
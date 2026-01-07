#ifndef HWS_ENGINE_H
#define HWS_ENGINE_H

#include <array>
#include <string>

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
    Engine(const std::string& name, Window* window) : world("TODO path"), // TODO: remove ref to overworld
                                                      name_(name), window_(window), run_(true)
    {}

    WorldEngine world;

    void initView(float max_fps = 30, float screen_width = 640, float screen_height = 480);

    void setVizualizerCamera(const std::array<double, 3>& position, const std::array<double, 3>& target);

    void finalise(double fps = 1. / 60.);

    void stop();

    void run(bool simulate = false);

    void setKeyCallback(const std::function<void(Key_e, bool)>& callback) { window_->setKeyCallback(callback); }

    WorldEngine* getWorld() { return &world; }

  private:
    Renderer renderer_; // TODO find way to expose addSkyBox function
    std::string name_;
    Window* window_;
    std::atomic<bool> run_;
  };

} // namespace hws

#endif // HWS_ENGINE_H
#ifndef HWS_ENGINE_H
#define HWS_ENGINE_H

#include "hello_worlds/Compat/ROS.h"
// should be first

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
    Engine(const std::string& name, Window* window) : world(compat::hws_ros::getShareDirectory("overworld")),
                                                      name_(name), window_(window), run_(true)
    {}

    WorldEngine world;

    void initView(float screen_width = 640, float screen_height = 480)
    {
      auto& cam = window_->getCamera();
      cam.setFieldOfView(60.f);
      cam.setOutputAA(hws::ViewAntiAliasing_e::msaa_x4);
      cam.setOutputResolution({screen_width, screen_height});
      cam.setPositionAndLookAt({6, 6, 1.7}, {0, 0, 0});
      cam.setPlanes({0.1, 60.});
      cam.finalize();

      renderer_.initialize(*window_);
    }

    void setVizualizerCamera(const std::array<double, 3>& position, const std::array<double, 3>& target)
    {
      window_->getCamera().setPositionAndLookAt(position, target);
    }

    void finalise(double fps = 1. / 60.)
    {
      renderer_.attachWorld(&world);
      world.setTimeStep(fps);
    }

    void stop()
    {
      run_ = false;
    }

    void run(bool simulate = false)
    {
      while(!window_->isCloseRequested() && run_)
      {
        window_->doPollEvents(renderer_);
        if(simulate)
          world.stepSimulation();
        renderer_.commit();
        window_->swapBuffer();
      }
    }

    void setKeyCallback(const std::function<void(Key_e, bool)>& callback) { window_->setKeyCallback(callback); }

    WorldEngine* getWorld() { return &world; }

  private:
    Renderer renderer_;
    std::string name_;
    Window* window_;
    std::atomic<bool> run_;
  };

} // namespace hws

#endif // HWS_ENGINE_H
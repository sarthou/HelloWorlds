#include "hello_worlds/Engine.h"

#include <array>
#include <string>

namespace hws {

  void Engine::initView(float max_fps, float screen_width, float screen_height)
  {
    auto& cam = window_->getCamera();
    cam.setFieldOfView(60.f);
    cam.setOutputAA(hws::ViewAntiAliasing_e::msaa_x4);
    cam.setOutputResolution({screen_width, screen_height});
    cam.setPositionAndLookAt({6, 6, 1.7}, {0, 0, 0});
    cam.setPlanes({0.1, 60.});
    cam.finalize();

    renderer_.initialize(*window_, max_fps);
  }

  void Engine::setVizualizerCamera(const std::array<double, 3>& position, const std::array<double, 3>& target)
  {
    window_->getCamera().setPositionAndLookAt(position, target);
  }

  void Engine::finalise(double fps)
  {
    renderer_.attachWorld(&world);
    world.setTimeStep(fps);
  }

  void Engine::stop()
  {
    run_ = false;
  }

  void Engine::run(bool simulate)
  {
    while(!window_->isCloseRequested() && run_)
    {
      window_->doPollEvents(renderer_);
      if(simulate)
        world.stepSimulation();
      while(renderer_.shouldRender() == false)
        usleep(500);
      renderer_.commit();
      window_->swapBuffer();
    }
  }

} // namespace hws
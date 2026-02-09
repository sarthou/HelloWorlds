#include "hello_worlds/Engine.h"

#include <array>
#include <string>

namespace hws {

  void Engine::setVizualizerCamera(const std::array<double, 3>& position, const std::array<double, 3>& target)
  {
    window_->getCamera().setPositionAndLookAt(position, target);
  }

  void Engine::stop()
  {
    run_ = false;
    if(detached_)
      engin_thread_.join();
  }

  void Engine::runDetached(float max_fps)
  {
    detached_ = true;
    engin_thread_ = std::thread(&Engine::run, this, max_fps);
  }

  void Engine::run(float max_fps)
  {
    renderer_.initialize(*window_, max_fps);
    renderer_.attachWorld(&world);

    while(!window_->isCloseRequested() && run_)
    {
      window_->doPollEvents(renderer_);
      if(simulate_)
        world.stepSimulation();
      while(renderer_.shouldRender() == false)
        usleep(500);
      renderer_.commit();
      window_->swapBuffer();
    }

    run_ = false;
  }

} // namespace hws
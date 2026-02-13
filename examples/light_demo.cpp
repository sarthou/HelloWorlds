#include <thread>

#include "hello_worlds/Engine.h"
#include "hello_worlds/Utils/AssetResolver.h"
#include "test_config.hpp"

void worldThread(const std::string& world_name, hws::Window* window)
{
  hws::Engine engine(window, false);

  engine.world.setBackgroundColor(135. / 255., 206. / 255., 235. / 255.);
  engine.world.addSkyBox(hwd::tests::get_asset_dir() + "SkyBox/");

  engine.world.setTimeStep(1. / 60.);

  engine.runDetached();

  /* Engine is running, just make things spawn*/
  engine.world.setLocatedAmbientLight({48.f, -2.f, 0.f},
                                      {1.0f, 0.98f, 0.9f},
                                      1.5);

  auto ligth_1 = engine.world.addPointLight({5.f, 7.0f, 2.9f},
                                            {1.0f, 1.0f, 1.0f},
                                            0.4, 0.5, 1.0,
                                            6.f);

  auto ligth_2 = engine.world.addPointLight({8.5f, 7.0f, 2.9f},
                                            {1.0f, 1.0f, 1.0f},
                                            0.5, 0.6, 1.0,
                                            6.0f);

  auto ligth_3 = engine.world.addPointLight({5.f, 12.0f, 2.9f},
                                            {1.0f, 1.0f, 1.0f},
                                            0.4, 0.5, 1.0,
                                            6.f);

  auto ligth_4 = engine.world.addPointLight({2.5f, -2.5f, 2.5f},
                                            {1.0f, 1.0f, 1.0f},
                                            0.4, 0.5, 1.0,
                                            6.f);

  engine.world.loadUrdf(hwd::tests::get_asset_dir() + "Adream/adream.urdf", {0., 0., 0.}, {0., 0., 0.});

  /* Main loop to do stuff */

  auto current_time = time(nullptr);
  while(engine.isRunning())
  {
    usleep(10000);
    current_time += 10;
    engine.world.setAmbientLightTime(current_time);

    engine.world.setPointLightOn(ligth_4, engine.world.isAmbientNight());
  }

  std::cout << "exiting" << std::endl;
}

int main()
{
  hws::Renderer::init();

  hws::Window window1("light_demo", 960, 720);

  hws::AssetResolver::instance().registerPackage("hello_worlds", hwd::tests::get_package_dir());

  std::thread world1(worldThread, "light_demo", &window1);

  hws::Window::run();

  std::cout << "window stop to run" << std::endl;

  world1.join();

  std::cout << "join ok" << std::endl;

  hws::Renderer::release();

  std::cout << "this is the end" << std::endl;
  return 0;
}
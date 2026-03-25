#include <chrono>
#include <thread>

#include "hello_worlds/Engine.h"
#include "hello_worlds/Utils/AssetResolver.h"
#include "test_config.hpp"

void worldThread(hws::Window* window)
{
  hws::Engine engine(window, false);

  engine.world.setBackgroundColor(135. / 255., 206. / 255., 235. / 255.);
  engine.world.addSkyBox(hwd::tests::get_asset_dir() + "SkyBox/");

  engine.world.setTimeStep(1. / 60.);

  engine.runDetached(60);

  /* Engine is running, just make things spawn*/
  engine.world.setLocatedAmbientLight({48.f, -2.f, 0.f},
                                      {1.0f, 0.98f, 0.9f},
                                      1.5);

  auto ligth_1 = engine.world.addPointLight({0.f, 0.0f, 2.9f},
                                            {1.0f, 1.0f, 1.0f},
                                            0.4, 0.5, 1.0,
                                            6.f);

  hws::urdf::Geometry_t geometry;
  geometry.type = hws::urdf::GeometryType_e::geometry_mesh;
  geometry.file_name = hwd::tests::get_asset_dir() + "Adream/appartment.fbx"; // bench_LOD0.obj";
  geometry.material.name_ = "plop1";
  geometry.material.diffuse_color_ = {0.0, 0.0, 0.8};

  hws::urdf::Geometry_t geometry2 = geometry;
  geometry2.material.name_ = "plop2";
  geometry2.material.diffuse_color_ = {0.8, 0.8, 0.8};

  hws::urdf::Geometry_t geometry3 = geometry2;
  geometry3.origin_translation = {0., 0., 1.};

  engine.world.loadUrdf(hwd::tests::get_asset_dir() + "Adream/adream.urdf", {0., 0., 0.}, {0., 0., 0.});
  // engine.world.createVisualActor({geometry}, {0., 0., 0.});
  // engine.world.createVisualActor({geometry}, {-2., 0., 0.});
  // engine.world.createVisualActor({geometry2, geometry3}, {2., 0., 0.});

  /* Main loop to do stuff */

  // auto current_time = time(nullptr);
  time_t current_time = 1000000;
  engine.world.setAmbientLightTime(current_time);
  while(engine.isRunning())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    // current_time += 10;
  }
}

int main()
{
  hws::Renderer::init();

  hws::Window window1("light_demo", 960, 720);

  hws::AssetResolver::instance().registerPackage("hello_worlds", hwd::tests::get_package_dir());

  std::thread world1(worldThread, &window1);

  hws::Window::run();

  world1.join();

  hws::Renderer::release();

  return 0;
}
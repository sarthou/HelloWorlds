#include <chrono>
#include <cmath>
#include <random>
#include <thread>
#include <vector>

#include "hello_worlds/Engine.h"
#include "hello_worlds/Utils/AssetResolver.h"
#include "test_config.hpp"

void worldThread(hws::Window* window)
{
  hws::Engine engine(window, false);

  engine.world.setBackgroundColor(0.1f, 0.1f, 0.15f); // Darker for better contrast
  engine.world.addSkyBox(hwd::tests::get_asset_dir() + "SkyBox/");

  // Use sub-stepping to ensure the LERP has room to breathe
  engine.world.setTimeStep(1. / 60.);
  engine.world.setSubstepping(4);

  engine.world.setGravity({0., 0., -9.81});
  engine.runDetached(60);

  // 1. Create Ground
  hws::urdf::Geometry_t ground_geo;
  ground_geo.type = hws::urdf::GeometryType_e::geometry_box;
  ground_geo.scale = {20, 20, 0.2};
  ground_geo.material.diffuse_color_ = {0.5, 0.5, 0.5};
  engine.world.createActor(ground_geo, {ground_geo}, {0., 0., -0.1});

  // 2. Create a Grid of Simulated Cubes
  hws::urdf::Geometry_t cube_geo;
  cube_geo.type = hws::urdf::GeometryType_e::geometry_box;
  cube_geo.scale = {0.4, 0.4, 0.4};
  cube_geo.material.diffuse_color_ = {0.8, 0.2, 0.2}; // Red cubes

  std::vector<int> cubes;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> dist(-0.05, 0.05);
  std::uniform_real_distribution<double> rot_dist(-0.1, 0.1);

  for(int x = -2; x <= 2; ++x)
  {
    for(int y = -2; y <= 2; ++y)
    {
      for(int z = -2; z <= 2; ++z)
      {
        // Add small random offsets to X and Y
        double px = x * 0.6; //+ dist(generator);
        double py = y * 0.6; //+ dist(generator);
        double pz = 4.0 + z * 0.6;

        // auto c = engine.world.createActor(cube_geo, {cube_geo}, {px, py, pz}, {rot_dist(generator), rot_dist(generator), rot_dist(generator)});
        auto c = engine.world.createActor(cube_geo, {cube_geo}, {px, py, pz}, {0., 0., 0., 1.});

        engine.world.setMass(c, -1, 0.5);
        engine.world.setStaticFriction(c, -1, 0.6);   // Harder to start sliding
        engine.world.setDynamicFriction(c, -1, 0.45); // Smooth once moving
        engine.world.setRestitution(c, -1, 0.3);      // Slight bounce, but settles
        engine.world.setActorMode(c, hws::ActorMode_e::simulated_mode);
        cubes.push_back(c);
      }
    }
  }

  // 3. Create the Kinematic Sweeper
  hws::urdf::Geometry_t sweeper_geo;
  sweeper_geo.type = hws::urdf::GeometryType_e::geometry_box;
  sweeper_geo.scale = {1.0, 1.0, 1.0};
  sweeper_geo.material.diffuse_color_ = {0.2, 0.8, 0.2}; // Green sweeper
  auto sweeper = engine.world.createActor(sweeper_geo, {sweeper_geo}, {-5.0, 0.0, 0.5});
  engine.world.setActorMode(sweeper, hws::ActorMode_e::kinematic_mode);

  // Lighting
  engine.world.setLocatedAmbientLight({10.f, 10.f, 10.f}, {1.0f, 1.0f, 1.0f}, 1.0);
  engine.world.addPointLight({0.f, 0.f, 5.f}, {1.f, 1.f, 1.f}, 0.1, 0.1, 1.0, 20.f);

  /* Main loop */
  int frame_count = 0;

  time_t current_time = 1000000;
  engine.world.setAmbientLightTime(current_time);

  int direction = 1.;
  double x_pos = -5.0;
  double y_pos = -1.;

  while(engine.isRunning())
  {
    // Every 2 seconds, reset the cubes to the air to keep the demo going
    if(frame_count % 120 == 0 && frame_count > 0)
    {
      // Optional: Reset logic here
    }

    // --- SWEEPER MOVEMENT LOGIC ---
    // We update the sweeper's position.
    // To test your "Data Gap" logic, we only send a new position every 5 frames.
    if(frame_count % 5 == 0)
    {
      x_pos += direction * 0.05f * 3.; // Moves from -5 to +5
      y_pos += 0.003f;
      if(x_pos > 5.0)
      {
        direction *= -1;
      }
      else if(x_pos < -5.0)
      {
        direction *= -1;
      }

      std::array<double, 3> pos = {x_pos, y_pos, 0.5};
      std::array<double, 4> ori = {0, 0, 0, 1}; // Identity quat

      engine.world.setBasePositionAndOrientation(sweeper, pos, ori);
    }
    else
    {
      // This triggers your 'pending_data_gap_ = true' logic!
      engine.world.setBasePositionAndOrientation(sweeper);
    }

    engine.world.stepSimulation();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    frame_count++;
  }
}

int main()
{
  hws::Renderer::init();

  hws::Window window1("physics_demo", 960, 720);

  hws::AssetResolver::instance().registerPackage("hello_worlds", hwd::tests::get_package_dir());

  std::thread world1(worldThread, &window1);

  hws::Window::run();

  world1.join();

  hws::Renderer::release();

  return 0;
}
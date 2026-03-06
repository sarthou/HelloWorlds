#include "hello_worlds/Common/Models/ModelManager.h"

#include <gtest/gtest.h>

#include "test_config.hpp"

TEST(ModelManagerTester, CheckPathExists)
{
  std::string path = hwd::tests::get_asset_dir();
  EXPECT_FALSE(path.empty());
  EXPECT_TRUE(path.find("assets") != std::string::npos);
}

TEST(ModelManagerTester, Load)
{
  std::string path = hwd::tests::get_asset_dir();
  std::string window_stl_path = path + "BuildingParts/window.stl";
  std::string window_obj_path = path + "BuildingParts/window.obj";

  auto& manager = hws::ModelManager::get();

  EXPECT_FALSE(manager.isModelLoaded(window_stl_path));
  EXPECT_FALSE(manager.isModelLoaded(window_obj_path));
  EXPECT_EQ(manager.getLoadedModelCount(), 0);

  hws::Model const* model = manager.loadAndGet(window_stl_path);
  // EXPECT_NE(model.id_, 0);
  EXPECT_FALSE(model->source_path_.empty());
  EXPECT_EQ(model->meshes_.size(), 1);
  EXPECT_EQ(model->scale_[0], 1.f);
  EXPECT_EQ(model->scale_[1], 1.f);
  EXPECT_EQ(model->scale_[2], 1.f);

  const hws::Mesh& mesh = model->meshes_[0];
  // EXPECT_NE(mesh.id_, 0);
  EXPECT_TRUE(mesh.name_.empty());
  EXPECT_NE(mesh.vertices_.size(), 0);
  EXPECT_EQ(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map with stl
  EXPECT_EQ(mesh.vertices_[0].uv_.y, 0.5);
  EXPECT_EQ(mesh.indices_.size() % 3, 0);
  EXPECT_EQ(mesh.indices_.size(), mesh.vertices_.size());

  EXPECT_TRUE(mesh.material_.name_.empty());
  EXPECT_TRUE(mesh.material_.diffuse_texture_.empty());
  EXPECT_TRUE(mesh.material_.specular_texture_.empty());
  EXPECT_TRUE(mesh.material_.normal_texture_.empty());
  EXPECT_EQ(mesh.material_.shininess_, -1);
  EXPECT_EQ(mesh.material_.diffuse_color_.r_, -1.);
  EXPECT_EQ(mesh.material_.diffuse_color_.g_, -1.);
  EXPECT_EQ(mesh.material_.diffuse_color_.b_, -1.);
  EXPECT_EQ(mesh.material_.diffuse_color_.a_, 0.);
  EXPECT_EQ(mesh.material_.specular_color_.r_, -1.);
  EXPECT_EQ(mesh.material_.specular_color_.g_, -1.);
  EXPECT_EQ(mesh.material_.specular_color_.b_, -1.);
  EXPECT_EQ(mesh.material_.specular_color_.a_, 0.);

  EXPECT_TRUE(manager.isModelLoaded(window_stl_path));
  EXPECT_FALSE(manager.isModelLoaded(window_obj_path));
  EXPECT_EQ(manager.getLoadedModelCount(), 1);

  hws::Model const* mode2 = manager.loadAndGet(window_obj_path);

  EXPECT_TRUE(manager.isModelLoaded(window_stl_path));
  EXPECT_TRUE(manager.isModelLoaded(window_obj_path));
  EXPECT_EQ(manager.getLoadedModelCount(), 2);

  hws::Model const* mode2bis = manager.loadAndGet(window_obj_path);

  EXPECT_TRUE(manager.isModelLoaded(window_stl_path));
  EXPECT_TRUE(manager.isModelLoaded(window_obj_path));
  EXPECT_EQ(manager.getLoadedModelCount(), 2);
}

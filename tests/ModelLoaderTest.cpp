#include "hello_worlds/Common/Models/Loaders/ModelLoader.h"

#include <gtest/gtest.h>

#include "test_config.hpp"

TEST(ModelLoaderTester, CheckPathExists)
{
  std::string path = hwd::tests::get_asset_dir();
  EXPECT_FALSE(path.empty());
  EXPECT_TRUE(path.find("assets") != std::string::npos);
}

TEST(ModelLoaderTester, LoadWindowStl)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "BuildingParts/window.stl";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 1);

  const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
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
}

TEST(ModelLoaderTester, LoadWindowObj)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "BuildingParts/window.obj";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 1);

  const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
  // EXPECT_NE(mesh.id_, 0);
  EXPECT_EQ(mesh.name_, "window_h10_Cube");
  EXPECT_NE(mesh.vertices_.size(), 0);
  EXPECT_EQ(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
  EXPECT_EQ(mesh.vertices_[0].uv_.y, 0.5);
  EXPECT_NE(mesh.indices_.size(), 0);
  EXPECT_EQ(mesh.indices_.size() % 3, 0);

  EXPECT_EQ(mesh.material_.name_, "M_glass_frame_black_glossy");
  EXPECT_TRUE(mesh.material_.diffuse_texture_.empty());
  EXPECT_TRUE(mesh.material_.specular_texture_.empty());
  EXPECT_TRUE(mesh.material_.normal_texture_.empty());
  EXPECT_EQ(mesh.material_.shininess_, 900);
  EXPECT_NEAR(mesh.material_.diffuse_color_.r_, 0.04896, 0.001);
  EXPECT_NEAR(mesh.material_.diffuse_color_.g_, 0.04896, 0.001);
  EXPECT_NEAR(mesh.material_.diffuse_color_.b_, 0.04896, 0.001);
  EXPECT_EQ(mesh.material_.diffuse_color_.a_, 1.);
  EXPECT_EQ(mesh.material_.specular_color_.r_, 1.);
  EXPECT_EQ(mesh.material_.specular_color_.g_, 1.);
  EXPECT_EQ(mesh.material_.specular_color_.b_, 1.);
  EXPECT_EQ(mesh.material_.specular_color_.a_, 1.);
}

TEST(ModelLoaderTester, LoadEveHeadObj)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "Eve/eve_head.obj";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 1);

  const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
  // EXPECT_NE(mesh.id_, 0);
  EXPECT_EQ(mesh.name_, "Head.001");
  EXPECT_NE(mesh.vertices_.size(), 0);
  EXPECT_NE(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
  EXPECT_NE(mesh.vertices_[0].uv_.y, 0.5);
  EXPECT_NE(mesh.vertices_[0].uv_.x, 0.); // error value
  EXPECT_NE(mesh.vertices_[0].uv_.y, 0.);
  EXPECT_NE(mesh.indices_.size(), 0);
  EXPECT_EQ(mesh.indices_.size() % 3, 0);

  EXPECT_EQ(mesh.material_.name_, "eveHead");
  EXPECT_EQ(mesh.material_.diffuse_texture_, hwd::tests::get_asset_dir() + "Eve/eve_head.png");
  EXPECT_TRUE(mesh.material_.specular_texture_.empty());
  EXPECT_TRUE(mesh.material_.normal_texture_.empty());
  EXPECT_EQ(mesh.material_.shininess_, 250);
  EXPECT_NEAR(mesh.material_.diffuse_color_.r_, 0.8, 0.001);
  EXPECT_NEAR(mesh.material_.diffuse_color_.g_, 0.8, 0.001);
  EXPECT_NEAR(mesh.material_.diffuse_color_.b_, 0.8, 0.001);
  EXPECT_EQ(mesh.material_.diffuse_color_.a_, 1.);
  EXPECT_EQ(mesh.material_.specular_color_.r_, 0.5);
  EXPECT_EQ(mesh.material_.specular_color_.g_, 0.5);
  EXPECT_EQ(mesh.material_.specular_color_.b_, 0.5);
  EXPECT_EQ(mesh.material_.specular_color_.a_, 1.);
}

TEST(ModelLoaderTester, LoadFrameDae)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "Frame/frame_z.dae";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 1);

  const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
  // EXPECT_NE(mesh.id_, 0);
  EXPECT_EQ(mesh.name_, "Cube_002-mesh");
  EXPECT_NE(mesh.vertices_.size(), 0);
  EXPECT_NE(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
  EXPECT_NE(mesh.vertices_[0].uv_.y, 0.5);
  EXPECT_NE(mesh.vertices_[0].uv_.x, 0.); // error value
  EXPECT_NE(mesh.vertices_[0].uv_.y, 0.);
  EXPECT_NE(mesh.indices_.size(), 0);
  EXPECT_EQ(mesh.indices_.size() % 3, 0);

  EXPECT_EQ(mesh.material_.name_, "MaterialFrame_001-material");
  EXPECT_EQ(mesh.material_.diffuse_texture_, hwd::tests::get_asset_dir() + "Frame/frame.png");
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
}

TEST(ModelLoaderTester, LoadFramesObj)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "Frame/frames.obj";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 2);

  {
    const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
    // EXPECT_NE(mesh.id_, 0);
    EXPECT_EQ(mesh.name_, "Cube1");
    EXPECT_NE(mesh.vertices_.size(), 0);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.5);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.); // error value
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.);
    EXPECT_NE(mesh.indices_.size(), 0);
    EXPECT_EQ(mesh.indices_.size() % 3, 0);

    EXPECT_EQ(mesh.material_.name_, "Material1");
    EXPECT_EQ(mesh.material_.diffuse_texture_, hwd::tests::get_asset_dir() + "Frame/frame.png");
    EXPECT_TRUE(mesh.material_.specular_texture_.empty());
    EXPECT_TRUE(mesh.material_.normal_texture_.empty());
    EXPECT_EQ(mesh.material_.shininess_, 640);
    EXPECT_NEAR(mesh.material_.diffuse_color_.r_, 0.6, 0.001); // default diffuse value
    EXPECT_NEAR(mesh.material_.diffuse_color_.g_, 0.6, 0.001); // default diffuse value
    EXPECT_NEAR(mesh.material_.diffuse_color_.b_, 0.6, 0.001); // default diffuse value
    EXPECT_EQ(mesh.material_.diffuse_color_.a_, 0.);
    EXPECT_NEAR(mesh.material_.specular_color_.r_, 0.470588, 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.g_, 0.470588, 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.b_, 0.470588, 0.001);
    EXPECT_EQ(mesh.material_.specular_color_.a_, 1.);
  }

  {
    const hws::RawMesh_t& mesh = model_ptr->meshes_.at(1);
    // EXPECT_NE(mesh.id_, 0);
    EXPECT_EQ(mesh.name_, "Cube2");
    EXPECT_NE(mesh.vertices_.size(), 0);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.5);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.); // error value
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.);
    EXPECT_NE(mesh.indices_.size(), 0);
    EXPECT_EQ(mesh.indices_.size() % 3, 0);

    EXPECT_EQ(mesh.material_.name_, "Material2");
    EXPECT_TRUE(mesh.material_.diffuse_texture_.empty());
    EXPECT_TRUE(mesh.material_.specular_texture_.empty());
    EXPECT_TRUE(mesh.material_.normal_texture_.empty());
    EXPECT_EQ(mesh.material_.shininess_, 250);
    EXPECT_NEAR(mesh.material_.diffuse_color_.r_, 0.800055, 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.g_, 0.006040, 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.b_, 0.013360, 0.001);
    EXPECT_EQ(mesh.material_.diffuse_color_.a_, 1.);
    EXPECT_NEAR(mesh.material_.specular_color_.r_, 0.5, 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.g_, 0.5, 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.b_, 0.5, 0.001);
    EXPECT_EQ(mesh.material_.specular_color_.a_, 1.);
  }
}

TEST(ModelLoaderTester, LoadFramesDae)
{
  std::string path = hwd::tests::get_asset_dir();
  path += "Frame/frames.dae";

  hws::ModelLoader loader;
  std::unique_ptr<hws::RawModel_t> model_ptr = loader.load(path);
  EXPECT_NE(model_ptr, nullptr);
  // EXPECT_NE(model_ptr->id_, 0);
  EXPECT_FALSE(model_ptr->source_path_.empty());
  EXPECT_EQ(model_ptr->meshes_.size(), 2);

  {
    const hws::RawMesh_t& mesh = model_ptr->meshes_.at(0);
    // EXPECT_NE(mesh.id_, 0);
    EXPECT_EQ(mesh.name_, "Cube_002-mesh");
    EXPECT_NE(mesh.vertices_.size(), 0);
    EXPECT_EQ(mesh.vertices_[0].uv_.x, 0.5);
    EXPECT_EQ(mesh.vertices_[0].uv_.y, 0.5);
    EXPECT_NE(mesh.indices_.size(), 0);
    EXPECT_EQ(mesh.indices_.size() % 3, 0);

    EXPECT_EQ(mesh.material_.name_, "Material2-material");
    EXPECT_TRUE(mesh.material_.diffuse_texture_.empty());
    EXPECT_TRUE(mesh.material_.specular_texture_.empty());
    EXPECT_TRUE(mesh.material_.normal_texture_.empty());
    EXPECT_EQ(mesh.material_.shininess_, -1);
    EXPECT_NEAR(mesh.material_.diffuse_color_.r_, 0.800055, 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.g_, 0.006040, 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.b_, 0.013360, 0.001);
    EXPECT_EQ(mesh.material_.diffuse_color_.a_, 1.);
    EXPECT_NEAR(mesh.material_.specular_color_.r_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.g_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.b_, -1., 0.001);
    EXPECT_EQ(mesh.material_.specular_color_.a_, 0.);
  }

  {
    const hws::RawMesh_t& mesh = model_ptr->meshes_.at(1);
    // EXPECT_NE(mesh.id_, 0);
    EXPECT_EQ(mesh.name_, "Cube_001-mesh");
    EXPECT_NE(mesh.vertices_.size(), 0);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.5); // default value as no uv map
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.5);
    EXPECT_NE(mesh.vertices_[0].uv_.x, 0.); // error value
    EXPECT_NE(mesh.vertices_[0].uv_.y, 0.);
    EXPECT_NE(mesh.indices_.size(), 0);
    EXPECT_EQ(mesh.indices_.size() % 3, 0);

    EXPECT_EQ(mesh.material_.name_, "Material1-material");
    EXPECT_EQ(mesh.material_.diffuse_texture_, hwd::tests::get_asset_dir() + "Frame/frame.png");
    EXPECT_TRUE(mesh.material_.specular_texture_.empty());
    EXPECT_TRUE(mesh.material_.normal_texture_.empty());
    EXPECT_EQ(mesh.material_.shininess_, -1);
    EXPECT_NEAR(mesh.material_.diffuse_color_.r_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.g_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.diffuse_color_.b_, -1., 0.001);
    EXPECT_EQ(mesh.material_.diffuse_color_.a_, 0.);
    EXPECT_NEAR(mesh.material_.specular_color_.r_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.g_, -1., 0.001);
    EXPECT_NEAR(mesh.material_.specular_color_.b_, -1., 0.001);
    EXPECT_EQ(mesh.material_.specular_color_.a_, 0.);
  }
}
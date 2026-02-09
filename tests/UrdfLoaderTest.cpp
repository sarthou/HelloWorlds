#include "hello_worlds/Common/Urdf/UrdfLoader.h"

#include <gtest/gtest.h>

#include "hello_worlds/Utils/AssetResolver.h"
#include "test_config.hpp"

TEST(UrdfLoaderTester, LoadUrdfFrameFile)
{
  hws::AssetResolver::instance().registerPackage("hello_worlds", hwd::tests::get_package_dir());

  std::string urdf_path = hwd::tests::get_asset_dir() + "Frame/frame.urdf";

  hws::UrdfLoader loader;

  hws::urdf::Urdf_t urdf = loader.read(urdf_path);

  EXPECT_EQ(urdf.name, "frame");
  EXPECT_EQ(urdf.root_link, "frame_y");
  EXPECT_EQ(urdf.joints.size(), 1);
  EXPECT_EQ(urdf.links.size(), 2);
  EXPECT_EQ(urdf.tree.size(), 1);
  EXPECT_EQ(urdf.link_to_parent_joint.size(), 1);

  auto frame_y_link_it = urdf.links.find("frame_y");

  EXPECT_NE(frame_y_link_it, urdf.links.end());
  EXPECT_EQ(frame_y_link_it->second.name, "frame_y");

  EXPECT_EQ(frame_y_link_it->second.inertia.mass, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.origin.x, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.origin.y, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.origin.z, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.ix.x, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.ix.y, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.ix.z, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.iy.y, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.iy.z, 0);
  EXPECT_EQ(frame_y_link_it->second.inertia.iz.z, 0);

  EXPECT_EQ(frame_y_link_it->second.visual.type, hws::urdf::GeometryType_e::geometry_mesh);
  EXPECT_EQ(frame_y_link_it->second.visual.file_name, "package://hello_worlds/assets/Frame/frame.obj");
  EXPECT_EQ(frame_y_link_it->second.visual.origin_translation.x, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.origin_translation.y, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.origin_translation.z, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.origin_rotation.x, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.origin_rotation.y, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.origin_rotation.z, 0);
  EXPECT_EQ(frame_y_link_it->second.visual.scale.x, 1.);
  EXPECT_EQ(frame_y_link_it->second.visual.scale.y, 1.);
  EXPECT_EQ(frame_y_link_it->second.visual.scale.z, 1.);

  EXPECT_EQ(frame_y_link_it->second.visual.material.name_, "");
  EXPECT_EQ(frame_y_link_it->second.visual.material.diffuse_texture_, "");
  EXPECT_EQ(frame_y_link_it->second.visual.material.specular_texture_, "");
  EXPECT_EQ(frame_y_link_it->second.visual.material.normal_texture_, "");
  EXPECT_EQ(frame_y_link_it->second.visual.material.shininess_, -1);

  EXPECT_EQ(frame_y_link_it->second.collisions.size(), 1.);
  auto& frame_y_link_collision = frame_y_link_it->second.collisions.front();

  EXPECT_EQ(frame_y_link_collision.type, hws::urdf::GeometryType_e::geometry_mesh);
  EXPECT_EQ(frame_y_link_collision.file_name, "package://hello_worlds/assets/Frame/frame.obj");
  EXPECT_EQ(frame_y_link_collision.origin_translation.x, 0);
  EXPECT_EQ(frame_y_link_collision.origin_translation.y, 0);
  EXPECT_EQ(frame_y_link_collision.origin_translation.z, 0);
  EXPECT_EQ(frame_y_link_collision.origin_rotation.x, 0);
  EXPECT_EQ(frame_y_link_collision.origin_rotation.y, 0);
  EXPECT_EQ(frame_y_link_collision.origin_rotation.z, 0);
  EXPECT_EQ(frame_y_link_collision.scale.x, 1.);
  EXPECT_EQ(frame_y_link_collision.scale.y, 1.);
  EXPECT_EQ(frame_y_link_collision.scale.z, 1.);

  auto frame_z_link_it = urdf.links.find("frame_z");
  EXPECT_EQ(frame_z_link_it->second.name, "frame_z");

  EXPECT_EQ(frame_z_link_it->second.inertia.mass, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.origin.x, 1.);
  EXPECT_EQ(frame_z_link_it->second.inertia.origin.y, 1.);
  EXPECT_EQ(frame_z_link_it->second.inertia.origin.z, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.ix.x, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.ix.y, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.ix.z, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.iy.y, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.iy.z, 0);
  EXPECT_EQ(frame_z_link_it->second.inertia.iz.z, 0);

  EXPECT_EQ(frame_z_link_it->second.visual.type, hws::urdf::GeometryType_e::geometry_mesh);
  EXPECT_EQ(frame_z_link_it->second.visual.file_name, "package://hello_worlds/assets/Frame/frame_z.dae");
  EXPECT_EQ(frame_z_link_it->second.visual.origin_translation.x, 1.);
  EXPECT_EQ(frame_z_link_it->second.visual.origin_translation.y, 1.);
  EXPECT_EQ(frame_z_link_it->second.visual.origin_translation.z, 0);
  EXPECT_EQ(frame_z_link_it->second.visual.origin_rotation.x, 0);
  EXPECT_EQ(frame_z_link_it->second.visual.origin_rotation.y, 0);
  EXPECT_EQ(frame_z_link_it->second.visual.origin_rotation.z, 0);
  EXPECT_EQ(frame_z_link_it->second.visual.scale.x, 1.);
  EXPECT_EQ(frame_z_link_it->second.visual.scale.y, 1.);
  EXPECT_EQ(frame_z_link_it->second.visual.scale.z, 1.);

  EXPECT_EQ(frame_z_link_it->second.visual.material.name_, "farme_material");
  std::string true_texture_dir = hwd::tests::get_asset_dir() + "Frame/frame.png";
  EXPECT_EQ(frame_z_link_it->second.visual.material.diffuse_texture_, true_texture_dir); // Textures are already resolved
  EXPECT_EQ(frame_z_link_it->second.visual.material.specular_texture_, "");
  EXPECT_EQ(frame_z_link_it->second.visual.material.normal_texture_, "");
  EXPECT_EQ(frame_z_link_it->second.visual.material.shininess_, -1);

  EXPECT_EQ(frame_z_link_it->second.collisions.size(), 1.);
  auto& frame_z_link_collision = frame_z_link_it->second.collisions.front();

  EXPECT_EQ(frame_z_link_collision.type, hws::urdf::GeometryType_e::geometry_mesh);
  EXPECT_EQ(frame_z_link_collision.file_name, "package://hello_worlds/assets/Frame/frame.obj");
  EXPECT_EQ(frame_z_link_collision.origin_translation.x, 1.);
  EXPECT_EQ(frame_z_link_collision.origin_translation.y, 1.);
  EXPECT_EQ(frame_z_link_collision.origin_translation.z, 0);
  EXPECT_EQ(frame_z_link_collision.origin_rotation.x, 0);
  EXPECT_EQ(frame_z_link_collision.origin_rotation.y, 0);
  EXPECT_EQ(frame_z_link_collision.origin_rotation.z, 0);
  EXPECT_EQ(frame_z_link_collision.scale.x, 1.);
  EXPECT_EQ(frame_z_link_collision.scale.y, 1.);
  EXPECT_EQ(frame_z_link_collision.scale.z, 1.);

  EXPECT_NE(frame_z_link_it, urdf.links.end());

  auto frame_link_joint_it = urdf.joints.find("frame_link");

  EXPECT_NE(frame_link_joint_it, urdf.joints.end());
  EXPECT_EQ(frame_link_joint_it->second.name, "frame_link");
  EXPECT_EQ(frame_link_joint_it->second.type, hws::urdf::JointType_e::joint_fixed);
  EXPECT_EQ(frame_link_joint_it->second.parent_link, "frame_y");
  EXPECT_EQ(frame_link_joint_it->second.child_link, "frame_z");
  EXPECT_EQ(frame_link_joint_it->second.limited, false);

  EXPECT_EQ(frame_link_joint_it->second.origin_translation.x, -1.);
  EXPECT_EQ(frame_link_joint_it->second.origin_translation.y, 1.);
  EXPECT_EQ(frame_link_joint_it->second.origin_translation.z, 0);
  EXPECT_EQ(frame_link_joint_it->second.origin_rotation.x, 0);
  EXPECT_EQ(frame_link_joint_it->second.origin_rotation.y, 0);
  EXPECT_EQ(frame_link_joint_it->second.origin_rotation.z, 0);

  EXPECT_EQ(urdf.link_to_parent_joint.find("frame_y"), urdf.link_to_parent_joint.end());
  EXPECT_NE(urdf.link_to_parent_joint.find("frame_z"), urdf.link_to_parent_joint.end());
  EXPECT_EQ(urdf.link_to_parent_joint.at("frame_z"), "frame_link");

  EXPECT_EQ(urdf.tree.find("frame_z"), urdf.tree.end());
  EXPECT_NE(urdf.tree.find("frame_y"), urdf.tree.end());
  EXPECT_EQ(urdf.tree.at("frame_y").size(), 1);
}
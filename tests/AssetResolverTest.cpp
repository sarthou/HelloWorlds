#include "hello_worlds/Utils/AssetResolver.h"

#include <gtest/gtest.h>

#include "test_config.hpp"

TEST(AssetResolverTester, CheckPathExists)
{
  std::string true_path = hwd::tests::get_asset_dir() + "eve.urdf";

  std::string mesh_path = hws::AssetResolver::instance().getFullPath("package://hello_worlds/assets/eve.urdf");
  if(const char* ament_ptr = std::getenv("AMENT_PREFIX_PATH"))
  {
    EXPECT_NE(mesh_path, "");
    EXPECT_NE(mesh_path, true_path); // In ROS2 the path should be found but should not point to the source asset path
  }
  else
    EXPECT_EQ(mesh_path, "");

  hws::AssetResolver::instance().registerPackage("hello_worlds", hwd::tests::get_package_dir());

  mesh_path = hws::AssetResolver::instance().getFullPath("package://hello_worlds/assets/eve.urdf");
  EXPECT_EQ(mesh_path, true_path);

  hws::AssetResolver::instance().clear();

  mesh_path = hws::AssetResolver::instance().getFullPath("package://hello_worlds/assets/eve.urdf");
  if(const char* ament_ptr = std::getenv("AMENT_PREFIX_PATH"))
  {
    EXPECT_NE(mesh_path, "");
    EXPECT_NE(mesh_path, true_path); // In ROS2 the path should be found but should not point to the source asset path
  }
  else
    EXPECT_EQ(mesh_path, "");

  hws::AssetResolver::instance().addSearchDirectory(hwd::tests::get_package_dir());

  mesh_path = hws::AssetResolver::instance().getFullPath("package://assets/eve.urdf");
  EXPECT_EQ(mesh_path, true_path);

  mesh_path = hws::AssetResolver::instance().getFullPath("package://Eve/eve_head.png");
  true_path = hwd::tests::get_asset_dir() + "Eve/eve_head.png";
  EXPECT_EQ(mesh_path, true_path);
}

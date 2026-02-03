#include "hello_worlds/Common/Camera/CameraUpdater.h"

#include <gtest/gtest.h>

#include "hello_worlds/Common/Camera/Camera.h"

TEST(CameraUpdaterTester, cameraCreation)
{
  hws::CameraUpdater updater;
  updater.setCameraView(hws::CameraView_e::regular_view);

  hws::Camera camera;
  hws::CameraUpdater updater_2(&camera);
  updater_2.setCameraView(hws::CameraView_e::segmented_view);

  EXPECT_NE(&camera, updater.getCamera());
  EXPECT_EQ(&camera, updater_2.getCamera());

  EXPECT_EQ(updater.getCamera()->getViewType(), hws::CameraView_e::regular_view);
  EXPECT_EQ(updater_2.getCamera()->getViewType(), hws::CameraView_e::segmented_view);
}

TEST(CameraUpdaterTester, setPositionAndLookAt)
{
  hws::Camera camera;
  hws::CameraUpdater updater(&camera);

  EXPECT_EQ(&camera, updater.getCamera());

  updater.setCameraView(hws::CameraView_e::regular_view);
  updater.setProjection(hws::CameraProjection_e::perspective);
  updater.setFieldOfView(45.);
  updater.setOutputResolution({640., 480.});
  updater.setPlanes({1., 10.});

  updater.finalize();

  EXPECT_EQ(camera.getNearPlane(), 1.);
  EXPECT_EQ(camera.getFarPlane(), 10.);

  EXPECT_EQ(camera.getWidth(), 640.);
  EXPECT_EQ(camera.getHeight(), 480.);
  EXPECT_TRUE(camera.sizeHasChanged());
  EXPECT_NEAR(camera.getFov(), (45. * M_PI / 180.), 0.001);
  EXPECT_EQ(camera.getViewType(), hws::CameraView_e::regular_view);

  updater.setPositionAndLookAt({0., 0., 1.}, {2., 0., 1.});

  auto pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  auto front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 1., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  auto corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[0].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.001);

  EXPECT_NEAR(corners[1].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[1].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.001);

  EXPECT_NEAR(corners[2].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[2].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.001);

  EXPECT_NEAR(corners[3].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[3].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.001);

  updater.setPositionAndLookAt({0., 0., 1.}, {0., 1.5, 1.});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 1., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, -0.55228, 0.001);
  EXPECT_NEAR(corners[0].y, 1.00000, 0.001);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.001);

  EXPECT_NEAR(corners[1].x, -5.5228, 0.001);
  EXPECT_NEAR(corners[1].y, 10.0000, 0.001);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.001);

  EXPECT_NEAR(corners[2].x, -0.55228, 0.001);
  EXPECT_NEAR(corners[2].y, 1.00000, 0.001);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.001);

  EXPECT_NEAR(corners[3].x, -5.5228, 0.001);
  EXPECT_NEAR(corners[3].y, 10.0000, 0.001);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.001);

  updater.setPositionAndLookAt({0., 0., 1.}, {0., 0., 2.});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 2., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, -0.41421, 0.001);
  EXPECT_NEAR(corners[0].y, -0.5522, 0.001);
  EXPECT_NEAR(corners[0].z, 2.00000, 0.001);

  EXPECT_NEAR(corners[1].x, -4.1421, 0.001);
  EXPECT_NEAR(corners[1].y, -5.5228, 0.001);
  EXPECT_NEAR(corners[1].z, 11.0000, 0.001);

  EXPECT_NEAR(corners[2].x, 0.41421, 0.001);
  EXPECT_NEAR(corners[2].y, -0.5522, 0.001);
  EXPECT_NEAR(corners[2].z, 2.0000, 0.001);

  EXPECT_NEAR(corners[3].x, 4.1421, 0.001);
  EXPECT_NEAR(corners[3].y, -5.5228, 0.001);
  EXPECT_NEAR(corners[3].z, 11.0000, 0.001);
}

TEST(CameraUpdaterTester, setPositionAndDirection)
{
  hws::Camera camera;
  hws::CameraUpdater updater(&camera);

  updater.setCameraView(hws::CameraView_e::regular_view);
  updater.setProjection(hws::CameraProjection_e::perspective);
  updater.setFieldOfView(45.);
  updater.setOutputResolution({640., 480.});
  updater.setPlanes({1., 10.});

  updater.finalize();

  updater.setPositionAndDirection({0., 0., 1.}, {1., 0., 0.});

  auto pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  auto front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 1., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  auto corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[0].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.001);

  EXPECT_NEAR(corners[1].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[1].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.001);

  EXPECT_NEAR(corners[2].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[2].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.001);

  EXPECT_NEAR(corners[3].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[3].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.001);

  updater.setPositionAndDirection({0., 0., 1.}, {0., 1., 0.});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 1., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, -0.55228, 0.001);
  EXPECT_NEAR(corners[0].y, 1.00000, 0.001);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.001);

  EXPECT_NEAR(corners[1].x, -5.5228, 0.001);
  EXPECT_NEAR(corners[1].y, 10.0000, 0.001);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.001);

  EXPECT_NEAR(corners[2].x, -0.55228, 0.001);
  EXPECT_NEAR(corners[2].y, 1.00000, 0.001);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.001);

  EXPECT_NEAR(corners[3].x, -5.5228, 0.001);
  EXPECT_NEAR(corners[3].y, 10.0000, 0.001);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.001);

  updater.setPositionAndDirection({0., 0., 1.}, {0., 0., 1.});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 2., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, -0.41421, 0.001);
  EXPECT_NEAR(corners[0].y, -0.5522, 0.001);
  EXPECT_NEAR(corners[0].z, 2.00000, 0.001);

  EXPECT_NEAR(corners[1].x, -4.1421, 0.001);
  EXPECT_NEAR(corners[1].y, -5.5228, 0.001);
  EXPECT_NEAR(corners[1].z, 11.0000, 0.001);

  EXPECT_NEAR(corners[2].x, 0.41421, 0.001);
  EXPECT_NEAR(corners[2].y, -0.5522, 0.001);
  EXPECT_NEAR(corners[2].z, 2.0000, 0.001);

  EXPECT_NEAR(corners[3].x, 4.1421, 0.001);
  EXPECT_NEAR(corners[3].y, -5.5228, 0.001);
  EXPECT_NEAR(corners[3].z, 11.0000, 0.001);
}

TEST(CameraUpdaterTester, setPositionAndOrientation)
{
  hws::Camera camera;
  hws::CameraUpdater updater(&camera);

  updater.setCameraView(hws::CameraView_e::regular_view);
  updater.setProjection(hws::CameraProjection_e::perspective);
  updater.setFieldOfView(45.);
  updater.setOutputResolution({640., 480.});
  updater.setPlanes({1., 10.});

  updater.finalize();

  updater.setPositionAndOrientation({0., 0., 1.}, {0., 0., 0., 1.});

  auto pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  auto front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 1., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  auto corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[0].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.001);

  EXPECT_NEAR(corners[1].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[1].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.001);

  EXPECT_NEAR(corners[2].x, 1.00000, 0.001);
  EXPECT_NEAR(corners[2].y, 0.55228, 0.001);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.001);

  EXPECT_NEAR(corners[3].x, 10.0000, 0.001);
  EXPECT_NEAR(corners[3].y, 5.5228, 0.001);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.001);

  updater.setPositionAndOrientation({0., 0., 1.}, {0., 0., 0.707, 0.707});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 1., 0.001);
  EXPECT_NEAR(front_pose.z, 1., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, -0.55228, 0.005);
  EXPECT_NEAR(corners[0].y, 1.00000, 0.005);
  EXPECT_NEAR(corners[0].z, 0.58578, 0.005);

  EXPECT_NEAR(corners[1].x, -5.5228, 0.005);
  EXPECT_NEAR(corners[1].y, 10.0000, 0.005);
  EXPECT_NEAR(corners[1].z, -3.14214, 0.005);

  EXPECT_NEAR(corners[2].x, -0.55228, 0.005);
  EXPECT_NEAR(corners[2].y, 1.00000, 0.005);
  EXPECT_NEAR(corners[2].z, 1.41421, 0.005);

  EXPECT_NEAR(corners[3].x, -5.5228, 0.005);
  EXPECT_NEAR(corners[3].y, 10.0000, 0.005);
  EXPECT_NEAR(corners[3].z, 5.1421, 0.005);

  updater.setPositionAndOrientation({0., 0., 1.}, {0., -0.707, 0., 0.707});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 2., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, 0.41421, 0.005);
  EXPECT_NEAR(corners[0].y, 0.5522, 0.005);
  EXPECT_NEAR(corners[0].z, 2.00000, 0.005);

  EXPECT_NEAR(corners[1].x, 4.1421, 0.005);
  EXPECT_NEAR(corners[1].y, 5.5228, 0.005);
  EXPECT_NEAR(corners[1].z, 11.0000, 0.005);

  EXPECT_NEAR(corners[2].x, -0.41421, 0.005);
  EXPECT_NEAR(corners[2].y, 0.5522, 0.005);
  EXPECT_NEAR(corners[2].z, 2.0000, 0.005);

  EXPECT_NEAR(corners[3].x, -4.1421, 0.005);
  EXPECT_NEAR(corners[3].y, 5.5228, 0.005);
  EXPECT_NEAR(corners[3].z, 11.0000, 0.005);

  // turn around z axis, not possible with other methods
  updater.setPositionAndOrientation({0., 0., 1.}, {0.5, 0.5, 0.5, -0.5});

  pose = camera.getPosition();
  EXPECT_EQ(pose.x, 0.);
  EXPECT_EQ(pose.y, 0.);
  EXPECT_EQ(pose.z, 1.);

  front_pose = camera.getFrontPose(); // normalized pose
  EXPECT_NEAR(front_pose.x, 0., 0.001);
  EXPECT_NEAR(front_pose.y, 0., 0.001);
  EXPECT_NEAR(front_pose.z, 2., 0.001);

  corners = camera.getFrustumCornersWorldSpace();
  EXPECT_EQ(corners.size(), 8);
  EXPECT_NEAR(corners[0].x, 0.5522, 0.005);
  EXPECT_NEAR(corners[0].y, -0.41421, 0.005);
  EXPECT_NEAR(corners[0].z, 2.00000, 0.005);

  EXPECT_NEAR(corners[1].x, 5.5228, 0.005);
  EXPECT_NEAR(corners[1].y, -4.1421, 0.005);
  EXPECT_NEAR(corners[1].z, 11.0000, 0.005);

  EXPECT_NEAR(corners[2].x, 0.5522, 0.005);
  EXPECT_NEAR(corners[2].y, 0.41421, 0.005);
  EXPECT_NEAR(corners[2].z, 2.0000, 0.005);

  EXPECT_NEAR(corners[3].x, 5.5228, 0.005);
  EXPECT_NEAR(corners[3].y, 4.1421, 0.005);
  EXPECT_NEAR(corners[3].z, 11.0000, 0.005);
}
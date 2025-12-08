# HelloWorlds

HelloWorld is a simple PhysX-based 3D world engine with visualization specialized for robotic applications.

Its main features include:

 - Creation of multiple independent worlds in parallel
 - Abstraction of PhysX complexity
 - Image rendering (and thus camera emulation)

The engine comes with mesh loaders (OBJ, STL, and Collada) and a URDF loader for out-of-the-box use.

Regarding 3D visualization, a dedicated OpenGL-based solution has been implemented. It supports ambient, specular, and normal textures, manages sunlight and point lights, provides skybox functionality, and allows for debug elements (lines and user-oriented text).

In sum: If you like PyBullet, you will prefer HelloWorld.

## Build

### 1. Standalone Build (No ROS)

To build and install the library without ROS, use standard CMake commands:

``` bash
mkdir build && cd build
cmake ..
make
sudo make install # Installs to system directories (e.g., /usr/local/lib, /usr/local/include)
```

### 2. ROS 1 Build

Use a catkin workspace:

``` bash
cd ~/catkin_ws/
source /opt/ros/noetic/setup.bash # Example for Noetic
catkin build hello_worlds
```

### 3. ROS 2 Build

Use an ament workspace:

``` bash
cd ~/ros2_ws/
source /opt/ros/humble/setup.bash # Example for Humble
colcon build --packages-select hello_worlds
```
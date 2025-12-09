# HelloWorlds

HelloWorlds is a simple PhysX-based 3D world engine with visualization specialized for robotic applications.

Its main features include:

 - Creation of multiple independent worlds in parallel
 - Abstraction of PhysX complexity
 - Image rendering (and thus camera emulation)

The engine comes with mesh loaders (OBJ, STL, and Collada) and a URDF loader for out-of-the-box use.

Regarding 3D visualization, a dedicated OpenGL-based solution has been implemented. It supports ambient, specular, and normal textures, manages sunlight and point lights, provides skybox functionality, and allows for debug elements (lines and user-oriented text).

In sum: If you like PyBullet, you will prefer HelloWorlds.

## Build PhysX

HelloWorlds uses PhysX. Here bellow we explain how to build it.

Once you have cloned HelloWorlds, go into the `External` folder qnd clone PhysX.

```bash
cd hello_worlds/External
git clone --depth 1 --branch 106.5-physx-5.5.1 https://github.com/NVIDIA-Omniverse/PhysX.git
```

```
hello_worlds/
├── CMakeLists.txt
├── src/
└── External/
    └── PhysX/
        ├── physx/
        │   ├── generate_projects.sh
        │   ├── compiler/
        │   └── include/
        ├── blast/
        └── flow/
```

We then have to generate the projects depending on your platform. Here bellow you assume a GCC use but you can freely use Clang.

```bash
cd PhysX/physx
./generate_projects.sh
```

```
hello_worlds/
├── CMakeLists.txt
├── src/
└── External/
    └── PhysX/
        ├── physx/
        │   ├── generate_projects.sh
        │   ├── compiler/
        │   │   ├── linux-gcc-debug
        │   │   └── linux-gcc-release
        │   └── include/
        ├── blast/
        └── flow/
```

Once the project generated, we can compile it.

```bash
cd compiler/linux-gcc-release
make
make install
```

```
hello_worlds/
├── CMakeLists.txt
├── src/
└── External/
    └── PhysX/
        ├── physx/
        │   ├── install
        │   │   └── linux
        │   │       └── PhysX
        │   │           ├── bin
        │   │           └── include
        │   ├── generate_projects.sh
        │   ├── compiler/
        │   │   ├── linux-gcc-debug
        │   │   └── linux-gcc-release
        │   └── include/
        ├── blast/
        └── flow/
```

## Build HelloWorlds

HelloWorlds is ROS independent but supports to be a ROS package. In such a way it can be built in different way depending on you usage.

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
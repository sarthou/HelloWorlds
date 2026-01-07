# Minimum CMake version
cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(hello_worlds)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if($ENV{ROS_VERSION} AND $ENV{ROS_VERSION} STREQUAL "2")
  find_package(ament_cmake)
  message(STATUS "Configuring for ROS 2 using ament_cmake...")
  set(ROS_BUILD_TYPE "ROS2")

elseif($ENV{ROS_VERSION} AND $ENV{ROS_VERSION} STREQUAL "1")
  find_package(catkin COMPONENTS)
  message(STATUS "Configuring for ROS 1 using catkin...")
  set(ROS_BUILD_TYPE "ROS1")

  catkin_package(

    # We don't need to specify LIBRARIES, CATKIN_DEPENDS, or DEPENDS
    # since your library doesn't depend on other ROS packages
  )

else()
  message(STATUS "Configuring for Standalone (No ROS) build...")
  set(ROS_BUILD_TYPE "STANDALONE")
  include(GNUInstallDirs)
endif()

find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(TINYXML2 REQUIRED tinyxml2)
pkg_search_module(CURL REQUIRED libcurl IMPORTED_TARGET GLOBAL)

include(FetchContent)
include(CheckLanguage)

set(CAD_FILES
  models/basic_shapes/cube.obj
  models/basic_shapes/cylinder.obj
  models/basic_shapes/sphere.obj
)

set(SHADER_FILES
  shaders/color_shader.fs
  shaders/color_shader.vs
  shaders/depth_shader.fs
  shaders/depth_shader.gs
  shaders/depth_shader.vs
  shaders/depthcube_shader.fs
  shaders/depthcube_shader.gs
  shaders/depthcube_shader.vs
  shaders/light_shader.fs
  shaders/light_shader.vs
  shaders/screen_shader.fs
  shaders/screen_shader.vs
  shaders/sky_shader.fs
  shaders/sky_shader.vs
  shaders/text_shader.fs
  shaders/text_shader.vs
)

# --- Embeded files ---
set(EMBEDDED_SUBDIR "hello_worlds_embedded")
set(EMBEDDED_DIR "${CMAKE_CURRENT_BINARY_DIR}/${EMBEDDED_SUBDIR}")
file(MAKE_DIRECTORY "${EMBEDDED_DIR}")

set(CAD_HEADERS "")
set(SHADER_HEADERS "")

function(embed_file_list file_list output_header_list_name)
  foreach(RESOURCE_PATH ${${file_list}})
    get_filename_component(BASE_NAME "${RESOURCE_PATH}" NAME)
    string(REPLACE "." "_" VAR_NAME "${BASE_NAME}")

    set(GENERATED_HEADER "${EMBEDDED_DIR}/embedded_${BASE_NAME}.h")

    add_custom_command(
      OUTPUT ${GENERATED_HEADER}
      COMMAND ${CMAKE_COMMAND} -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_PATH}"
      -DOUTPUT_HEADER=${GENERATED_HEADER}
      -DVAR_NAME=${VAR_NAME}
      -P "${CMAKE_CURRENT_SOURCE_DIR}/ResourceEmbed.cmake"
      DEPENDS "${RESOURCE_PATH}"
      COMMENT "Embedding resource: ${BASE_NAME}"
    )

    list(APPEND ${output_header_list_name} ${GENERATED_HEADER})
  endforeach()

  set(${output_header_list_name} ${${output_header_list_name}} PARENT_SCOPE)
endfunction()

embed_file_list(CAD_FILES CAD_HEADERS)
embed_file_list(SHADER_FILES SHADER_HEADERS)

# --- Registry file generation ---
set(REGISTRY_FILE "${EMBEDDED_DIR}/resource_registry.cpp")
set(REGISTRY_CONTENT "#include <vector>\n#include <string>\n#include <cstddef>\n")

foreach(HEADER ${CAD_HEADERS} ${SHADER_HEADERS})
  get_filename_component(FNAME ${HEADER} NAME)
  string(APPEND REGISTRY_CONTENT "#include \"${FNAME}\"\n")
endforeach()

string(APPEND REGISTRY_CONTENT "\nstruct ResourceEntry_t { std::string name; const char* data; size_t size; };\n")
string(APPEND REGISTRY_CONTENT "std::vector<ResourceEntry_t> getAllResources() {\n    return {\n")

foreach(FILE_PATH ${CAD_FILES} ${SHADER_FILES})
  get_filename_component(FNAME ${FILE_PATH} NAME)
  string(MAKE_C_IDENTIFIER ${FNAME} VAR_NAME)
  string(APPEND REGISTRY_CONTENT "        {\"${FNAME}\", resources::${VAR_NAME}_data, resources::${VAR_NAME}_size},\n")
endforeach()

string(APPEND REGISTRY_CONTENT "    };\n}\n")

file(WRITE ${REGISTRY_FILE} "${REGISTRY_CONTENT}")

# --- Libraries Compilation ---
function(hws_add_library TARGET)
  if(NOT TARGET)
    message(FATAL_ERROR "Expected the target name as first argument")
  endif()

  if(NOT ARGN)
    message(FATAL_ERROR "Expected source file list after target name")
  endif()

  add_library(${TARGET} ${ARGN})

  target_include_directories(${TARGET}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/External>
    $<INSTALL_INTERFACE:include>)

  set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

  target_compile_options(${TARGET}
    PRIVATE
    -Wall -Wextra)
endfunction(hws_add_library)

hws_add_library(hello_worlds_SolarAzEl External/SolarAzEl/SolarAzEl.cpp)

hws_add_library(hello_worlds_utility_lib
  src/Utils/RosPackage.cpp
  src/Utils/XmlTokenize.cpp)

hws_add_library(hello_worlds_engine_common
  src/Common/Camera/Camera.cpp
  src/Common/Camera/CameraUpdater.cpp
  src/Common/Camera/VirtualCamera.cpp
  src/Common/Debug/DebugLine.cpp
  src/Common/Lights/AmbientLight.cpp
  src/Common/Lights/PointLights.cpp
  src/Common/Models/Loaders/ColladaLoader.cpp
  src/Common/Models/Loaders/ObjLoader.cpp
  src/Common/Models/Loaders/StlLoader.cpp
  src/Common/Models/Loaders/TinyObjReader.cpp
  src/Common/Models/Loaders/ModelLoader.cpp
  src/Common/Models/Mesh.cpp
  src/Common/Models/Model.cpp
  src/Common/Models/ModelManager.cpp
  src/Common/Urdf/UrdfLoader.cpp
  src/Common/Urdf/Urdf.cpp
  src/Common/Urdf/Actor.cpp
  src/Common/Urdf/VisualActor.cpp
  src/Common/Urdf/Joint.cpp
  src/Common/World.cpp
  ${CAD_HEADERS})
target_link_libraries(hello_worlds_engine_common PUBLIC
  hello_worlds_utility_lib
  hello_worlds_SolarAzEl
  PRIVATE
  ${TinyXML_LIBRARIES}
  ${TinyXML2_LIBRARIES})
target_include_directories(hello_worlds_engine_common PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

hws_add_library(hello_worlds_glad External/glad/glad.c)

hws_add_library(hello_worlds_graphics
  src/Graphics/GLFW/Window.cpp
  src/Graphics/OpenGL/Cubemap.cpp
  src/Graphics/OpenGL/LinesHandle.cpp
  src/Graphics/OpenGL/MeshHandle.cpp
  src/Graphics/OpenGL/OffScreen.cpp
  src/Graphics/OpenGL/PointShadow.cpp
  src/Graphics/OpenGL/Renderer.cpp
  src/Graphics/OpenGL/Screen.cpp
  src/Graphics/OpenGL/Shader.cpp
  src/Graphics/OpenGL/TextRenderer.cpp
  src/Graphics/OpenGL/AmbientShadow.cpp
  src/Graphics/OpenGL/Texture2D.cpp
  ${SHADER_HEADERS})
target_link_libraries(hello_worlds_graphics PUBLIC
  hello_worlds_engine_common
  hello_worlds_glad
  assimp
  glfw
  ${FREETYPE_LIBRARIES})
target_include_directories(hello_worlds_graphics PRIVATE ${FREETYPE_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(hello_worlds_graphics PRIVATE $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-function>)

set(PHYSX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/PhysX/physx/install/linux/PhysX")
set(PHYSX_INCLUDE_DIR "${PHYSX_ROOT_DIR}/include")

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(VERBOSE "Building with PhysX in debug configuration")
  add_compile_definitions(_DEBUG)
  set(PHYSX_BIN_PATH "${PHYSX_ROOT_DIR}/bin/linux.x86_64/release")
else()
  message(VERBOSE "Building with PhysX in release configuration")
  add_compile_definitions(NDEBUG)
  set(PHYSX_BIN_PATH "${PHYSX_ROOT_DIR}/bin/linux.x86_64/release")
endif()

link_directories("${PHYSX_BIN_PATH}")

hws_add_library(hello_worlds_physics
  src/Physics/PhysX/Actors/Actor.cpp
  src/Physics/PhysX/Actors/DynamicActor.cpp
  src/Physics/PhysX/Actors/LinkActor.cpp
  src/Physics/PhysX/Actors/StaticActor.cpp
  src/Physics/PhysX/Joint.cpp
  src/Physics/PhysX/Urdf.cpp
  src/Physics/PhysX/Context.cpp
  src/Physics/PhysX/SharedContext.cpp
  src/Physics/PhysX/World.cpp)
target_include_directories(hello_worlds_physics PUBLIC

  # While BUILDING your project, use these local absolute paths
  $<BUILD_INTERFACE:${PHYSX_INCLUDE_DIR}>

  # After INSTALLING your project, consumers should look in the install folder
  # We will put the PhysX headers in a subfolder called 'PhysXExternal'
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/PhysXExternal>
)

target_link_libraries(hello_worlds_physics PUBLIC hello_worlds_engine_common)
target_link_libraries(hello_worlds_physics
  PUBLIC
  PhysX
  PhysXCommon
  PhysXCooking
  PhysXExtensions
  PhysXFoundation
  PhysXPvdSDK)

hws_add_library(hello_worlds src/Engine.cpp)
target_link_libraries(hello_worlds PUBLIC hello_worlds_physics hello_worlds_graphics)

# --- Testing ---
option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
  # 1. Find GTest
  if(ROS_BUILD_TYPE STREQUAL "ROS1")
    # ROS 1 handles GTest through catkin
    find_package(GTest REQUIRED)
  elseif(ROS_BUILD_TYPE STREQUAL "ROS2")
    # ROS 2 uses ament_cmake_gtest
    find_package(ament_cmake_gtest REQUIRED)
  else()
    # Standalone: Try to find installed GTest or use FetchContent as a backup
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
      include(FetchContent)
      FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
      )
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
      FetchContent_MakeAvailable(googletest)
    endif()
  endif()

  # 2. Register the Test
  if(ROS_BUILD_TYPE STREQUAL "ROS1")
    # This macro creates the executable and registers it with catkin/ctest
    catkin_add_gtest(ResourceTests tests/resource_embedding.cpp ${REGISTRY_FILE} ${CAD_HEADERS} ${SHADER_HEADERS})
    target_include_directories(ResourceTests PRIVATE "${EMBEDDED_DIR}")

  elseif(ROS_BUILD_TYPE STREQUAL "ROS2")
    # This macro handles the ament-specific test registration
    ament_add_gtest(ResourceTests tests/resource_embedding.cpp ${REGISTRY_FILE} ${CAD_HEADERS} ${SHADER_HEADERS})
    target_include_directories(ResourceTests PRIVATE "${EMBEDDED_DIR}")

  else()
    # Standalone standard CMake
    enable_testing()
    add_executable(ResourceTests tests/resource_embedding.cpp ${REGISTRY_FILE} ${CAD_HEADERS} ${SHADER_HEADERS})
    target_link_libraries(ResourceTests PRIVATE GTest::gtest_main)
    target_include_directories(ResourceTests PRIVATE "${EMBEDDED_DIR}")

    include(GoogleTest)
    gtest_discover_tests(ResourceTests)
  endif()
endif()

# --- 7. ROS-Specific Installation/Setup ---
include(GNUInstallDirs)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY "${PHYSX_INCLUDE_DIR}"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/PhysXExternal"
)

install(TARGETS
  hello_worlds_SolarAzEl
  hello_worlds_utility_lib
  hello_worlds_engine_common
  hello_worlds_glad
  hello_worlds_graphics
  hello_worlds_physics
  hello_worlds
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ROS 2 Specific Exports
if(ROS_BUILD_TYPE STREQUAL "ROS2")
  # This tells ament to export the targets we defined above
  ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)

  # This installs the generated CMake files for ament
  install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION share/${PROJECT_NAME}/cmake
  )
endif()

# Standalone Specific Exports (Optional but professional)
if(ROS_BUILD_TYPE STREQUAL "STANDALONE")
  install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Config.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )
endif()

# IMPORTANT: This must be the very last line for ROS 2
if(ROS_BUILD_TYPE STREQUAL "ROS2")
  ament_package()
endif()
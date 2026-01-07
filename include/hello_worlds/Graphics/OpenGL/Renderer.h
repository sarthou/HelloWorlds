#ifndef HWS_GRAPHICS_OPENGL_RENDERER_H
#define HWS_GRAPHICS_OPENGL_RENDERER_H

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "hello_worlds/Common/Camera/Camera.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Common/Shapes/Shape.h"
#include "hello_worlds/Common/Urdf/Actor.h"
#include "hello_worlds/Graphics/Common/InstanceData.h"
#include "hello_worlds/Graphics/OpenGL/AmbientShadow.h"
#include "hello_worlds/Graphics/OpenGL/Cubemap.h"
#include "hello_worlds/Graphics/OpenGL/LinesHandle.h"
#include "hello_worlds/Graphics/OpenGL/MeshHandle.h"
#include "hello_worlds/Graphics/OpenGL/OffScreen.h"
#include "hello_worlds/Graphics/OpenGL/PointShadow.h"
#include "hello_worlds/Graphics/OpenGL/Screen.h"
#include "hello_worlds/Graphics/OpenGL/Shader.h"
#include "hello_worlds/Graphics/OpenGL/TextRenderer.h"
#include "hello_worlds/Graphics/OpenGL/Texture2D.h"

namespace hws {
  class Window;
  class World;
  class AmbientLight;
  class PointLights;

  class Renderer
  {
  public:
    ~Renderer();
    static void init();
    static void release();

    bool initialize(Window& window);
    void setBackgroundColor(float r, float g, float b);
    void addSkyBox(const std::string& images_folder); // TODO: method to set default background color ?
    void attachWorld(World* world) { world_ = world; }

    void commit();

    void setRenderCamera(Camera* camera);

  private:
    World* world_ = nullptr;
    Camera render_camera_;
    std::unordered_map<std::string, Shader> shaders_;
    Screen screen_;
    std::vector<OffScreen> off_screens_;
    Cubemap sky_;
    AmbientShadow shadow_;
    PointShadow point_shadows_;
    TextRenderer text_renderer_;
    float max_fps_ = 120;
    float delta_time_;
    float last_frame_ = 0;

    bool render_collision_models_ = false;
    bool render_shadows_ = false;

    std::vector<std::function<void()>> render_steps_;

    bool use_sky_box_ = false;
    std::array<float, 3> background_color_{0.5, 0.5, 0.5};

    std::unordered_map<std::string, Texture2D> loaded_textures_;
    std::unordered_map<Model::Id, std::unordered_map<Mesh::Id, MeshHandle>> cached_models_;
    std::unordered_map<Model::Id, std::unordered_map<Mesh::Id, std::vector<InstanceData>>> current_mesh_batches_;
    std::unordered_map<unsigned int, LinesHandle> cached_lines_;
    std::unordered_map<unsigned int, LinesHandle> debug_axis_;
    std::unordered_map<unsigned int, LinesHandle> debug_lights_;

    void loadWorld();
    void loadActor(Actor* actor, const ShapeBox& shape, bool default_material = false);
    void loadActor(Actor* actor, const ShapeCapsule& shape, bool default_material = false);
    void loadActor(Actor* actor, const ShapeCustomMesh& shape, bool default_material = false);
    void loadActor(Actor* actor, const ShapeCylinder& shape, bool default_material = false);
    void loadActor(Actor* actor, const ShapeDummy& shape, bool default_material = false);
    void loadActor(Actor* actor, const ShapeSphere& shape, bool default_material = false);
    void loadInstance(const Model& model, const Material& material, const glm::mat4& model_mat, uint32_t object_id);
    Material combineMaterials(const Material& shape_material, const Material& model_material);
    std::vector<Texture2D> loadTextures(Material& material);
    void loadModel(const Model& model, const Material& material);
    void loadDebugLines();

    void renderModels(const Shader& shader, unsigned int texture_offset = 0);
    void renderModelsSegmented(const Shader& shader);

    void renderMainScreen();
    void renderOffScreens();
    void renderOffscreenRgb(Camera* camera);
    void renderOffscreenSegmented(Camera* camera);
    void renderAmbientShadowDepth();
    void renderPointShadowDepth();
    void renderDebug();

    void setLightsUniforms(const Shader& shader, bool use_ambient_shadows = true, bool use_points_shadows = true);
    void setAntiAliasing(ViewAntiAliasing_e setting);

    Material createColisionMaterial(size_t uid);
  };
} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_RENDERER_H

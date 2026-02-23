#include "hello_worlds/Graphics/OpenGL/Renderer.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <variant>
#include <vector>

#include "glad/glad.h"
#include "hello_worlds/Common/Camera/Camera.h"
#include "hello_worlds/Common/Camera/CameraView.h"
#include "hello_worlds/Common/Camera/ViewAntiAliasing.h"
#include "hello_worlds/Common/Debug/DebugLine.h"
#include "hello_worlds/Common/Lights/AmbientLight.h"
#include "hello_worlds/Common/Lights/PointLights.h"
#include "hello_worlds/Common/Models/Color.h"
#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Common/Shapes/Shape.h"
#include "hello_worlds/Common/Urdf/Actor.h"
#include "hello_worlds/Common/World.h"
#include "hello_worlds/Graphics/Common/InstanceData.h"
#include "hello_worlds/Graphics/GLFW/Window.h"
#include "hello_worlds/Graphics/OpenGL/Cubemap.h"
#include "hello_worlds/Graphics/OpenGL/MeshHandle.h"
#include "hello_worlds/Graphics/OpenGL/Texture2D.h"
#include "hello_worlds/Utils/GlmMath.h"
#include "hello_worlds_embedded/embedded_color_shader.fs.h"
#include "hello_worlds_embedded/embedded_color_shader.vs.h"
#include "hello_worlds_embedded/embedded_depth_prepass.fs.h"
#include "hello_worlds_embedded/embedded_depth_prepass.vs.h"
#include "hello_worlds_embedded/embedded_depth_shader.fs.h"
#include "hello_worlds_embedded/embedded_depth_shader.gs.h"
#include "hello_worlds_embedded/embedded_depth_shader.vs.h"
#include "hello_worlds_embedded/embedded_depthcube_shader.fs.h"
#include "hello_worlds_embedded/embedded_depthcube_shader.gs.h"
#include "hello_worlds_embedded/embedded_depthcube_shader.vs.h"
#include "hello_worlds_embedded/embedded_light_shader.fs.h"
#include "hello_worlds_embedded/embedded_light_shader.vs.h"
#include "hello_worlds_embedded/embedded_screen_shader.fs.h"
#include "hello_worlds_embedded/embedded_screen_shader.vs.h"
#include "hello_worlds_embedded/embedded_sky_shader.fs.h"
#include "hello_worlds_embedded/embedded_sky_shader.vs.h"
#include "hello_worlds_embedded/embedded_text_shader.fs.h"
#include "hello_worlds_embedded/embedded_text_shader.vs.h"

// should be after glad
#include <GLFW/glfw3.h>

#define POINT_SHADOW_TEXTURE 20

void GLAPIENTRY
messageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  if(severity == GL_DEBUG_SEVERITY_HIGH /* || (severity == GL_DEBUG_SEVERITY_MEDIUM)*/)
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

namespace hws {

  Renderer::~Renderer()
  {
    delete screen_sharder_;
  }

  void Renderer::init()
  {
    hws::Window::init();
    hws::Window tmp("HelloWorlds");
    tmp.makeCurrentContext();

    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwMakeContextCurrent(nullptr);
  }

  void Renderer::release()
  {
    hws::Window::release();
  }

  bool Renderer::initialize(Window& window, float max_fps)
  {
    max_fps_ = max_fps;
    window.makeCurrentContext();

    setRenderCamera(window.getUpdatedCamera());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(messageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_MULTISAMPLE);
    screen_.init();
    screen_.setSize((unsigned int)render_camera_.getWidth(), (unsigned int)render_camera_.getHeight());
    if(render_camera_.getAASetting() != ViewAntiAliasing_e::off)
      setAntiAliasing(render_camera_.getAASetting());

    screen_sharder_ = new Shader(resources::screen_shader_vs_data, resources::screen_shader_fs_data);
    main_shader_ = new DefaultShader(resources::light_shader_vs_data, resources::light_shader_fs_data);

    shaders_.insert({
      "depth", {resources::depth_shader_vs_data, resources::depth_shader_fs_data, resources::depth_shader_gs_data}
    });
    shaders_.insert({
      "depth_prepass", {resources::depth_prepass_vs_data, resources::depth_prepass_fs_data}
    });
    shaders_.insert({
      "depthcube", {resources::depthcube_shader_vs_data, resources::depthcube_shader_fs_data, resources::depthcube_shader_gs_data}
    });
    shaders_.insert({
      "text", {resources::text_shader_vs_data, resources::text_shader_fs_data}
    });
    shaders_.insert({
      "color", {resources::color_shader_vs_data, resources::color_shader_fs_data}
    });

    shadow_.init(render_camera_.getNearPlane(), render_camera_.getFarPlane());
    point_shadows_.init();
    initShadowSamplers();
    text_renderer_.init();
    text_renderer_.load("/usr/share/fonts/truetype/open-sans/OpenSans-Regular.ttf", 48);

    debug_axis_.emplace(0,
                        DebugLine({glm::vec3(0., 0., 0.),
                                   glm::vec3(1., 0., 0.)},
                                  {0, 1},
                                  glm::vec3(1., 0., 0.)));
    debug_axis_.emplace(1,
                        DebugLine({glm::vec3(0., 0., 0.),
                                   glm::vec3(0., 1., 0.)},
                                  {0, 1},
                                  glm::vec3(0., 1., 0.)));
    debug_axis_.emplace(2,
                        DebugLine({glm::vec3(0., 0., 0.),
                                   glm::vec3(0., 0., 1.)},
                                  {0, 1},
                                  glm::vec3(0., 0., 1.)));

    screen_sharder_->use();
    screen_sharder_->setInt("screenTexture", 0);

    render_steps_.emplace_back([this]() { loadWorld(); });
    render_steps_.emplace_back([this]() { renderPointShadowDepth(); });
    render_steps_.emplace_back([this]() { loadDebugLines(); });
    render_steps_.emplace_back([this]() { renderAmbientShadowDepth(); });
    render_steps_.emplace_back([this]() { renderMainScreen(); });

    return true;
  }

  void Renderer::addSkyBox(const std::string& images_folder)
  {
    if(sky_.init(images_folder))
    {
      shaders_.insert({
        "sky", {resources::sky_shader_vs_data, resources::sky_shader_fs_data}
      });
      use_sky_box_ = true;
    }
  }

  void Renderer::attachWorld(World* world)
  {
    world_ = world;
    if(world_->skybox_folder_.empty() == false)
      addSkyBox(world_->skybox_folder_);
    background_color_ = world_->background_color_;
  }

  bool Renderer::shouldRender()
  {
    float current_frame = (float)glfwGetTime();
    if(last_frame_ <= 0)
      last_frame_ = current_frame;
    float delta_time = current_frame - last_frame_;

    if(world_->has_render_request_)
      return true;
    else
      return ((1.0f / max_fps_) - delta_time <= 0);
  }

  void Renderer::commit()
  {
    if(world_ == nullptr)
      return;

    float current_frame = (float)glfwGetTime();
    delta_time_ = current_frame - last_frame_;
    last_frame_ = current_frame;

    world_->processDebugLifeTime((double)delta_time_);

    for(auto& model : current_mesh_batches_)
      for(auto& mesh : model.second)
        mesh.second.clear();

    render_collision_models_ = render_camera_.shouldRendercollisionModels();
    render_shadows_ = render_camera_.shouldShadows();

    for(const auto& step : render_steps_)
    {
      step();
      renderOffScreens();
    }
    glFinish();
  }

  void Renderer::setRenderCamera(Camera* camera)
  {
    render_camera_ = *camera;
    if(render_camera_.sizeHasChanged())
    {
      screen_.setSize((unsigned int)render_camera_.getWidth(), (unsigned int)render_camera_.getHeight());
      glViewport(0, 0, (int)render_camera_.getWidth(), (int)render_camera_.getHeight());
      screen_.reinitBuffers();
    }
  }

  void Renderer::loadWorld()
  {
    for(const auto& actor : world_->getActors())
    {
      if(render_collision_models_)
      {
        std::visit([this, actor](const auto& shape_resolv) { loadActor(actor.second, shape_resolv, true); }, actor.second->collision_shape_);
      }
      else
      {
        for(const auto& shape : actor.second->visual_shapes_)
        {
          std::visit([this, actor](const auto& shape_resolv) { loadActor(actor.second, shape_resolv); }, shape);
        }
      }
    }

    for(size_t i = 0; i < PointLights::MAX_POINT_LIGHTS; i++)
    {
      if(world_->point_lights_.isUsed(i))
      {
        double dist = world_->point_lights_.getAttenuationDistance(i);
        auto pose = world_->point_lights_.getPosition(i);
        auto color = world_->point_lights_.getDiffuse(i);
        if(debug_lights_.contains(i) == false)
        {
          debug_lights_.emplace(i,
                                DebugLine({glm::vec3(pose.x, pose.y, pose.z),
                                           glm::vec3(pose.x, pose.y, pose.z - dist)},
                                          {0, 1},
                                          glm::vec3(color.x, color.y, color.z)));
        }
      }
      else
        debug_lights_.erase(i);
    }
  }

  void Renderer::loadActor(Actor* actor, const ShapeBox& shape, bool default_material)
  {
    const auto size_mat = glm::scale(glm::mat4(1.f), toGlmV3(shape.half_extents_));
    const auto model_mat = shape.shape_transform_ * toM4(actor->getModelMatrix()) * size_mat;

    if(default_material == false)
      loadInstance(shape.box_model_, {"", shape.diffuse_color_, shape.diffuse_color_, 0., "", "", ""}, model_mat, actor->unique_id_);
    else
      loadInstance(shape.box_model_, createColisionMaterial(actor->unique_id_), model_mat, actor->unique_id_);
  }

  void Renderer::loadActor(Actor* actor, const ShapeCapsule& shape, bool default_material)
  {
    (void)actor; // todo
    (void)shape; // todo
    (void)default_material;
  }

  void Renderer::loadActor(Actor* actor, const ShapeCustomMesh& shape, bool default_material)
  {
    const auto size_mat = glm::scale(glm::mat4(1.f), shape.scale_);
    const auto model_mat = toM4(actor->getModelMatrix()) * shape.shape_transform_ * size_mat;

    if(default_material == false)
      loadInstance(shape.custom_model_, shape.material_, model_mat, actor->unique_id_);
    else
      loadInstance(shape.custom_model_, createColisionMaterial(actor->unique_id_), model_mat, actor->unique_id_);
  }

  void Renderer::loadActor(Actor* actor, const ShapeCylinder& shape, bool default_material)
  {
    const auto size_mat = glm::scale(glm::mat4(1.f), glm::vec3(shape.radius_, shape.height_, shape.radius_));
    const auto model_mat = toM4(actor->getModelMatrix()) * shape.shape_transform_ * size_mat;

    if(default_material == false)
      loadInstance(shape.cylinder_model_, {"", shape.diffuse_color_, shape.diffuse_color_, 0., "", "", ""}, model_mat, actor->unique_id_);
    else
      loadInstance(shape.cylinder_model_, createColisionMaterial(actor->unique_id_), model_mat, actor->unique_id_);
  }

  void Renderer::loadActor(Actor* actor, const ShapeDummy& shape, bool default_material)
  {
    (void)actor;
    (void)shape;
    (void)default_material;
  }

  void Renderer::loadActor(Actor* actor, const ShapeSphere& shape, bool default_material)
  {
    (void)actor; // todo
    (void)shape; // todo
    (void)default_material;
  }

  void Renderer::loadInstance(const Model& model, const Material& material, const glm::mat4& model_mat, uint32_t object_id)
  {
    loadModel(model, material, object_id);

    for(const auto& mesh : model.meshes_)
    {
      glm::vec3 world_center = glm::vec3(model_mat[3]) + glm::mat3(model_mat) * mesh.getCenter();

      float s1 = glm::dot(glm::vec3(model_mat[0]), glm::vec3(model_mat[0]));
      float s2 = glm::dot(glm::vec3(model_mat[1]), glm::vec3(model_mat[1]));
      float s3 = glm::dot(glm::vec3(model_mat[2]), glm::vec3(model_mat[2]));

      float max_scale_sq = glm::max(s1, glm::max(s2, s3));
      float world_radius = mesh.getRadius() * glm::sqrt(max_scale_sq);

      current_mesh_batches_[model.id_][mesh.id_].emplace_back(InstanceData{model_mat, world_center, world_radius, {}, object_id}); // TODO add instance data ?
    }
  }

  Material Renderer::combineMaterials(const Material& shape_material, const Material& model_material)
  {
    Material material;

    if(model_material.diffuse_texture_.empty())
      material.diffuse_texture_ = shape_material.diffuse_texture_;
    else
      material.diffuse_texture_ = model_material.diffuse_texture_;

    if(model_material.specular_texture_.empty())
      material.specular_texture_ = shape_material.specular_texture_;
    else
      material.specular_texture_ = model_material.specular_texture_;

    if(model_material.normal_texture_.empty())
      material.normal_texture_ = shape_material.normal_texture_;
    else
      material.normal_texture_ = model_material.normal_texture_;

    if(model_material.diffuse_color_.a_ == 0.f)
    {
      if(shape_material.diffuse_color_.a_ > 0.001f)
        material.diffuse_color_ = shape_material.diffuse_color_;
      else
      {
        material.diffuse_color_ = model_material.diffuse_color_;
        material.diffuse_color_.a_ = 1.0;
      }
    }
    else
      material.diffuse_color_ = model_material.diffuse_color_;

    if(model_material.specular_color_.a_ <= 0.001f)
    {
      if(shape_material.specular_color_.a_ > 0.001f)
        material.specular_color_ = shape_material.specular_color_;
      else
      {
        material.specular_color_ = model_material.specular_color_;
        material.specular_color_.a_ = 1.0;
      }
    }
    else
      material.specular_color_ = model_material.specular_color_;

    if(model_material.shininess_ == -1.f)
      material.shininess_ = shape_material.shininess_;
    else
      material.shininess_ = shape_material.shininess_;

    return material;
  }

  std::vector<Texture2D> Renderer::loadTextures(Material& material)
  {
    std::vector<Texture2D> textures;

    if(material.diffuse_texture_.empty() == false)
    {
      auto text_it = loaded_textures_.find(material.diffuse_texture_);
      if(text_it == loaded_textures_.end())
      {
        text_it = loaded_textures_.insert({
                                            material.diffuse_texture_, {material.diffuse_texture_, texture_diffuse, true, true}
        })
                    .first;
      }
      textures.emplace_back(text_it->second);
    }

    if(material.specular_texture_.empty() == false)
    {
      auto text_it = loaded_textures_.find(material.specular_texture_);
      if(text_it == loaded_textures_.end())
      {
        text_it = loaded_textures_.insert({
                                            material.specular_texture_, {material.specular_texture_, texture_specular, false, true}
        })
                    .first;
      }
      textures.emplace_back(text_it->second);
    }

    if(material.normal_texture_.empty() == false)
    {
      auto text_it = loaded_textures_.find(material.normal_texture_);
      if(text_it == loaded_textures_.end())
      {
        text_it = loaded_textures_.insert({
                                            material.normal_texture_, {material.normal_texture_, texture_normal, false, true}
        })
                    .first;
      }
      textures.emplace_back(text_it->second);
    }

    return textures;
  }

  void Renderer::loadModel(const Model& model, const Material& material, uint32_t instance_id)
  {
    if(model.meshes_.empty())
      return;

    auto model_it = cached_models_.find(model.id_);
    if(model_it == cached_models_.end())
      model_it = cached_models_.insert({model.id_, {}}).first;

    for(const auto& mesh : model.meshes_)
    {
      auto mesh_it = model_it->second.find(mesh.id_);
      if(mesh_it != model_it->second.end())
      {
        if(mesh_it->second.materials.contains(instance_id))
          continue;
      }
      else
        mesh_it = model_it->second.insert({mesh.id_, MeshHandle(mesh)}).first;

      auto mesh_material = combineMaterials(mesh.material_, material);
      auto textures = loadTextures(mesh_material);

      MeshMaterialHandle_t material_handle;
      material_handle.textures = textures;
      material_handle.color = mesh_material.diffuse_color_;
      material_handle.color.a_ = 1.0;
      material_handle.shininess = mesh_material.shininess_ <= 0 ? 64.f : mesh_material.shininess_;
      material_handle.specular = mesh_material.specular_color_.a_ == 0. ? 0.1f : mesh_material.specular_color_.r_;
      mesh_it->second.materials.emplace(instance_id, material_handle);
    }
  }

  void Renderer::loadDebugLines()
  {
    std::set<unsigned int> ids;
    for(auto& cached : cached_lines_)
      ids.insert(cached.first);

    for(auto& debug_line : world_->debug_lines_)
    {
      if(debug_line.indices_.empty())
      {
        cached_lines_.erase(debug_line.id_);
        continue;
      }

      auto cache_it = cached_lines_.find(debug_line.id_);
      if(cache_it == cached_lines_.end())
        cached_lines_.emplace(debug_line.id_, debug_line);

      ids.erase(debug_line.id_);
    }

    for(auto id : ids)
      cached_lines_.erase(id);
  }

  void Renderer::renderMainScreen()
  {
    auto cameraCull = [&](const glm::vec3& center, float radius) {
      return render_camera_.getFrustum().isSphereVisible(center, radius);
    };

    // --- SETUP ---
    screen_.bindFrameBuffer();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // --- STEP 0: DEPTH PRE-PASS ---
    auto& prepass = shaders_.at("depth_prepass");
    prepass.use();
    prepass.setView(render_camera_.getViewMatrix());
    prepass.setProjection(render_camera_.getProjectionMatrix());

    // Prepare for Depth-Only
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.2f, 1.0f); // push pre-pass depth away // factor, units
    renderModels(prepass, cameraCull);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // --- STEP 1: COLOR PASS ---
    main_shader_->use();

    // Enable color writing again
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    auto sky = world_->ambient_light_.getSkyColor(glm::vec3(background_color_[0], background_color_[1], background_color_[2]));
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE); // The depth is already perfect; no need to write it again

    main_shader_->setVec3("view_pose", render_camera_.getPosition());
    main_shader_->setView(render_camera_.getViewMatrix());
    main_shader_->setProjection(render_camera_.getProjectionMatrix());

    shadow_.setUniforms(*main_shader_, 1);
    setLightsUniforms(main_shader_, render_shadows_, render_shadows_);

    renderTexturedModels(main_shader_, 2, cameraCull);

    // --- STEP 2: SKYBOX & DEBUG ---
    glDepthMask(GL_TRUE); // Back on for skybox/debug
    glDepthFunc(GL_LESS);

    if(use_sky_box_)
    {
      auto& sky_shader = shaders_.at("sky");
      sky_shader.use();
      glm::mat4 view = glm::mat4(glm::mat3(render_camera_.getViewMatrix()));
      sky_shader.setView(view);
      sky_shader.setProjection(render_camera_.getProjectionMatrix());
      sky_shader.setVec4("color", world_->ambient_light_.getDiffuse());

      sky_.draw(sky_shader);
    }

    if(render_camera_.shouldRenderDebug())
      renderDebug();

    // --- STEP 3: SCREEN ---
    // blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
    screen_.generateColorTexture();

    // render quad with scene's visuals as its texture image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);

    // draw Screen quad
    screen_sharder_->use();
    screen_.draw();
  }

  void Renderer::renderOffScreens()
  {
    if(world_->has_render_request_)
    {
      for(size_t i = 0; i < world_->cameras_.size(); i++)
      {
        auto& virtual_camera = world_->cameras_[i];
        if(i >= off_screens_.size())
        {
          off_screens_.emplace_back();
          off_screens_.back().init(virtual_camera.getWidth(), virtual_camera.getHeight());
        }

        off_screens_.at(i).bindFrameBuffer();

        Camera* camera = virtual_camera.getCamera();
        if(camera->getViewType() == CameraView_e::regular_view)
          renderOffscreenRgb(camera);
        else
          renderOffscreenSegmented(camera);

        // glFlush();
        off_screens_[i].getImage(virtual_camera.getImageData());
      }
      world_->has_render_request_ = false;
    }
  }

  void Renderer::renderOffscreenRgb(Camera* camera)
  {
    // 1. draw scene as normal in multisampled buffers
    auto sky = world_->ambient_light_.getSkyColor(glm::vec3(background_color_[0], background_color_[1], background_color_[2]));
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    main_shader_->use();

    main_shader_->setVec3("view_pose", camera->getPosition());
    main_shader_->setView(camera->getViewMatrix());
    main_shader_->setProjection(camera->getProjectionMatrix());

    shadow_.setUniforms(*main_shader_, 1);
    // shadow_.setLightMatrices();
    setLightsUniforms(main_shader_, false, render_shadows_); // We never use the ambient shadows for offscreen

    auto cameraCull = [&](const glm::vec3& center, float radius) {
      return camera->getFrustum().isSphereVisible(center, radius);
    };

    renderTexturedModels(main_shader_, 2, cameraCull);

    // 1.2 draw background
    if(use_sky_box_)
    {
      auto& sky_shader = shaders_.at("sky");
      sky_shader.use();
      glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
      sky_shader.setView(view);
      sky_shader.setProjection(camera->getProjectionMatrix());
      sky_shader.setVec4("color", world_->ambient_light_.getDiffuse());

      sky_.draw(sky_shader);
    }
  }

  void Renderer::renderOffscreenSegmented(Camera* camera)
  {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto& shader = shaders_.at("color");

    shader.use();
    shader.setView(camera->getViewMatrix());
    shader.setProjection(camera->getProjectionMatrix());

    auto cameraCull = [&](const glm::vec3& center, float radius) {
      return camera->getFrustum().isSphereVisible(center, radius);
    };

    renderModelsSegmented(shader, cameraCull);
  }

  void Renderer::renderAmbientShadowDepth()
  {
    if(render_shadows_ == false)
      return;

    auto& shadow_shader = shaders_.at("depth");
    shadow_shader.use();

    shadow_.bindFrameBuffer();
    glEnable(GL_DEPTH_TEST);

    if(world_->ambient_light_.getDirection().y >= -0.3)
    {
      auto ambient_dir = -glm::normalize(world_->ambient_light_.getDirection());
      shadow_.computeLightSpaceMatrices(render_camera_, ambient_dir);

      shadow_.setLightMatrices();

      auto ambientCull = [&](const glm::vec3& center, float radius) {
        return shadow_.getMasterFrustum().isSphereVisible(center, radius);
      };

      renderModels(shadow_shader, ambientCull);
    }
  }

  void Renderer::renderPointShadowDepth()
  {
    if(render_shadows_ == false)
      return;

    auto& shadow_point_shader = shaders_.at("depthcube");
    glEnable(GL_DEPTH_TEST);
    Frustum_t cam_frustum = render_camera_.getFrustum();

    // Points shadows
    shadow_point_shader.use();
    for(size_t i = 0; i < PointLights::MAX_POINT_LIGHTS; i++)
    {
      if(world_->point_lights_.isUsed(i) && world_->point_lights_.isOn(i))
      {
        glm::vec3 light_pos = world_->point_lights_.getPosition(i);
        float light_radius = world_->point_lights_.getAttenuationDistance(i) * 1.2;

        if(!cam_frustum.isSphereVisible(light_pos, light_radius)) // /!\ point shadows may not be computed for offscreen rendering
          continue;

        point_shadows_.bindFrameBuffer(i);

        if(point_shadows_.isInit(i) == false)
          point_shadows_.init(i, world_->point_lights_.getAttenuationDistance(i));

        point_shadows_.computeLightTransforms(i, glm::vec3(world_->point_lights_.getPosition(i)));

        point_shadows_.setUniforms(i, shadow_point_shader);

        auto pointLightCull = [&](const glm::vec3& center, float radius) {
          float d = glm::distance(light_pos, center);
          return d < (light_radius + radius);
        };

        renderModels(shadow_point_shader, pointLightCull);
      }
    }
  }

  void Renderer::renderDebug()
  {
    auto& text_shader = shaders_.at("text");
    auto& lines_shader = shaders_.at("color");

    // Draw lines

    lines_shader.use();
    lines_shader.setView(render_camera_.getViewMatrix());
    lines_shader.setProjection(render_camera_.getProjectionMatrix());
    lines_shader.setModel(glm::mat4(1.));

    for(auto& line_handle : cached_lines_)
    {
      if(line_handle.second.actor == nullptr)
        lines_shader.setModel(glm::mat4(1.));
      else
      {
        auto pose = line_handle.second.actor->getPositionAndOrientation().first;
        lines_shader.setModel(glm::translate(glm::mat4(1), glm::vec3(pose[0], pose[1], pose[2])));
      }
      line_handle.second.draw(lines_shader);
    }

    if(render_camera_.shouldRenderAllDebug())
    {
      glLineWidth(6);
      for(auto& debug_light : debug_lights_)
        debug_light.second.draw(lines_shader);
      glLineWidth(1);
    }

    // Draw debug axis

    glDisable(GL_DEPTH_TEST);
    glLineWidth(3);
    glm::mat4 model = glm::scale(glm::mat4(1), glm::vec3(0.05, 0.05, 0.05));
    glm::mat4 view = glm::mat4(glm::mat3(render_camera_.getViewMatrix()));
    double aspect = (double)screen_.width_ / (double)screen_.height_;
    double trans = tan(render_camera_.getFov() / 2.);
    double trans_x = trans - 0.05;
    double trans_y = trans - (0.05 * aspect);
    glm::mat4 view_translation = glm::translate(glm::mat4(1), glm::vec3(trans_x * aspect, -trans_y, -1.));
    view = view_translation * view;

    lines_shader.setProjection(render_camera_.getProjectionMatrix());
    lines_shader.setView(view);
    lines_shader.setModel(model);

    debug_axis_.at(0).draw(lines_shader);
    debug_axis_.at(1).draw(lines_shader);
    debug_axis_.at(2).draw(lines_shader);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(1);

    // Draw text

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    text_shader.use();
    text_shader.setProjection(render_camera_.getProjectionMatrix());
    for(const auto& debug : world_->debug_texts_)
    {
      glm::vec4 actor_pose(0., 0., 0., 0.);
      if(debug.linked_actor != nullptr)
      {
        auto pose = debug.linked_actor->getPositionAndOrientation().first;
        actor_pose = glm::vec4(pose[0], pose[1], pose[2], 0.);
      }

      if(debug.text.empty() == false)
        text_renderer_.renderText(text_shader, render_camera_.getViewMatrix(), actor_pose, debug);
    }

    glDisable(GL_BLEND);
  }

  void Renderer::renderTexturedModels(DefaultShader* shader,
                                      unsigned int texture_offset,
                                      std::function<bool(const glm::vec3&, float)> visibility_test)
  {
    for(auto& [model_id, batch] : current_mesh_batches_)
    {
      auto& meshes = cached_models_.at(model_id);

      for(auto& [mesh_id, transforms] : batch)
      {
        const MeshHandle& mesh = meshes.at(mesh_id);

        for(const InstanceData& transform : transforms)
        {
          if(visibility_test && !visibility_test(transform.world_center_, transform.world_radius_))
            continue;

          shader->setModel(transform.mvp_);
          mesh.drawWithMaterial(*shader, transform.object_id_, texture_offset);
        }
      }
    }
  }

  void Renderer::renderModels(const ModelShader& shader,
                              std::function<bool(const glm::vec3&, float)> visibility_test)
  {
    for(auto& [model_id, batch] : current_mesh_batches_)
    {
      auto& meshes = cached_models_.at(model_id);

      for(auto& [mesh_id, transforms] : batch)
      {
        const MeshHandle& mesh = meshes.at(mesh_id);

        for(const InstanceData& transform : transforms)
        {
          if(visibility_test && !visibility_test(transform.world_center_, transform.world_radius_))
            continue;

          shader.setModel(transform.mvp_);
          mesh.draw(shader);
        }
      }
    }
  }

  void Renderer::renderModelsSegmented(const ModelShader& shader,
                                       std::function<bool(const glm::vec3&, float)> visibility_test)
  {
    for(auto& [model_id, batch] : current_mesh_batches_)
    {
      auto& meshes = cached_models_.at(model_id);

      for(auto& [mesh_id, transforms] : batch)
      {
        const MeshHandle& mesh = meshes.at(mesh_id);

        for(const InstanceData& transform : transforms)
        {
          if(visibility_test && !visibility_test(transform.world_center_, transform.world_radius_))
            continue;

          shader.setModel(transform.mvp_);
          mesh.drawId(shader, (uint32_t)transform.object_id_);
        }
      }
    }
  }

  void Renderer::initShadowSamplers()
  {
    main_shader_->use();
    for(size_t i = 0; i < PointLights::MAX_POINT_LIGHTS; i++)
    {
      main_shader_->setInt("point_lights_depth_maps[" + std::to_string(i) + "]", POINT_SHADOW_TEXTURE + i);
    }
  }

  void Renderer::setLightsUniforms(DefaultShader* shader, bool use_ambient_shadows, bool use_points_shadows)
  {
    const AmbientLight& ambient = world_->ambient_light_;

    DirLightUBO_t dir_light{
      ambient.getDirection(),
      ambient.getAmbient(),
      ambient.getDiffuse(),
      ambient.getSpecular(),
      shadow_.getCascadeCount(),
      shadow_.getFarPlane(),
      shadow_.getTexelSize()};
    shader->setDirLight(&dir_light);

    shader->setUsePointLight(use_points_shadows);
    shader->setUseAmbiantShadow(use_ambient_shadows);

    if(use_points_shadows)
    {
      for(size_t i = 0; i < PointLights::MAX_POINT_LIGHTS; i++)
      {
        if(world_->point_lights_.isUsed(i) && world_->point_lights_.isOn(i))
        {
          glActiveTexture(GL_TEXTURE0 + POINT_SHADOW_TEXTURE + i);
          glBindTexture(GL_TEXTURE_CUBE_MAP, point_shadows_.getCubeMapId(i));
        }
      }
    }

    PointLightUBO_t gpu_data[20];
    const auto& points = world_->point_lights_;

    for(size_t i = 0; i < points.getNbLightsSize(); i++)
    {
      gpu_data[i].position = points.getPosition(i);
      gpu_data[i].ambient = points.getAmbient(i);
      gpu_data[i].diffuse = points.getDiffuse(i);
      gpu_data[i].specular = points.getSpecular(i);
      gpu_data[i].attenuation = points.getAttenuation(i);
      gpu_data[i].far_plane = point_shadows_.getFarPlane(i);
      gpu_data[i].on_off = (points.isUsed(i) && points.isOn(i)) ? 1.0f : 0.0f;
    }

    shader->setPointLights(gpu_data, points.getNbLightsSize());
    shader->setNbPointLight(points.getNbLightsFloat());
  }

  void Renderer::setAntiAliasing(ViewAntiAliasing_e setting)
  {
    int samples = 1;
    if(setting == ViewAntiAliasing_e::msaa_x1)
      samples = 1;
    else if(setting == ViewAntiAliasing_e::msaa_x2)
      samples = 2;
    else if(setting == ViewAntiAliasing_e::msaa_x4)
      samples = 4;
    else if(setting == ViewAntiAliasing_e::msaa_x8)
      samples = 8;
    else if(setting == ViewAntiAliasing_e::msaa_x16)
      samples = 16;

    screen_.initBuffers(samples);
  }

  Material Renderer::createColisionMaterial(size_t uid)
  {
    Material material;
    float id = ((float)(uid % 60) / 100.f) + 0.2f;
    material.diffuse_color_ = Color({id, id, id, 1.f});
    material.specular_color_ = Color({id, id, id, 1.f});
    material.shininess_ = 0;

    return material;
  }

} // namespace hws
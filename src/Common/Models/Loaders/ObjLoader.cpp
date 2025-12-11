#include "hello_worlds/Common/Models/Loaders/ObjLoader.h"

#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>

#include "hello_worlds/Common/Models/Material.h"
#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"
#include "hello_worlds/Common/Models/Vertex.h"
#include "hello_worlds/Utils/ShellDisplay.h"

namespace hws {

  std::unique_ptr<hws::Model> ObjLoader::read(const std::string& path)
  {
    std::string mtl_path;
    auto pos = path.find_last_of("/\\");
    if(pos != std::string::npos)
      mtl_path = path.substr(0, pos);

    parseFromFile(path, mtl_path);

    if(isValid() == false)
      return nullptr;

    tinyobj::Attrib_t attributes = getAttrib();
    std::vector<tinyobj::Mesh_t> meshes = getMeshes();
    std::vector<tinyobj::Material_t> materials = getMaterials();

    std::unique_ptr<hws::Model> model = getModel(attributes, meshes, materials);
    if(model != nullptr)
      model->source_path_ = path;

    return model;
  }

  Vertex ObjLoader::getVertex(const tinyobj::Attrib_t& attribute,
                              const tinyobj::Index_t& indexes)
  {
    Vertex vtx;
    vtx.position_.x = (float)attribute.vertices[3lu * indexes.vertex_index];
    vtx.position_.y = (float)attribute.vertices[3lu * indexes.vertex_index + 1lu];
    vtx.position_.z = (float)attribute.vertices[3lu * indexes.vertex_index + 2lu];

    if(attribute.texcoords.empty())
    {
      int uv0_index = 2 * indexes.texcoord_index;
      int uv1_index = 2 * indexes.texcoord_index + 1;
      if(uv0_index >= 0 && uv1_index >= 0 &&
         (uv1_index < int(attribute.texcoords.size())))
      {
        vtx.uv_.x = (float)attribute.texcoords[uv0_index];
        vtx.uv_.y = (float)attribute.texcoords[uv1_index];
      }
      else
      {
        vtx.uv_.x = 0;
        vtx.uv_.y = 0;
      }
    }
    else
    {
      vtx.uv_.x = 0.5;
      vtx.uv_.y = 0.5;
    }

    return vtx;
  }

  std::unique_ptr<hws::Model> ObjLoader::getModel(const tinyobj::Attrib_t& attribute,
                                                  std::vector<tinyobj::Mesh_t>& meshes,
                                                  std::vector<tinyobj::Material_t>& materials)
  {
    auto model = std::make_unique<Model>(Model::create());

    for(const auto& tiny_mesh : meshes)
    {
      model->meshes_.emplace_back(Mesh::create());
      Mesh* hws_mesh = &(model->meshes_.back());
      hws_mesh->name_ = tiny_mesh.name;

      if(tiny_mesh.material_id >= 0)
      {
        tinyobj::Material_t& material = materials[tiny_mesh.material_id];
        hws_mesh->material_.diffuse_color_ = Color({(float)material.diffuse[0],
                                                    (float)material.diffuse[1],
                                                    (float)material.diffuse[2],
                                                    material.has_diffuse ? 1.f : 0.f});
        hws_mesh->material_.specular_color_ = Color({(float)material.specular[0],
                                                     (float)material.specular[1],
                                                     (float)material.specular[2],
                                                     material.has_specular ? 1.f : 0.f});
        hws_mesh->material_.shininess_ = (float)material.shininess;
        hws_mesh->material_.diffuse_texture_ = material.diffuse_texname;
        hws_mesh->material_.specular_texture_ = material.specular_texname;
        hws_mesh->material_.normal_texture_ = material.normal_texname;
      }

      size_t face_count = tiny_mesh.indices.size();

      for(size_t f = 0; f < face_count; f += 3)
      {
        int vtx_base_index = (int)hws_mesh->vertices_.size();

        tinyobj::Index_t v_0 = tiny_mesh.indices[f];
        Vertex vtx0 = getVertex(attribute, v_0);

        tinyobj::Index_t v_1 = tiny_mesh.indices[f + 1];
        Vertex vtx1 = getVertex(attribute, v_1);

        tinyobj::Index_t v_2 = tiny_mesh.indices[f + 2];
        Vertex vtx2 = getVertex(attribute, v_2);

        unsigned n0_index = tiny_mesh.indices[f].normal_index;
        unsigned n1_index = tiny_mesh.indices[f + 1].normal_index;
        unsigned n2_index = tiny_mesh.indices[f + 2].normal_index;

        bool has_normals = (attribute.normals.empty() == false);
        if(has_normals)
        {
          unsigned int max_index = 0;

          max_index = std::max(max_index, 3 * n0_index + 0);
          max_index = std::max(max_index, 3 * n0_index + 1);
          max_index = std::max(max_index, 3 * n0_index + 2);
          max_index = std::max(max_index, 3 * n1_index + 0);
          max_index = std::max(max_index, 3 * n1_index + 1);
          max_index = std::max(max_index, 3 * n1_index + 2);
          max_index = std::max(max_index, 3 * n2_index + 0);
          max_index = std::max(max_index, 3 * n2_index + 1);
          max_index = std::max(max_index, 3 * n2_index + 2);

          has_normals = (max_index < attribute.normals.size());
        }

        if(has_normals == false)
        {
          auto normal = glm::cross((vtx1.position_ - vtx0.position_), (vtx2.position_ - vtx0.position_));
          float len = glm::length(normal);
          // skip degenerate triangles
          if(len > FLT_EPSILON)
            normal = glm::normalize(normal);
          else
            normal = glm::vec3(0.f, 0.f, 0.f);

          vtx0.normal_ = normal;
          vtx1.normal_ = normal;
          vtx2.normal_ = normal;
        }
        else
        {
          vtx0.normal_.x = (float)attribute.normals[3 * n0_index + 0];
          vtx0.normal_.y = (float)attribute.normals[3 * n0_index + 1];
          vtx0.normal_.z = (float)attribute.normals[3 * n0_index + 2];
          vtx1.normal_.x = (float)attribute.normals[3 * n1_index + 0];
          vtx1.normal_.y = (float)attribute.normals[3 * n1_index + 1];
          vtx1.normal_.z = (float)attribute.normals[3 * n1_index + 2];
          vtx2.normal_.x = (float)attribute.normals[3 * n2_index + 0];
          vtx2.normal_.y = (float)attribute.normals[3 * n2_index + 1];
          vtx2.normal_.z = (float)attribute.normals[3 * n2_index + 2];
        }

        hws_mesh->vertices_.push_back(vtx0);
        hws_mesh->vertices_.push_back(vtx1);
        hws_mesh->vertices_.push_back(vtx2);
        hws_mesh->indices_.push_back(vtx_base_index);
        hws_mesh->indices_.push_back(vtx_base_index + 1);
        hws_mesh->indices_.push_back(vtx_base_index + 2);
      }
    }

    return model;
  }

} // namespace hws
#include "hello_worlds/Graphics/OpenGL/MeshHandle.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "glad/glad.h"
#include "hello_worlds/Common/Models/Color.h"
#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Vertex.h"
#include "hello_worlds/Graphics/OpenGL/Shaders/DefaultShader.h"
#include "hello_worlds/Graphics/OpenGL/Shaders/Shader.h"
#include "hello_worlds/Graphics/OpenGL/Texture2D.h"

namespace hws {

  MeshHandle::MeshHandle(const Mesh& mesh) : vertices(mesh.vertices_),
                                             indices(mesh.indices_)
  {
    setupMesh();
  }

  void MeshHandle::drawId(const Shader& shader, uint32_t model_id) const
  {
    // model_id *= 4'000'000;
    shader.setVec4("color", glm::vec4((uint8_t)(model_id >> 24) / 255.,
                                      (uint8_t)(model_id >> 16) / 255.,
                                      (uint8_t)(model_id >> 8) / 255.,
                                      (uint8_t)(model_id) / 255.));

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  void MeshHandle::draw(const Shader& shader) const
  {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  void MeshHandle::drawWithMaterial(const DefaultShader& shader, uint32_t model_id, unsigned int texture_pose_offset) const
  {
    auto material_it = materials.find(model_id);
    if(material_it == materials.end())
      return;

    bool use_normal_texture = false;
    bool use_diffuse_texture = false;
    bool use_specular_texture = false;

    for(unsigned int i = 0; i < material_it->second.textures.size(); i++)
    {
      glActiveTexture(GL_TEXTURE0 + texture_pose_offset + i);            // active proper texture unit before binding
      glBindTexture(GL_TEXTURE_2D, material_it->second.textures[i].id_); // now bind the texture

      switch(material_it->second.textures[i].type_)
      {
      case texture_diffuse:
        shader.setInt("texture_diffuse", (int)(texture_pose_offset + i));
        use_diffuse_texture = true;
        break;
      case texture_specular:
        shader.setInt("texture_specular", (int)(texture_pose_offset + i));
        use_specular_texture = true;
        break;
      case texture_normal:
        shader.setInt("texture_normal", (int)(texture_pose_offset + i));
        use_normal_texture = true;
        break;
      default:
        break;
      }
    }

    unsigned int nb_used = material_it->second.textures.size();

    if(use_diffuse_texture == false)
    {
      glActiveTexture(GL_TEXTURE0 + texture_pose_offset + nb_used);
      glBindTexture(GL_TEXTURE_2D, 0);
      shader.setInt("texture_diffuse", (int)(texture_pose_offset + nb_used));
      nb_used++;
    }

    if(use_specular_texture == false)
    {
      glActiveTexture(GL_TEXTURE0 + texture_pose_offset + nb_used);
      glBindTexture(GL_TEXTURE_2D, 0);
      shader.setInt("texture_specular", (int)(texture_pose_offset + nb_used));
      nb_used++;
    }

    if(use_normal_texture == false)
    {
      glActiveTexture(GL_TEXTURE0 + texture_pose_offset + nb_used);
      glBindTexture(GL_TEXTURE_2D, 0);
      shader.setInt("texture_normal", (int)(texture_pose_offset + nb_used));
      nb_used++;
    }

    MaterialUBO_t data;
    data.color = glm::vec4(material_it->second.color.r_,
                           material_it->second.color.g_,
                           material_it->second.color.b_,
                           (use_diffuse_texture ? 0. : material_it->second.color.a_));
    data.shininess = material_it->second.shininess;
    data.specular = use_specular_texture ? -1 : material_it->second.specular;
    data.use_normal = use_normal_texture ? 1. : 0.0f;
    data.padding = 0.;

    shader.setMaterial(&data);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
  }

  // initializes all the buffer objects/arrays
  void MeshHandle::setupMesh()
  {
    // create buffers/arrays
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (long)(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal_));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv_));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent_));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent_));
  }

} // namespace hws
#ifndef HWS_COLLADALOADER_H
#define HWS_COLLADALOADER_H

#include <glm/matrix.hpp>
#include <map>
#include <memory>
#include <string>
#include <tinyxml2.h>
#include <vector>

#include "hello_worlds/Common/Models/RawModel.h"

namespace hws {

  class ColladaLoader
  {
  public:
    std::unique_ptr<hws::RawModel_t> read(const std::string& path);

  private:
    bool getXmlDocument(const std::string& path, tinyxml2::XMLDocument& doc);
    void getScalingAndTransform(tinyxml2::XMLElement* root);
    std::map<std::string, Material> getMaterialLibrary(tinyxml2::XMLElement* root);
    std::map<std::string, RawMesh_t> getMeshLibrary(tinyxml2::XMLElement* root);

    std::vector<RawMesh_t> readSceneGeometries(tinyxml2::XMLElement* root, std::map<std::string, RawMesh_t>& meshes_library);
    void readNodeHierarchy(tinyxml2::XMLElement* node, std::map<std::string, RawMesh_t>& meshes_library, std::vector<RawMesh_t>& instances, const glm::mat4& parent_trans_mat);

    void readFloatArray(tinyxml2::XMLElement* source, std::vector<float>& float_array, int& component_stride);

    glm::mat3 tr_;
    float unit_meter_scaling_;
  };

} // namespace hws

#endif // HWS_COLLADALOADER_H
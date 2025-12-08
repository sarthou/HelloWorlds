#ifndef HWS_COLLADALOADER_H
#define HWS_COLLADALOADER_H

#include <glm/matrix.hpp>
#include <map>
#include <memory>
#include <string>
#include <tinyxml.h>
#include <vector>

#include "hello_worlds/Common/Models/Mesh.h"
#include "hello_worlds/Common/Models/Model.h"

namespace hws {

  class ColladaLoader
  {
  public:
    std::unique_ptr<hws::Model> read(const std::string& path);

  private:
    bool getXmlDocument(const std::string& path, TiXmlDocument& doc);
    void getScalingAndTransform(TiXmlElement* root);
    std::map<std::string, Material> getMaterialLibrary(TiXmlElement* root);
    std::map<std::string, Mesh> getMeshLibrary(TiXmlElement* root);

    std::vector<Mesh> readSceneGeometries(TiXmlElement* root, std::map<std::string, Mesh>& meshes_library);
    void readNodeHierarchy(TiXmlElement* node, std::map<std::string, Mesh>& meshes_library, std::vector<Mesh>& instances, const glm::mat4& parent_trans_mat);

    void readFloatArray(TiXmlElement* source, std::vector<float>& float_array, int& component_stride);

    glm::mat3 tr_;
    float unit_meter_scaling_;
  };

} // namespace hws

#endif // HWS_COLLADALOADER_H
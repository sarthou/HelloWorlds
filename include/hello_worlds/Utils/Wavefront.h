#ifndef HWS_WAVEFRONT_H
#define HWS_WAVEFRONT_H

#include <array>
#include <string>
#include <vector>

namespace hws {

  namespace wavefront {

    bool isWavefront(const std::string& cad_file);

    std::vector<std::array<double, 3>> getVertexes(const std::string& obj_path);

    std::string getMltFile(const std::string& obj_path);

    std::vector<std::string> getMltMaterials(const std::string& mlt_content);

    std::string getMaterialColor(const std::string& mlt_content, const std::string& material_name);

    std::string getMaterialTexture(const std::string& mlt_content, const std::string& material_name);

  } // namespace wavefront

} // namespace hws

#endif // HWS_WAVEFRONT_H
#ifndef HWS_GRAPHICS_OPENGL_TEXTURE2D_H
#define HWS_GRAPHICS_OPENGL_TEXTURE2D_H

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

typedef unsigned int GLenum; // NOLINT

namespace hws {

  enum TextureType_e
  {
    texture_diffuse,
    texture_specular,
    texture_normal,
    texture_height,
    texture_invalid
  };

  class Texture2D
  {
  public:
    unsigned int id_;
    TextureType_e type_;
    int width;
    int height;
    int nb_channels;
    GLenum internal_format;
    GLenum data_format;

    Texture2D(const std::string& path, TextureType_e type, bool gamma_correction = false, bool flip = false);
    Texture2D(const std::array<uint8_t, 4>& color, TextureType_e type);

    void bind(GLenum unit) const;

  private:
    void loadGreyAsRgb(unsigned char* data) const;
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_TEXTURE2D_H
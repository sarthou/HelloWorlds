#ifndef HWS_GRAPHICS_OPENGL_MSAASCREEN_H
#define HWS_GRAPHICS_OPENGL_MSAASCREEN_H

#include <array>

namespace hws {

  class MsaaScreen
  {
    static std::array<float, 24> screen_vertices;

  public:
    void init(unsigned int width, unsigned int height);
    void reinit(unsigned int width, unsigned int height);

    void initBuffers(unsigned int msaa_samples);
    void bindFrameBuffer() const;
    void generateColorTexture() const;

    void renderQuad() const;

    unsigned int getFrameBuffer() const { return msaa_framebuffer_; }

    void draw() const;

    int width_;
    int height_;

  private:
    unsigned int texture_color_buffer_ms_;
    unsigned int screen_texture_;
    unsigned int msaa_framebuffer_;
    unsigned int msaa_renderbuffer_;
    unsigned int intermediate_framebuffer_;
    unsigned int msaa_samples_;

    unsigned int screen_vao_;
    unsigned int screen_vbo_;

    bool init_ = false;
  };

} // namespace hws

#endif // HWS_GRAPHICS_OPENGL_MSAASCREEN_H
#ifndef HWS_GEOMETRYBUFFER_H
#define HWS_GEOMETRYBUFFER_H

namespace hws {

  class GeometryBuffer
  {
  public:
    void init(unsigned int width, unsigned int height);
    void reinit(unsigned int width, unsigned int height);

    void bind() const;
    void unbind() const;

    unsigned int getNormalTexture() const { return g_normal_; }
    unsigned int getDepthTexture() const { return g_depth_; }

  private:
    unsigned int fbo_ = 0;
    unsigned int g_normal_ = 0;
    unsigned int g_depth_ = 0;

    unsigned int width_;
    unsigned int height_;
  };

} // namespace hws

#endif // HWS_GEOMETRYBUFFER_H
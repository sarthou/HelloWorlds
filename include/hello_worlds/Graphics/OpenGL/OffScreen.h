#ifndef HWS_OFFSCREEN_H
#define HWS_OFFSCREEN_H

#include <cstdint>

namespace hws {

  class OffScreen
  {
  public:
    OffScreen() = default;
    ~OffScreen();

    void init(unsigned int width, unsigned int height);
    void bindFrameBuffer() const;

    void getImage(uint32_t** data) const;

  private:
    int width_;
    int height_;

    unsigned int framebuffer_;
    unsigned int colorbuffer_;
    unsigned int depthbuffer_;
  };

} // namespace hws

#endif // HWS_OFFSCREEN_H
#ifndef HWS_SSAO_MANAGER_H
#define HWS_SSAO_MANAGER_H

#include <glm/vec3.hpp>
#include <vector>

namespace hws {

  class SSAOManager
  {
  public:
    void init(unsigned int width, unsigned int height);
    void reinit(unsigned int width, unsigned int height);

    // Getters for the shaders
    unsigned int getNoiseTexture() const { return noise_texture_; }
    const std::vector<glm::vec3>& getKernel() const { return ssao_kernel_; }

    // FBO handles for rendering
    unsigned int getSSAOFrameBuffer() const { return ssao_fbo_; }
    unsigned int getBlurFrameBuffer() const { return blur_fbo_; }

    // Result textures
    unsigned int getSSAOTexture() const { return ssao_color_buffer_; }
    unsigned int getBlurredSSAOTexture() const { return ssao_blur_buffer_; }

  private:
    unsigned int ssao_fbo_;
    unsigned int blur_fbo_;

    unsigned int ssao_color_buffer_;
    unsigned int ssao_blur_buffer_;

    unsigned int noise_texture_;

    std::vector<glm::vec3> ssao_kernel_;
    float lerp(float a, float b, float f) { return a + f * (b - a); }
  };

} // namespace hws

#endif // HWS_SSAO_MANAGER_H
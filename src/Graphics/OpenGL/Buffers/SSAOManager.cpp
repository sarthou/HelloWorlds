#include "hello_worlds/Graphics/OpenGL/Buffers/SSAOManager.h"

#include <glm/gtc/packing.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>
#include <vector>

#include "glad/glad.h"

namespace hws {

  void SSAOManager::init(unsigned int width, unsigned int height)
  {
    // SSAO textures are half-resolution
    unsigned int ssao_w = width / 2;
    unsigned int ssao_h = height / 2;

    // Generate Sample Kernel
    std::uniform_real_distribution<float> random_floats(0.0, 1.0);
    std::default_random_engine generator;

    unsigned int kernel_size = 32;
    ssao_kernel_.clear();
    ssao_kernel_.reserve(kernel_size);
    for(unsigned int i = 0; i < kernel_size; ++i)
    {
      glm::vec3 sample(
        random_floats(generator) * 2.0 - 1.0,
        random_floats(generator) * 2.0 - 1.0,
        random_floats(generator) // Hemisphere: Z is 0 to 1
      );
      sample = glm::normalize(sample);
      sample *= random_floats(generator);

      // Scale samples so they're more clustered near origin
      float scale = (float)i / (float)kernel_size;
      scale = lerp(0.1f, 1.0f, scale * scale);
      ssao_kernel_.emplace_back(sample * scale, 0.);
    }

    // Generate Noise Texture (4x4)
    std::vector<glm::vec3> ssao_noise;
    for(unsigned int i = 0; i < 16; i++)
    {
      glm::vec3 noise(
        random_floats(generator) * 2.0 - 1.0,
        random_floats(generator) * 2.0 - 1.0,
        0.0f);
      ssao_noise.push_back(noise);
    }

    glGenTextures(1, &noise_texture_);
    glBindTexture(GL_TEXTURE_2D, noise_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssao_noise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create SSAO FBOs (SSAO result is a single channel float)
    glGenFramebuffers(1, &ssao_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo_);
    glGenTextures(1, &ssao_color_buffer_);
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int)ssao_w, (int)ssao_h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color_buffer_, 0);

    // Create Blur FBO
    glGenFramebuffers(1, &blur_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo_);
    glGenTextures(1, &ssao_blur_buffer_);
    glBindTexture(GL_TEXTURE_2D, ssao_blur_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int)ssao_w, (int)ssao_h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur_buffer_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SSAOManager::reinit(unsigned int width, unsigned int height) const
  {
    // SSAO textures are half-resolution
    unsigned int ssao_w = width / 2;
    unsigned int ssao_h = height / 2;

    // Resize the raw SSAO result buffer
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int)ssao_w, (int)ssao_h, 0, GL_RED, GL_FLOAT, nullptr);

    // Resize the blurred SSAO result buffer
    glBindTexture(GL_TEXTURE_2D, ssao_blur_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int)ssao_w, (int)ssao_h, 0, GL_RED, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

} // namespace hws
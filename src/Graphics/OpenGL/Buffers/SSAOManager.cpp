#include "hello_worlds/Graphics/OpenGL/Buffers/SSAOManager.h"

#include <glm/gtc/packing.hpp>
#include <glm/vec3.hpp>
#include <random>
#include <vector>

#include "glad/glad.h"

namespace hws {

  void SSAOManager::init(unsigned int width, unsigned int height)
  {
    // Generate Sample Kernel
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for(unsigned int i = 0; i < 64; ++i)
    {
      glm::vec3 sample(
        randomFloats(generator) * 2.0 - 1.0,
        randomFloats(generator) * 2.0 - 1.0,
        randomFloats(generator) // Hemisphere: Z is 0 to 1
      );
      sample = glm::normalize(sample);
      sample *= randomFloats(generator);

      // Scale samples so they're more clustered near origin
      float scale = (float)i / 64.0f;
      scale = lerp(0.1f, 1.0f, scale * scale);
      ssao_kernel_.push_back(sample * scale);
    }

    // Generate Noise Texture (4x4)
    std::vector<glm::vec3> ssaoNoise;
    for(unsigned int i = 0; i < 16; i++)
    {
      glm::vec3 noise(
        randomFloats(generator) * 2.0 - 1.0,
        randomFloats(generator) * 2.0 - 1.0,
        0.0f);
      ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noise_texture_);
    glBindTexture(GL_TEXTURE_2D, noise_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Create SSAO FBOs (SSAO result is a single channel float)
    glGenFramebuffers(1, &ssao_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo_);
    glGenTextures(1, &ssao_color_buffer_);
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color_buffer_, 0);

    // Create Blur FBO
    glGenFramebuffers(1, &blur_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo_);
    glGenTextures(1, &ssao_blur_buffer_);
    glBindTexture(GL_TEXTURE_2D, ssao_blur_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur_buffer_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SSAOManager::reinit(unsigned int width, unsigned int height)
  {
    // Resize the raw SSAO result buffer
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);

    // Resize the blurred SSAO result buffer
    glBindTexture(GL_TEXTURE_2D, ssao_blur_buffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

} // namespace hws
#version 420 core
layout (location = 0) out vec3 gNormal; // Matches the GeometryBuffer setup

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

struct MaterialData {
  vec4  color;
  float shininess;
  float specular;
  float use_normal;
  float padding; 
};

layout (std140, binding = 1) uniform MaterialBlock {
  MaterialData material;
};

uniform sampler2D texture_normal;

void main()
{    
  // Store View-Space Normal
  vec3 n = normalize(Normal);
  if(material.use_normal > 0.5) {
    // Sample normal map and transform to View-Space via TBN
    n = texture(texture_normal, TexCoords).rgb;
    n = normalize(n * 2.0 - 1.0);
    n = normalize(TBN * n);
  }
  
  // Write to the normal texture
  gNormal = n;

  // Depth is written automatically to the Depth Texture 
  // attached to the FBO's GL_DEPTH_ATTACHMENT.
}
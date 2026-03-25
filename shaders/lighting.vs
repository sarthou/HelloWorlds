#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent; 

layout (location = 0) out vec3 FragPos;  
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoords;
layout (location = 3) out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  // World space position for lighting and shadows
  FragPos = vec3(model * vec4(aPos, 1.0));
  
  // Normal matrix to handle non-uniform scaling
  mat3 normalMatrix = transpose(inverse(mat3(model)));
  Normal = normalMatrix * aNormal;
  
  TexCoords = aTexCoords;

  // TBN Matrix for Normal Mapping
  vec3 T = normalize(normalMatrix * aTangent);
  vec3 B = normalize(normalMatrix * aBitangent);
  vec3 N = normalize(normalMatrix * aNormal);
  TBN = mat3(T, B, N);

  gl_Position = projection * view * vec4(FragPos, 1.0);
}
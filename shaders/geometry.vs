#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;   // View-space position
out vec2 TexCoords;
out vec3 Normal;    // View-space normal
out mat3 TBN;       // View-space TBN for normal mapping

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  // Calculate View-Space position
  vec4 viewPos = view * model * vec4(aPos, 1.0);
  FragPos = viewPos.xyz; 
  TexCoords = aTexCoords;

  // Calculate View-Space Normal Matrix
  // We need the inverse transpose of the ModelView matrix 3x3
  mat3 normalMatrix = transpose(inverse(mat3(view * model)));
  
  Normal = normalize(normalMatrix * aNormal);

  // View-Space TBN (To get normal maps to affect SSAO)
  vec3 T = normalize(normalMatrix * aTangent);
  vec3 N = normalize(normalMatrix * aNormal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  TBN = mat3(T, B, N);

  gl_Position = projection * viewPos;
}
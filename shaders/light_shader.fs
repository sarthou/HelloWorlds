#version 420 core
out vec4 FragColor;

// --- INPUTS ---
layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in mat3 TBN;

struct MaterialData {
  vec4  color;      // 16 bytes
  float shininess;  // 4 bytes
  float specular;   // 4 bytes
  float use_normal; // 4 bytes
  float padding;    // 4 bytes 
};

layout (std140, binding = 1) uniform MaterialBlock {
    MaterialData material;
};

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

struct DirLightData {
  vec4 direction;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  int cascade_count;
  float far_plane;
  vec2 texel_size; 
};

layout (std140, binding = 2) uniform DirLightBlock {
    DirLightData dir_light;
};

struct PointLightData {
  vec4 position;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 attenuation;
  float far_plane;
  float on_off;
  float padding1;
  float padding2;
};

#define NR_POINT_LIGHTS 20
layout (std140, binding = 3) uniform LightBlock {
    PointLightData point_lights[NR_POINT_LIGHTS];
};

uniform samplerCube point_lights_depth_maps[NR_POINT_LIGHTS];
uniform float nb_point_lights;
uniform float use_ambient_shadows;
uniform float use_point_shadows;
uniform vec3 view_pose;
uniform mat4 view;

layout (std140, binding = 0) uniform LightSpaceMatrices {
    mat4 light_space_matrices[5];
};
uniform sampler2DArray shadow_maps;
uniform float cascade_planes_distances[5];

const float PI = 3.14159265359;

// --- PROTOTYPES ---
vec3 CalcPBR(vec3 L, vec3 radiance, vec3 N, vec3 V, float NdotV, float NdotL, vec3 albedo, float roughness, float metallic, float shadow);
float directionalShadowCalculation(vec3 fragPosWorldSpace, vec3 normal, float distToCamera, float NdotL);
float pointShadowCalculation(int index, vec3 fragPosWorldSpace, vec3 normal, float NdotL, float distToLight, float distToCamera);

// --- PBR MATH ---

vec3 ACESFilm(vec3 x) {
  return clamp((x*(2.51*x+0.03))/(x*(2.43*x+0.59)+0.14), 0.0, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a2 = roughness * roughness * roughness * roughness;
  float NdotH = max(dot(N, H), 0.0);
  float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
  return a2 / (PI * denom * denom + 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float k) {
  return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;
  return GeometrySchlickGGX(NdotV, k) * GeometrySchlickGGX(NdotL, k);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --- MAIN ---

void main()
{
    // 1. Setup Vectors
    vec3 N = normalize(Normal);
    if(material.use_normal > 0.5) {
        N = texture(texture_normal, TexCoords).rgb * 2.0 - 1.0;
        N = normalize(TBN * N);
    }
    vec3 V = normalize(view_pose - FragPos);
    float NdotV = max(dot(N, V), 0.0);
    float distToCamera = length(view_pose - FragPos);

    // 2. Material Logic
    vec4 baseColor = (material.color.w == 0) ? texture(texture_diffuse, TexCoords) : material.color;
    vec3 albedo = baseColor.rgb; // Assuming input is already linearized or sRGB-handled by sampler
    float roughness = clamp(1.0 - sqrt(material.shininess / 1000.0), 0.05, 1.0);
    
    // 3. Lighting Accumulation
    vec3 totalAmbient = vec3(0.0);
    vec3 totalDirect = vec3(0.0);

    // --- Directional Light Pass ---
    vec3 L_dir = -normalize(dir_light.direction.xyz);
    float NdotL_dir = max(dot(N, L_dir), 0.0);
    
    totalAmbient += dir_light.ambient.rgb * albedo;

    if (NdotL_dir > 0.001) {
        float shadowDir = directionalShadowCalculation(FragPos, N, distToCamera, NdotL_dir);
        totalDirect += CalcPBR(L_dir, dir_light.diffuse.rgb, N, V, NdotV, NdotL_dir, albedo, roughness, 0.0, shadowDir);
    }

    // --- Point Light Loop ---
    for(int i = 0; i < int(nb_point_lights); i++)
    {
      if(point_lights[i].on_off < 0.5) continue;

      vec3 L_unnormalized = point_lights[i].position.xyz - FragPos;
      float dist = length(L_unnormalized);
      if(dist > point_lights[i].far_plane) continue;

      vec3 L = L_unnormalized / dist;
      float NdotL = max(dot(N, L), 0.0);
      // if (NdotL < 0.001) continue; // Skip shadow/PBR if light is behind or parallel

      float attenuation = 1.0 / (point_lights[i].attenuation.x + 
                                point_lights[i].attenuation.y * dist + 
                                point_lights[i].attenuation.z * (dist * dist));
      if (attenuation < 0.001) continue;

      totalAmbient += point_lights[i].ambient.rgb * albedo * attenuation;

      if (NdotL > 0.0)
      {
        float shadowPoint = pointShadowCalculation(i, FragPos, N, NdotL, dist, distToCamera);
        vec3 radiance = point_lights[i].diffuse.rgb * attenuation;
        totalDirect += CalcPBR(L, radiance, N, V, NdotV, NdotL, albedo, roughness, 0.0, shadowPoint); 
      }      
    }

    // 4. Composition & Tone Mapping
    vec3 color = totalAmbient + totalDirect;
    color = vec3(1.0) - exp(-color * 1.0); // Simple Exposure
    color = ACESFilm(color);
    
    FragColor = vec4(color, baseColor.a);
}

// --- OPTIMIZED PBR CALCULATION ---
vec3 CalcPBR(vec3 L, vec3 radiance, vec3 N, vec3 V, float NdotV, float NdotL, vec3 albedo, float roughness, float metallic, float shadow) {
    if (shadow > 0.99) return vec3(0.0);

    vec3 H = normalize(V + L);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(NdotV, NdotL, roughness);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 specular = (NDF * G * F) / (4.0 * NdotV * NdotL + 0.0001);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);     

    return (1.0 - shadow) * (kD * albedo / PI + specular) * radiance * NdotL;  
}

// --- SHADOW LOD LOGIC ---

float directionalShadowCalculation(vec3 fragPosWorldSpace, vec3 normal, float distToCamera, float NdotL)
{
  if(use_ambient_shadows < 0.5) return 0.0;

  float depth_value = abs((view * vec4(fragPosWorldSpace, 1.0)).z);
  int layer = dir_light.cascade_count;
  for (int i = 0; i < dir_light.cascade_count; ++i) {
      if (depth_value < cascade_planes_distances[i]) { layer = i; break; }
  }

  vec4 frag_pos_light_space = light_space_matrices[layer] * vec4(fragPosWorldSpace, 1.0);
  vec3 proj_coords = (frag_pos_light_space.xyz / frag_pos_light_space.w) * 0.5 + 0.5;
  if (proj_coords.z > 1.0) return 0.0;

  float bias = max(0.05 * (1.0 - NdotL), 0.005);
  float current_cascade_dist = (layer == dir_light.cascade_count) ? dir_light.far_plane * 0.5 : cascade_planes_distances[layer] * ((layer == 0) ? 4. : 1.5) ;
  bias *= 1.0 / (current_cascade_dist);

  // Directional Shadow LOD: Only do 3x3 PCF if close to camera
  if (distToCamera > 20.0) {
      float pcf_depth = texture(shadow_maps, vec3(proj_coords.xy, layer)).r;
      return (proj_coords.z - bias) > pcf_depth ? 1.0 : 0.0;
  }

  float shadow = 0.0;
  for(int x = -1; x <= 1; ++x) {
      for(int y = -1; y <= 1; ++y) {
          float pcf_depth = texture(shadow_maps, vec3(proj_coords.xy + vec2(x, y) * dir_light.texel_size, layer)).r;
          shadow += (proj_coords.z - bias) > pcf_depth ? 1.0 : 0.0;       
      }    
  }
  return shadow / 9.0;
}

vec3 sampleOffsets[20] = vec3[](
  vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
  vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
  vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
  vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
  vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float pointShadowCalculation(int index, vec3 fragPosWorldSpace, vec3 normal, float NdotL, float distToLight, float distToCamera)
{
  if(use_point_shadows < 0.5) return 0.0;
  
  // 1. DYNAMIC BIAS
  // We scale the bias based on the angle. Grazing angles need more bias.
  float bias = max(0.05 * (1.0 - NdotL), 0.005); 

  // 2. NORMAL OFFSET
  // This is the "magic" fix for the chest grid. We move the lookup 
  // slightly away from the surface along the normal.
  // 0.015 is a good starting point for a small robot.
  vec3 offsetFragPos = fragPosWorldSpace + normal * 0.015;
  vec3 fragToLight = offsetFragPos - point_lights[index].position.xyz;

  // Point Shadow LOD
  int samples = (distToCamera < 6.0) ? 20 : (distToCamera < 12.0 ? 8 : 1);

  if (samples == 1) {
      float closestDepth = texture(point_lights_depth_maps[index], fragToLight).r;
      // Convert normalized [0,1] depth back to world units
      return (distToLight - bias > closestDepth * point_lights[index].far_plane) ? 1.0 : 0.0;
  }

  float shadow = 0.0;
  // Use a disk radius that scales slightly with distance to keep it soft
  float diskRadius = (1.0 + (distToLight / point_lights[index].far_plane)) / 50.0; 

  for(int i = 0; i < samples; ++i) {
      // Use the offset sampleOffsets with our dynamic diskRadius
      float closestDepth = texture(point_lights_depth_maps[index], fragToLight + sampleOffsets[i] * diskRadius).r;
      if(distToLight - bias > closestDepth * point_lights[index].far_plane) shadow += 1.0;
  }
  return shadow / float(samples);
}
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
    float padding;    // 4 bytes (to align to 16-byte chunks)
};

layout (std140, binding = 1) uniform MaterialBlock {
    MaterialData material;
};

// Textures stay as regular uniforms
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

struct DirLightData {
  vec4 direction;
  vec4 ambient;
  vec4 diffuse;  // Used as Light Color
  vec4 specular; // Ignored in PBR (PBR calculates spec from roughness)
  int cascade_count;
  float far_plane;
  vec2 texel_size; // aligned to a 16-byte
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
    float padding2; // Padding to make the struct 96 bytes (multiple of 16)
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
    mat4 light_space_matrices[16];
};
uniform sampler2DArray shadow_maps;
uniform float cascade_planes_distances[16];

// --- PROTOTYPES ---
vec3 CalcPBR(vec3 lightDir, vec3 lightColor, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic, float shadow);
float ambiantShadowCalculation(vec3 fragPosWorldSpace, vec3 normal);
float pointShadowCalculation(int index, vec3 fragPosWorldSpace, float currentDepth);

const float PI = 3.14159265359;

// --- PBR FUNCTIONS (Blender Equivalent) ---

vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

// 1. Distribution (GGX) - Controls the size/shape of the highlight
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / max(denom, 0.0000001); // Prevent divide by zero
}

// 2. Geometry (Smith-Schlick) - Controls self-shadowing of microfacets
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// 3. Fresnel (Schlick) - Controls reflection intensity at angles
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --- MAIN ---

void main()
{
    // 1. Normal Mapping
    vec3 N = normalize(Normal);
    if(material.use_normal > 0) {
        N = texture(texture_normal, TexCoords).rgb;
        N = N * 2.0 - 1.0;   
        N = normalize(TBN * N);
    }
    vec3 V = normalize(view_pose - FragPos);

    // 2. Material Logic
    vec4 baseColor = material.color;
    if(material.color.w == 0) {
        baseColor = texture(texture_diffuse, TexCoords);
    }
    // IMPORTANT: Linearize the input color
    //vec3 albedo = pow(baseColor.rgb, vec3(2.2)); // not performed 
    vec3 albedo = pow(baseColor.rgb, vec3(1.));


    // Convert Ns to Roughness
    float roughness = clamp(1.0 - sqrt(material.shininess / 1000.0), 0.05, 1.0);
    
    // Use your actual Ks (specular) value
    float specIntensity = material.specular; 
    if(material.specular <= 0) {
      specIntensity = texture(texture_specular, TexCoords).r;
    }

    // 3. Lighting Accumulation
    vec3 totalAmbient = vec3(0.0);
    vec3 totalDirect = vec3(0.0);

    // --- Directional Light ---
    float shadowDir = ambiantShadowCalculation(FragPos, N);
    
    // Restore your actual ambient uniform
    totalAmbient += dir_light.ambient.rgb * albedo;
    
    // Direct PBR contribution
    totalDirect += CalcPBR(-normalize(dir_light.direction.xyz), dir_light.diffuse.rgb, N, V, albedo, roughness, 0.0, shadowDir);

    // --- Point Lights ---
    for(int i = 0; i < int(nb_point_lights); i++)
    {
      if(point_lights[i].on_off < 0.5) continue;

      vec3 L_unnormalized = point_lights[i].position.xyz - FragPos;
      float dist = length(L_unnormalized);
      
      // 1. RANGE CHECK: If the pixel is further than the light's reach, skip.
      if(dist > point_lights[i].far_plane) continue;

      float attenuation = 1.0 / (point_lights[i].attenuation.x + 
                                point_lights[i].attenuation.y * dist + 
                                point_lights[i].attenuation.z * (dist * dist));

      // 2. ATTENUATION CHECK: If light is too dim to see, skip.
      if(attenuation < 0.001) continue;

      // 3. SHADOW CHECK: Only do the 20 samples if the light actually hits the pixel.
      float shadowPoint = pointShadowCalculation(i, FragPos, dist);
      
      vec3 L = L_unnormalized / dist; // Normalize using the distance we already calculated
      
      totalAmbient += point_lights[i].ambient.rgb * albedo * attenuation;
      vec3 radiance = point_lights[i].diffuse.rgb * attenuation;
      totalDirect += CalcPBR(L, radiance, N, V, albedo, roughness, 0.0, shadowPoint); 
    }

    // 4. Final Combination
    // Ambient is NOT shadowed (standard), Direct light IS shadowed inside CalcPBR
    vec3 color = totalAmbient + totalDirect;

    // 5. Tonemapping & Gamma (The "Brightener")
    // If the scene is too dark, increase the exposure constant (1.0) here
    float exposure = 1.; 
    color = vec3(1.0) - exp(-color * exposure);
    
    color = ACESFilm(color);
    // Convert back to Gamma Space
    //FragColor = vec4(pow(color, vec3(1.0/2.2)), baseColor.a);
    FragColor = vec4(color, baseColor.a);
}

// --- GENERIC PBR CALCULATION ---
vec3 CalcPBR(vec3 L, vec3 radiance, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic, float shadow) {
    if (shadow > 0.99) return vec3(0.0); // Optimization for full shadow

    vec3 H = normalize(V + L);
    
    // Calculate F0 (Surface reflection at 0 degrees)
    // 0.04 for dielectric, Albedo color for metal
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // kD is the remaining energy (1 - kS)
    vec3 kD = vec3(1.0) - kS;
    // Multiply kD by inverse metalness (metals have no diffuse light)
    kD *= 1.0 - metallic;	  

    float NdotL = max(dot(N, L), 0.0);

    // Final Radiance combine
    // (Diffuse + Specular) * Light Color * Angle
    return (1.0 - shadow) * (kD * albedo + specular) * radiance * NdotL;  
}

// --- SHADOW FUNCTIONS (UNCHANGED LOGIC) ---
vec3 sampleOffsetDirections[20] = vec3[]
(
  vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
  vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
  vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
  vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
  vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float pointShadowCalculation(int index, vec3 fragPosWorldSpace, float distToLight)
{
    if(use_point_shadows == 0) return 0.0;
    
    vec3 fragToLight = fragPosWorldSpace - point_lights[index].position.xyz;
    float currentDepth = distToLight;
    float shadow = 0.0;
    float bias = 0.04; 

    // Determine sample count based on distance to CAMERA
    float distToCamera = length(view_pose - fragPosWorldSpace);
    int samples;
    
    if(distToCamera < 5.0) {
        samples = 20; // High quality close up
    } else if(distToCamera < 15.0) {
        samples = 8;  // Medium quality
    } else {
        samples = 2;  // Low quality background
    }

    // Optimization: If the shadow is too far away, just do a single sample
    if (samples == 2) {
        float closestDepth = texture(point_lights_depth_maps[index], fragToLight).r;
        closestDepth *= point_lights[index].far_plane;
        return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    }

    // Otherwise, run the PCF loop
    float diskRadius = 0.002;
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(point_lights_depth_maps[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= point_lights[index].far_plane;
        if(currentDepth - bias > closestDepth) shadow += 1.0;
    }
    
    return shadow / float(samples);
}

float ambiantShadowCalculation(vec3 fragPosWorldSpace, vec3 normal)
{
  if(use_ambient_shadows == 0) return 0.0;
  
  float depth_value = abs((view * vec4(fragPosWorldSpace, 1.0)).z);

  int layer = dir_light.cascade_count;
  for (int i = 0; i < dir_light.cascade_count; ++i)
  {
    if (depth_value < cascade_planes_distances[i])
    {
      layer = i;
      break;
    }
  }

  vec4 frag_pos_light_space = light_space_matrices[layer] * vec4(fragPosWorldSpace, 1.0);
  vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
  proj_coords = proj_coords * 0.5 + 0.5;

  if (proj_coords.z > 1.0) return 0.0;

  float bias = max(0.05 * (1.0 - dot(normal, dir_light.direction.xyz)), 0.005);
  float current_cascade_dist = (layer == dir_light.cascade_count) ? dir_light.far_plane : cascade_planes_distances[layer];
  bias *= 1.0 / (current_cascade_dist * 7.0);

  float shadow = 0.0;

  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcf_depth = texture(shadow_maps, vec3(proj_coords.xy + vec2(x, y) * dir_light.texel_size, layer)).r;
      shadow += (proj_coords.z - bias) > pcf_depth ? 1.0 : 0.0;       
    }    
  }
  shadow /= 9.0;
  return shadow;
}
#version 420 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D texNoise;

#define KERNEL_SIZE 32

layout (std140, binding = 5) uniform SamplesBlock {
    vec4 samples[KERNEL_SIZE];
};

//uniform vec3 samples[KERNEL_SIZE];
uniform mat4 projection;
uniform vec2 noiseScale;
uniform float radius = 0.5;
uniform float bias = 0.025;

void main()
{
    // 1. Reconstruct View-Space Position from Depth
    float z = texture(gDepth, TexCoords).r;
    if(z > 100.)
    {
        FragColor = 1.0;
        return;
    }
    mat4 inv_projection = inverse(projection);
    // Standard transformation from [0,1] depth to View-Space Z
    // (Note: This assumes a standard perspective projection)
    vec4 clipSpacePos = vec4(TexCoords * 2.0 - 1.0, z * 2.0 - 1.0, 1.0);
    vec4 viewSpacePos = inv_projection * clipSpacePos;
    vec3 fragPos = viewSpacePos.xyz / viewSpacePos.w;

    // 2. Get Normal and Noise
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    // 3. Create TBN matrix to align our kernel with the normal
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // 4. Calculate Occlusion Factor
    float occlusion = 0.0;
    for(int i = 0; i < KERNEL_SIZE; ++i)
    {
        // Get sample position in View-Space
        vec3 samplePos = TBN * samples[i].xyz; 
        samplePos = fragPos + samplePos * radius; 
        
        // Project sample back to screen UVs to check its depth
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Get the actual depth at that screen position
        float sampleDepth = texture(gDepth, offset.xy).r;
        // Reconstruct that neighbor's View-Z
        vec4 neighborClip = vec4(offset.xy * 2.0 - 1.0, sampleDepth * 2.0 - 1.0, 1.0);
        vec4 neighborView = inv_projection * neighborClip;
        float neighborZ = neighborView.z / neighborView.w;

        // Range check: Prevents shadows from "bleeding" onto objects far behind
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - neighborZ));
        occlusion += (neighborZ >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    
    // Normalize and invert (0 = occluded, 1 = open)
    FragColor = 1.0 - (occlusion / KERNEL_SIZE);
}
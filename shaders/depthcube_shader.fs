#version 400 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // Direct distance calculation
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // Map to [0, 1] range
    gl_FragDepth = lightDistance / far_plane;
}
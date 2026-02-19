#version 420 core
layout (triangles, invocations = 6) in; // Parallelize 6 times
layout (triangle_strip, max_vertices = 3) out;

layout (std140, binding = 4) uniform ShadowMatrices
{
    mat4 shadow_matrices[6];
};

out vec4 FragPos; 

void main()
{
    // gl_InvocationID tells us which face (0-5) this instance is for
    gl_Layer = gl_InvocationID; 
    
    for(int i = 0; i < 3; ++i)
    {
        FragPos = gl_in[i].gl_Position;
        // Apply the matrix specific to this invocation
        gl_Position = shadow_matrices[gl_InvocationID] * FragPos;
        EmitVertex();
    }    
    EndPrimitive();
}
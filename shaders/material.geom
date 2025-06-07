#version 460 core

layout(points) in;                // Input primitive type: points
layout(triangle_strip, max_vertices = 12) out; // Output primitive: quad

out vec3 FragPos;                 // Fragment position for the fragment shader
out vec3 FragNormal;              // Normal vector for lighting

// Uniforms
uniform sampler2D heightMap;       // Height map texture
uniform int resolution;
uniform float quadSize;
uniform mat4 view;                 // View matrix
uniform mat4 projection;           // Projection matrix
uniform mat4 model;                // Model matrix (optional for positioning the entire set of quads)

const float eps = 1e-6;

// Helper function to emit a vertex with position and normal
void EmitVertexWithNormal(vec3 position, vec3 normal)
{
    FragPos = (model * vec4(position, 1.0)).xyz;
    FragNormal = normalize((model * vec4(normal, 0.0)).xyz);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    EmitVertex();
}

void main()
{
    // Sample height from the height map based on 2D point (TexCoords)
    //vec2 coords = TexCoords[gl_InvocationID];
    vec2 coords = gl_in[0].gl_Position.xy;
    float x = coords.x;
    float z = coords.y;    

    float myHeight = texture(heightMap, coords).r;
    float xNextHeight = texture(heightMap, coords + vec2(quadSize, 0)).r;
    float zNextHeight = texture(heightMap, coords + vec2(0, quadSize)).r;
    float xzNextHeight = texture(heightMap, coords + vec2(quadSize, quadSize)).r;

    // Define the four corners of the quad in the XZ plane, with Y as the height

    vec3 me = vec3(x, myHeight, z);
    vec3 xNext = vec3(x + quadSize, xNextHeight, z);
    vec3 zNext = vec3(x, zNextHeight, z + quadSize);
    vec3 xzNext = vec3(x + quadSize, xzNextHeight, z + quadSize);

    // Handle additional faces
    if (x == 0.0 && z != 1.0)
    {
        vec3 sideNormal = vec3(-1, 0, 0);
        EmitVertexWithNormal(vec3(me.x, 0, me.z), sideNormal);   
        EmitVertexWithNormal(me, sideNormal);
        EmitVertexWithNormal(vec3(zNext.x, 0, zNext.z), sideNormal);
        EmitVertexWithNormal(zNext, sideNormal);     
    }
    else if (x >= 1.0 - eps && z != 1.0)
    {
        vec3 sideNormal = vec3(1, 0, 0);
        EmitVertexWithNormal(me, sideNormal);
        EmitVertexWithNormal(vec3(me.x, 0, me.z), sideNormal);   
        EmitVertexWithNormal(zNext, sideNormal);
        EmitVertexWithNormal(vec3(zNext.x, 0, zNext.z), sideNormal);        
    }

    if (z == 0.0 && x != 1.0)
    {
        vec3 sideNormal = vec3(0, 0, 1); // TODO
        EmitVertexWithNormal(vec3(me.x, 0, me.z), sideNormal);
        EmitVertexWithNormal(vec3(xNext.x, 0, xNext.z), sideNormal);
        EmitVertexWithNormal(me, sideNormal);
        EmitVertexWithNormal(xNext, sideNormal);
        
    }
    else if (z >= 1.0 - eps && x != 1.0)
    {
        vec3 sideNormal = vec3(0, 0, 1);
        EmitVertexWithNormal(me, sideNormal);
        EmitVertexWithNormal(xNext, sideNormal);        
        EmitVertexWithNormal(vec3(me.x, 0, me.z), sideNormal);
        EmitVertexWithNormal(vec3(xNext.x, 0, xNext.z), sideNormal);
    }

    // Handle edges
    if (x == 1.0 || z == 1.0)
        return;

    vec3 normal = normalize(cross((zNext-me), (xNext-me)));

    // Emit the quad
    EmitVertexWithNormal(me, normal);
    EmitVertexWithNormal(xNext, normal);
    EmitVertexWithNormal(zNext, normal);   
    EmitVertexWithNormal(xzNext, normal);

    EndPrimitive();
}

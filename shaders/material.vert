#version 460 core

layout(location = 0) in vec2 aPos;  // 2D position

void main()
{
    // gl_Position will be computed in the geometry shader
    gl_Position = vec4(aPos, 0.0, 1.0); // Pass a dummy position
}

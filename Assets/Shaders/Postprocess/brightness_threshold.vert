#version 420 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec2 OurTexCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    OurTexCoords = inTexCoord;
}  
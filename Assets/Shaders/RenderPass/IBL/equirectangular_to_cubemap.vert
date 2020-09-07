#version 330 core
layout (location = 0) in vec3 inPos;

out vec3 OurLocalPos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    OurLocalPos = inPos;  
    gl_Position =  u_projection * u_view * vec4(OurLocalPos, 1.0);
}
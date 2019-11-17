#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in mat4 inInstanceModel;

uniform mat4 u_model;

void main()
{
  gl_Position = u_model * vec4(inPos, 1.0);
}  
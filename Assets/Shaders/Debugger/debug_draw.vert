#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;


out VS_OUT
{
	vec3 ourFragColor;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(inPos, 1.0f);

  vs_out.ourFragColor = inColor;
}
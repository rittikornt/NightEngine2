#version 330 core

out vec4 FragColor;

in VS_OUT
{
	vec3 ourFragColor;
} fs_in;

void main()
{
	FragColor = vec4(fs_in.ourFragColor, 1.0);
} 
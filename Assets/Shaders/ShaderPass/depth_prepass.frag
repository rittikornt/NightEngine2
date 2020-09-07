#version 330 core

out vec4 o_FragColor;

in VS_OUT
{
	vec2 ourTexCoord;
} fs_in;

void main()
{
	o_FragColor = vec4(0.0,0.0,0.0,1.0);
}
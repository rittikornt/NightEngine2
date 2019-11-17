#version 330 core

out vec4 FragColor;

in VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
} fs_in;

uniform vec3 u_color;
uniform sampler2D u_texture;

void main()
{
	vec4 color = texture2D(u_texture, fs_in.ourTexCoord);
	FragColor = color * vec4(u_color, 1.0);

	if(FragColor.w == 0.0)
	{
		gl_FragDepth = 1.0;
	}
} 
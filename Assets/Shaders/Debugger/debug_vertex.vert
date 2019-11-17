
#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(inPos, 1.0f);

	vs_out.ourTexCoord = inTexCoord;

	//Fragment position in worldspace
	vs_out.ourFragPos = vec3(u_model * vec4(inPos, 1.0));

	//For uniform scaling
	//Fragment Normal in worldspace
	vs_out.ourFragNormal = mat3(u_model) * inNormal;
	//Non-uniform scaling
	//vs_out.ourFragNormal = mat3(transpose(inverse(u_model))) * inNormal;
}
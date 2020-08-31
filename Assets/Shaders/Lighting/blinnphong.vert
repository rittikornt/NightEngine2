
#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in mat4 inInstanceModel;

out VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
	vec4 ourFragPosLightSpace;
} vs_out;

layout (std140) uniform u_matrices
{
	mat4 u_vp;
	mat4 u_projection;
};

uniform mat4 u_model;
uniform bool u_instanceRendering = false;
uniform mat4 u_lightSpaceMatrix;

void main()
{
	mat4 model = u_instanceRendering? inInstanceModel:u_model;

	vs_out.ourTexCoord = inTexCoord;

	//Fragment position in worldspace
	vs_out.ourFragPos = vec3(model * vec4(inPos, 1.0));

	//Non-uniform scaling
	vs_out.ourFragNormal = mat3(transpose(inverse(u_model))) * inNormal;

	vs_out.ourFragPosLightSpace = u_lightSpaceMatrix * vec4(vs_out.ourFragPos, 1.0);
	
	gl_Position = u_vp * model * vec4(inPos, 1.0f);
}
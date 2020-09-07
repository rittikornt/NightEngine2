#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in mat4 inInstanceModel;

out VS_OUT
{
	vec2 ourTexCoord;
} vs_out;

//***************************************
// Uniforms
//***************************************
layout (std140) uniform u_matrices
{
	mat4 u_vp;
	mat4 u_projection;
};

uniform mat4 u_model;
//TODO: consider a separate shader/variant for this bool, since it probably will branch
uniform bool u_instanceRendering = false;

void main()
{
	mat4 model = u_instanceRendering? inInstanceModel:u_model;

	vs_out.ourTexCoord = inTexCoord;
	gl_Position = u_vp * model * vec4(inPos, 1.0f);
}
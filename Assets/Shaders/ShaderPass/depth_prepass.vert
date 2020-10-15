#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in mat4 inInstanceModel;

out VS_OUT
{
	vec2 ourTexCoord;
	vec4 positionCS;
	vec4 prevPositionCS;
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
uniform mat4 u_prevModel;

uniform mat4 u_unjitteredVP;
uniform mat4 u_prevUnjitteredVP;

//TODO: consider a separate shader/variant for this bool, since it probably will branch
uniform bool u_instanceRendering = false;

void main()
{
	mat4 model = u_instanceRendering? inInstanceModel:u_model;

	vec4 worldPos = model * vec4(inPos, 1.0f);
	vec4 prevWorldPos = u_instanceRendering? worldPos: (u_prevModel * vec4(inPos, 1.0f));
	vs_out.ourTexCoord = inTexCoord;

	//No motion vector for GPU instancing for now
	vs_out.positionCS = u_instanceRendering? vec4(0.0, 0.0, 0.0, 1.0): u_unjitteredVP * worldPos;
	vs_out.prevPositionCS = u_instanceRendering? vec4(0.0, 0.0, 0.0, 1.0): u_prevUnjitteredVP * prevWorldPos;

	gl_Position = u_vp * worldPos;
}
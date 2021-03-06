#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in mat4 inInstanceModel;

out VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
	vec4 ourFragPosLightSpace;
	vec3 ourCameraPosTS;
	vec3 ourFragPosTS;
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
uniform bool u_instanceRendering = false;

uniform mat4 u_lightSpaceMatrix;
uniform bool u_useBumpmap = false;
uniform vec3 u_cameraPosWS;

void main()
{
	mat4 model = u_instanceRendering? inInstanceModel:u_model;

	vs_out.ourTexCoord = inTexCoord;

	//Fragment position in worldspace
	vs_out.ourFragPos = (model * vec4(inPos, 1.0)).xyz;

	vs_out.ourFragPosLightSpace = u_lightSpaceMatrix * vec4(vs_out.ourFragPos, 1.0);
	
	gl_Position = u_vp * model * vec4(inPos, 1.0f);
	
	// NormalOS to NormalWS
	// Support Non-uniform scaling
	mat3 inverseTransposeModel = mat3(transpose(inverse(u_model)));
	vs_out.ourFragNormal = normalize(inverseTransposeModel * inNormal);

	//Calculate TBN only if use Normal map
	if(u_useBumpmap)
	{
		//TBN
		vec3 T = normalize(vec3(inverseTransposeModel * inTangent));
		vec3 B = normalize(cross(vs_out.ourFragNormal, T));
		mat3 InverseTBNMatrix = transpose(mat3(T, B, vs_out.ourFragNormal));
		vs_out.ourCameraPosTS = InverseTBNMatrix * u_cameraPosWS;
		vs_out.ourFragPosTS = InverseTBNMatrix * vs_out.ourFragPos;
	}
}
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
	mat4 mv = u_view * u_model;
	const float baseScale = 3.0;
	const float minScale = 0.7;
	const float maxScale = 0.9;

	//The further away the vertex is, the small it appear
	float scale = clamp(baseScale/ length(mv * vec4(inPos, 1.0f) ), minScale, maxScale );

	//Pos (0,0,0) in view space
	vec4 billboard = (mv * vec4(vec3(0.0), 1.0));

	//Pos(x,y) * scale in Clip space
	vec4 newPosition = u_projection * billboard
										+ vec4(scale * inPos.xy, 0.0, 0.0);

	gl_Position = newPosition;

	vs_out.ourTexCoord = inTexCoord;

	//Fragment position in worldspace
	vs_out.ourFragPos = vec3(u_model * vec4(inPos, 1.0));

	//For uniform scaling
	//Fragment Normal in worldspace
	vs_out.ourFragNormal = mat3(u_model) * inNormal;
	//Non-uniform scaling
	//vs_out.ourFragNormal = mat3(transpose(inverse(u_model))) * inNormal;
}
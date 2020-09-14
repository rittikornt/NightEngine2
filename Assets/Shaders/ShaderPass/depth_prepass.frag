#version 330 core

layout (location = 0) out vec4 o_color;

in VS_OUT
{
	vec2 ourTexCoord;
	vec4 positionCS;
	vec4 prevPositionCS;
} fs_in;

void main()
{
	vec4 positionCS = fs_in.positionCS;
	vec4 prevPositionCS = fs_in.prevPositionCS;
	
	//[-1, 1] Clip Space position (perspective div)
	positionCS.xy /= positionCS.w;
	prevPositionCS.xy /= prevPositionCS.w;

	//Velocity from prev to current in clip space pos
	vec2 velocity = positionCS.xy - prevPositionCS.xy;

	// Convert velocity from Clip Space [-1,1] into NDC [0, 1]
	// Note: ((positionCS * 0.5 + 0.5) - (prevPositionCS * 0.5 + 0.5)) = (velocity * 0.5)
	velocity *= 0.5f;

	o_color = vec4(velocity.xy,0.0,1.0);
}
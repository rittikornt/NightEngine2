#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

#include "ShaderLibrary/pbr_lighting.glsl"
#include "ShaderLibrary/gbuffer.glsl"

void main()
{
	MaterialData matData;
	SurfaceData surfaceData;
	UnpackGBufferData(OurTexCoords, matData, surfaceData);

	vec3 viewDir = surfaceData.viewDir;

	if(surfaceData.normal == vec3(0.0, 0.0, 0.0))
	{
		discard;
	}
	
	/////////////////////////////////////////////////////////

	//Main Lighting
	float atten = CalculateDirLightAttenuation(matData.positionLS.xyz, surfaceData.lightDir, surfaceData.normal);

	vec3 Lo = atten * CalculateMainLighting(surfaceData
					, matData.albedo.rgb, matData.roughness, matData.metallic);

	//Additional Lighting (Point/Spot Lights)
	Lo += CalculateAdditionalLighting(viewDir, surfaceData.normal, matData.positionWS.xyz
					, matData.albedo.rgb, matData.roughness, matData.metallic);

	//Ambient
	vec3 ambient = CalculateAmbientLighting(surfaceData
					, matData.albedo.rgb, matData.roughness, matData.metallic);
	vec3 color = Lo + ambient;
	o_fragColor = vec4(color, 1.0);

	//Emissive
	o_fragColor.rgb += matData.emissive.rgb;
}
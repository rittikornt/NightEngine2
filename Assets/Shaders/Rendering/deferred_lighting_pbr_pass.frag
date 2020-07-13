#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

#include "Library/pbr_lighting.glsl"

//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D positionAndNormalX;   //(0) vec4(pos.xyz, n.x)
	sampler2D albedoAndNormalY;     //(1) vec4(albedo.xyz, n.y)
	sampler2D lsPosAndMetallic;     //(2) vec4(lightSpacePos, metallic.x)
	sampler2D emissiveAndRoughness; //(3) vec4(emissive.xyz, roughness.x)
};
uniform GBufferResult u_gbufferResult;

void main()
{
	//Unpack datas from GBuffer
	vec4 albedoAndNormalY = texture(u_gbufferResult.albedoAndNormalY, OurTexCoords);
	vec3 albedo = albedoAndNormalY.rgb;
	
	vec4 positionAndNormalX = texture(u_gbufferResult.positionAndNormalX, OurTexCoords);
	vec3 fragPos = positionAndNormalX.xyz;

	//Unpack normal
	vec3 normal = vec3(0.0);
	normal.x = positionAndNormalX.a;
	normal.y = albedoAndNormalY.a;

	//Discard if normal is black
	UnpackNormalFromRG(normal);
	normal = normalize(normal);
	if(normal == vec3(0.0, 0.0, 0.0))
	{
		discard;
	}

	//Material data
	vec4 lsPosAndMetallic = texture(u_gbufferResult.lsPosAndMetallic, OurTexCoords);
	vec4 emissiveAndRoughness = texture(u_gbufferResult.emissiveAndRoughness, OurTexCoords);
	
	vec3 emissive = emissiveAndRoughness.xyz;
	vec3 fragPosLightSpace = lsPosAndMetallic.xyz;

	float roughness = GetFilteredRoughness(emissiveAndRoughness.a, normal);
	float metallic = lsPosAndMetallic.a;

	/////////////////////////////////////////////////////////

	//Surface Data
	SurfaceData surfaceData = GetSurfaceData(normal, fragPos.xyz);
	vec3 viewDir = surfaceData.viewDir;
	
	//Main Lighting
	float atten = CalculateDirLightAttenuation(fragPosLightSpace.xyz, surfaceData.lightDir, surfaceData.normal);

	vec3 Lo = atten * CalculateMainLighting(surfaceData
					, albedo.rgb, roughness, metallic);

	//Additional Lighting (Point/Spot Lights)
	Lo += CalculateAdditionalLighting(viewDir, surfaceData.normal, fragPos.xyz
					, albedo.rgb, roughness, metallic);

	//Ambient
	vec3 ambient = CalculateAmbientLighting(surfaceData
					, albedo.rgb, roughness, metallic);
	vec3 color = Lo + ambient;
	o_fragColor = vec4(color, 1.0);

	//Emissive
	o_fragColor.rgb += emissive.rgb;
}
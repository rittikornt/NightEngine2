#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

#include "Library/pbr_lighting.glsl"
#include "Library/gbuffer.glsl"

//***************************************
// Uniforms
//***************************************
uniform int u_debugViewIndex;
uniform int u_debugShadowViewIndex;

vec3 GetAdditionalLightingShadows(vec3 ViewDir, vec3 Normal, vec3 fragPos
, vec3 Albedo, float Roughness, float Metallic)
{
	vec3 Lo = vec3(0.0, 0.0, 0.0);

	//Pointlight
	for(int i=0; i < POINTLIGHT_NUM; ++i)
	{
		vec3 LightDir = normalize(u_pointLightInfo[i].m_position - fragPos);
		vec3 Halfway = normalize(ViewDir + LightDir);

		//Shadow
		float shadowAtten = SamplePointLightShadowPCF(i, fragPos
		, u_pointLightInfo[i].m_position, u_cameraInfo.m_position);

		Lo += vec3(shadowAtten);
	}
	Lo /= POINTLIGHT_NUM;
	return Lo;
}

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
	
	vec3 albedo = matData.albedo.rgb;
	vec3 normal = surfaceData.normal;
	vec3 emissive = matData.emissive.xyz;
	float roughness = matData.roughness;
	float metallic = matData.metallic;

	vec3 fragPos = matData.positionWS.xyz;
	vec3 fragPosLightSpace = matData.positionLS.xyz;

	/////////////////////////////////////////////////////////

	vec3 color = vec3(0.0);
	if(u_debugViewIndex == 0)
	{
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

		color = Lo + ambient;
		color.rgb += emissive.rgb;
	}

	//Match the case in IRenderLoop.hpp DebugView enum
	switch(u_debugViewIndex)
	{
		case 1:	//ALBEDO
		color = albedo.rgb;
		break;
		case 2: //NORMAL_WORLD_SPACE
		color = normal.rgb;
		break;
		case 3: //METALLIC
		color = vec3(metallic);
		break;
		case 4: //ROUGHNESS
		color = vec3(roughness);
		break;
		case 5: //EMISSIVE
		color = emissive;
		break;
		case 6: //LIGHT_SPACE_POS
		float lsdepth = (fragPosLightSpace.z * 0.5) + 0.5;
		color = vec3(lsdepth, 0.0, 0.0);
		break;
		case 7: //AMBIENT
		color = CalculateAmbientLighting(surfaceData
						, albedo.rgb, roughness, metallic);
		break;
		case 8: //MAIN_SHADOW_DEPTH
		color = vec3(texture(u_shadowMap2D, OurTexCoords).r, 0.0, 0.0);
		break;
	}

	switch(u_debugShadowViewIndex)
	{
		case 1:	//SHADOW_MAIN_ONLY
		float atten = CalculateDirLightAttenuation(fragPosLightSpace.xyz, surfaceData.lightDir, surfaceData.normal);

		color = vec3(atten);
		break;
		case 2:	//SHADOW_ALL
		float atten2 = CalculateDirLightAttenuation(fragPosLightSpace.xyz, surfaceData.lightDir, surfaceData.normal);

		color = vec3(atten2) * GetAdditionalLightingShadows(viewDir, surfaceData.normal, fragPos.xyz
						, albedo.rgb, roughness, metallic);
		break;
		case 3: //SHADOW_CASCADE
		break;
	}

	o_fragColor = vec4(color, 1.0);
}
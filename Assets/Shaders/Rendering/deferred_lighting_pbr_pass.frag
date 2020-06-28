#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

#include "Library/pbr_lighting.glinc"

//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D m_positionTex;
	sampler2D m_normalTex;
	sampler2D m_albedoTex;
	sampler2D m_specularTex;
	sampler2D m_emissiveTex;
	sampler2D m_lightSpacePos;
};
uniform GBufferResult u_gbufferResult;

void main()
{
	//Discard if normal is black
	vec3 normal = texture(u_gbufferResult.m_normalTex, OurTexCoords).rgb;
	if(normal == vec3(0.0,0.0,0.0))
	{
		discard;
	}
	normal = normalize(normal);

    //Sample Values from G-buffer
	vec3 fragPos = texture(u_gbufferResult.m_positionTex, OurTexCoords).rgb;
	vec4 fragPosLightSpace = texture(u_gbufferResult.m_lightSpacePos, OurTexCoords);

	//Material data
	vec4 diffuse = texture(u_gbufferResult.m_albedoTex, OurTexCoords);
	vec3 specularTex = texture(u_gbufferResult.m_specularTex, OurTexCoords).rgb;
	vec4 emissive = texture(u_gbufferResult.m_emissiveTex, OurTexCoords);
	float roughness = GetFilteredRoughness(specularTex.r, normal); //0.2;
	float metallic = specularTex.g;
	
	//Surface Data
	SurfaceData surfaceData = GetSurfaceData(normal, fragPos);
	vec3 viewDir = surfaceData.viewDir;
	
	//Main Lighting
	float atten = CalculateDirLightAttenuation(fragPosLightSpace, surfaceData.lightDir, surfaceData.normal);

	vec3 Lo = atten * CalculateMainLighting(surfaceData
					, diffuse.rgb, roughness, metallic);

	//Additional Lighting (Point/Spot Lights)
	Lo += CalculateAdditionalLighting(viewDir, surfaceData.normal, fragPos
					, diffuse.rgb, roughness, metallic);

	//Ambient
	vec3 ambient = CalculateAmbientLighting(surfaceData
					, diffuse.rgb, roughness, metallic);
	vec3 color = Lo + ambient;
	o_fragColor = vec4(color, 1.0);

	//Emissive
	o_fragColor += emissive;
}
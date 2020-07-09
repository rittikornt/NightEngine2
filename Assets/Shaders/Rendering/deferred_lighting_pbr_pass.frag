#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

#include "Library/pbr_lighting.glsl"

//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D m_positionTex; // (0) vec4(pos.xyz, n.x)
	sampler2D m_normalTex; //(1) vec4(albedo.xyz, n.y)
	sampler2D m_albedoTex; //(2) vec4(lightSpacePos, metallic.x)
	sampler2D m_specularTex; //(3) vec4(emissive.xyz, roughness.x)
	sampler2D m_emissiveTex;
	sampler2D m_lightSpacePos;
};
uniform GBufferResult u_gbufferResult;

void UnpackNormalFromRG(inout vec3 normal)
{
	// Unpack B value from only RG
	// sqrt(x^2 + y^2 + z^2) = 1
	// z^2 = (1 - x^2 - y^2)
	// z = sqrt(1 - x^2 - y^2)
	normal.z = sqrt(1 - (normal.r * normal.r) - (normal.g * normal.g));
}

void main()
{
	//Discard if normal is black
	vec4 albedoNY = texture(u_gbufferResult.m_normalTex, OurTexCoords);
	vec3 diffuse = albedoNY.rgb;
	//if(normal == vec3(0.0,0.0,0.0))
	//{
	//	discard;
	//}
	
    //Sample Values from G-buffer
	vec4 fragPosNormalX = texture(u_gbufferResult.m_positionTex, OurTexCoords);
	vec3 fragPos = fragPosNormalX.xyz;

	vec3 normal = vec3(0.0);
	normal.x = fragPosNormalX.a;
	normal.y = albedoNY.a;

	if(normal.xy == vec2(0.0, 0.0))
	{
		discard;
	}
	UnpackNormalFromRG(normal);
	normal = normalize(normal);
	//o_fragColor = vec4(normal, 1.0);
	//return;

	//Material data
	vec4 lspM = texture(u_gbufferResult.m_albedoTex, OurTexCoords);
	vec4 specularTex = texture(u_gbufferResult.m_specularTex, OurTexCoords);
	
	//vec4 emissive = texture(u_gbufferResult.m_emissiveTex, OurTexCoords);
	vec3 emissive = specularTex.xyz;//texture(u_gbufferResult.m_emissiveTex, OurTexCoords);
	
	//vec4 fragPosLightSpace = texture(u_gbufferResult.m_lightSpacePos, OurTexCoords);
	vec3 fragPosLightSpace = lspM.xyz;//texture(u_gbufferResult.m_lightSpacePos, OurTexCoords);

	//float roughness = GetFilteredRoughness(specularTex.r, normal); //0.2;
	float roughness = GetFilteredRoughness(specularTex.a, normal); //0.2;
	//float metallic = specularTex.g;
	float metallic = lspM.a;

	//Surface Data
	SurfaceData surfaceData = GetSurfaceData(normal, fragPos.xyz);
	vec3 viewDir = surfaceData.viewDir;
	
	//Main Lighting
	float atten = CalculateDirLightAttenuation(fragPosLightSpace.xyz, surfaceData.lightDir, surfaceData.normal);

	vec3 Lo = atten * CalculateMainLighting(surfaceData
					, diffuse.rgb, roughness, metallic);

	//Additional Lighting (Point/Spot Lights)
	Lo += CalculateAdditionalLighting(viewDir, surfaceData.normal, fragPos.xyz
					, diffuse.rgb, roughness, metallic);

	//Ambient
	vec3 ambient = CalculateAmbientLighting(surfaceData
					, diffuse.rgb, roughness, metallic);
	vec3 color = Lo + ambient;
	o_fragColor = vec4(color, 1.0);

	//Emissive
	o_fragColor.rgb += emissive.rgb;
}
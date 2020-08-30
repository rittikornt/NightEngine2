#version 330 core

// 4 4 4 4
layout (location = 0) out vec4 o_gbuffer0;  //(0) vec4(pos.xyz, n.x)
layout (location = 1) out vec4 o_gbuffer1;	//(1) vec4(albedo.xyz, metallic.x)
layout (location = 2) out vec4 o_gbuffer2;	//(2) vec4(lightSpacePos, n.y)
layout (location = 3) out vec4 o_gbuffer3;	//(3) vec4(emissive.xyz, roughness.x)

#include "Library/common.glsl"

in VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
	vec4 ourFragPosLightSpace;
	mat3 ourTBNMatrix;
} fs_in;

//! brief Store Material information
struct Material
{
  sampler2D 	m_diffuseMap;	//albedo
  
  sampler2D 	m_normalMap;
  float         m_normalMultiplier;

  sampler2D 	m_roughnessMap;
  float 		m_roughnessValue;

  sampler2D		m_metallicMap;
  float 		m_metallicValue;
	
  sampler2D 	m_emissiveMap;
  float			m_emissiveStrength;
  
  sampler2D 	m_opacityMap;
  float			m_cutOffValue;
};
uniform Material u_material;
uniform vec4	 u_diffuseColor = vec4(1.0f);

//***************************************
// Uniforms
//***************************************
uniform bool        u_useNormalmap = false;
uniform bool        u_useOpacityMap = false;

void main()
{
	vec2 uv = fs_in.ourTexCoord.xy;

	if(u_useOpacityMap)
	{
		float opacity = texture(u_material.m_opacityMap, uv).r;
		if(opacity < u_material.m_cutOffValue)
		{
			discard;
		}
	}

	//Sample Normal map
	vec3 normal = fs_in.ourFragNormal;
	if(u_useNormalmap)
	{
		normal = (texture(u_material.m_normalMap, uv).rgb);

		//Remap to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);
		normal.z *= u_material.m_normalMultiplier;

		//Transform tangent to world space normal
		normal = (fs_in.ourTBNMatrix * normal);
		normal = normalize(normal);
	}

	//Roughness, Metallic
	float roughness = texture(u_material.m_roughnessMap, uv).r
								* u_material.m_roughnessValue;
	roughness = clamp(roughness, 0.01, 1.0);

	float metallic = texture(u_material.m_metallicMap, uv).r
								* u_material.m_metallicValue;

	/////////////////////////////////////////////

	// (0) vec4(pos.xyz, n.x)
	o_gbuffer0.xyz = fs_in.ourFragPos.xyz;
	o_gbuffer0.w = normal.x;

	//(1) vec4(albedo.xyz, n.y)
	o_gbuffer1.rgb = texture(u_material.m_diffuseMap, uv).rgb
								* u_diffuseColor.rgb;
	o_gbuffer1.a = metallic;

	//(2) vec4(lightSpacePos, metallic)
	vec3 lightSpacePos = fs_in.ourFragPosLightSpace.xyz / fs_in.ourFragPosLightSpace.w;
	o_gbuffer2.xyz = lightSpacePos;
	o_gbuffer2.w = normal.y;

	//(3) vec4(emissive.xyz, roughness.x)
	o_gbuffer3.rgb = EncodeQuantization(texture(u_material.m_emissiveMap, uv).rgb
								* u_material.m_emissiveStrength, 12);
	o_gbuffer3.a = roughness;
}
#version 330 core

// 4 4 4 4
layout (location = 0) out vec4 o_positionAndNormalX;   //(0) vec4(pos.xyz, n.x)
layout (location = 1) out vec4 o_albedoAndNormalY;	   //(1) vec4(albedo.xyz, n.y)
layout (location = 2) out vec4 o_lsPosAndMetallic;	   //(2) vec4(lightSpacePos, metallic.x)
layout (location = 3) out vec4 o_emissiveAndRoughness; //(3) vec4(emissive.xyz, roughness.x)

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
};
uniform Material u_material;
uniform vec4	 u_diffuseColor = vec4(1.0f);

//***************************************
// Uniforms
//***************************************
uniform bool        u_useNormalmap = false;

void main()
{
	//Sample Normal map
	vec3 normal = fs_in.ourFragNormal;
	if(u_useNormalmap)
	{
		normal = (texture(u_material.m_normalMap, fs_in.ourTexCoord).rgb);

		//Remap to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);
		normal.z *= u_material.m_normalMultiplier;

		//Transform tangent to world space normal
		normal = (fs_in.ourTBNMatrix * normal);
		normal = normalize(normal);
	}

	//Roughness, Metallic
	float roughness = texture(u_material.m_roughnessMap, fs_in.ourTexCoord).r
								* u_material.m_roughnessValue;
	roughness = max(0.01, roughness);	//Somehow 0 roughness doesn't behave properly

	float metallic = texture(u_material.m_metallicMap, fs_in.ourTexCoord).r
								* u_material.m_metallicValue;

	/////////////////////////////////////////////

	// (0) vec4(pos.xyz, n.x)
	o_positionAndNormalX.xyz = fs_in.ourFragPos.xyz;
	o_positionAndNormalX.w = normal.x;

	//(1) vec4(albedo.xyz, n.x)
	o_albedoAndNormalY.rgb = texture(u_material.m_diffuseMap, fs_in.ourTexCoord).rgb
								* u_diffuseColor.rgb;
	o_albedoAndNormalY.a = normal.y;

	//(2) vec4(lightSpacePos, metallic.x)
	vec3 lightSpacePos = fs_in.ourFragPosLightSpace.xyz / fs_in.ourFragPosLightSpace.w;
	o_lsPosAndMetallic.xyz = lightSpacePos;
	o_lsPosAndMetallic.w = metallic;

	//(3) vec4(emissive.xyz, roughness.x)
	o_emissiveAndRoughness.rgb = texture(u_material.m_emissiveMap, fs_in.ourTexCoord).rgb
								* u_material.m_emissiveStrength;
	o_emissiveAndRoughness.a = roughness;
}
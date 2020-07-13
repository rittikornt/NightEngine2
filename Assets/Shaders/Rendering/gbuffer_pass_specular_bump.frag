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
  
  sampler2D 	m_bumpMap;
  float         m_bumpMultiplier;

  sampler2D 	m_specularMap;
  float 		m_specularValue;

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
uniform bool        u_useBumpmap = false;

void main()
{
	//Sample Bump map
	vec3 normal = fs_in.ourFragNormal;
	if(u_useBumpmap)
	{
		float bump = (texture(u_material.m_bumpMap, fs_in.ourTexCoord).r);

		//Remap to range [-1,1]
		bump = (bump * 2.0 - 1.0);
		bump *= u_material.m_bumpMultiplier;
		
		normal.z = bump;
		normal = normalize(normal);

		//Transform tangent to world space normal
		//normal = (fs_in.ourTBNMatrix * bump);
	}

	//Roughness, Metallic
	float roughness = max(texture(u_material.m_specularMap, fs_in.ourTexCoord).r
								, u_material.m_specularValue);
	roughness = max(0.01, roughness);	//Somehow 0 roughness doesn't behave properly

	float metallic = max(texture(u_material.m_metallicMap, fs_in.ourTexCoord).r
								, u_material.m_metallicValue);

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
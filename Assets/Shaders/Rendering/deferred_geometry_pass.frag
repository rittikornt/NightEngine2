#version 330 core

// 4 4 4 4
layout (location = 0) out vec3 o_position;	//3
layout (location = 1) out vec3 o_normal;	//2
layout (location = 2) out vec3 o_albedo;	//3
layout (location = 3) out vec3 o_roughnessMetallic; //2
layout (location = 4) out vec3 o_emissive; //3
layout (location = 5) out vec4 o_lightSpacePos; //3

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
  sampler2D 	m_diffuseMap;
  
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
	//Position
	o_position = fs_in.ourFragPos;

	//Sample Normal map
	vec3 normal = fs_in.ourFragNormal;
	if(u_useNormalmap)
	{
		normal = (texture(u_material.m_normalMap, fs_in.ourTexCoord).rgb);

		//Remap to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);
		normal.xz *= u_material.m_normalMultiplier;

		//Transform to world space
		normal = (fs_in.ourTBNMatrix * normal);
	}
	o_normal = normalize(normal);

	//Albedo
	o_albedo.rgb = texture(u_material.m_diffuseMap, fs_in.ourTexCoord).rgb
								* u_diffuseColor.rgb;

	//Specular
	float roughness = max(texture(u_material.m_roughnessMap, fs_in.ourTexCoord).r
								, u_material.m_roughnessValue);
	roughness = max(0.01, roughness);	//Somehow 0 roughness doesn't behave properly

	o_roughnessMetallic.r = roughness;
	o_roughnessMetallic.g = max(texture(u_material.m_metallicMap, fs_in.ourTexCoord).r
								, u_material.m_metallicValue);

	//Emissive
	o_emissive.rgb = texture(u_material.m_emissiveMap, fs_in.ourTexCoord).rgb
								* u_material.m_emissiveStrength;

	//Light space Matrix
	o_lightSpacePos = fs_in.ourFragPosLightSpace;
}
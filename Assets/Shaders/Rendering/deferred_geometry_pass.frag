#version 330 core

layout (location = 0) out vec3 o_position;	//Final output Color
layout (location = 1) out vec3 o_normal;
layout (location = 2) out vec3 o_albedo;
layout (location = 3) out vec3 o_roughnessMetallic;
layout (location = 4) out vec3 o_emissive;
layout (location = 5) out vec4 o_lightSpacePos;

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
  float       m_normalMultiplier;

  sampler2D 	m_roughnessMap;
	float 			m_roughnessValue;

	sampler2D		m_metallicMap;
	float 			m_metallicValue;
	
  sampler2D 	m_emissiveMap;
	float				m_emissiveStrength;
};
uniform Material u_material;
uniform vec3		 u_diffuseColor = vec3(1.0f);

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
    normal.xy *= u_material.m_normalMultiplier;

    //Transform to world space
    normal = normalize(fs_in.ourTBNMatrix * normal);
  }
	o_normal = normal;

	//Albedo
	o_albedo.rgb = texture(u_material.m_diffuseMap, fs_in.ourTexCoord).rgb
								* u_diffuseColor;

	//Specular
	o_roughnessMetallic.r = max(texture(u_material.m_roughnessMap, fs_in.ourTexCoord).r
								, u_material.m_roughnessValue);
	o_roughnessMetallic.g = max(texture(u_material.m_metallicMap, fs_in.ourTexCoord).r
								, u_material.m_metallicValue);

	//Emissive
	o_emissive.rgb = texture(u_material.m_emissiveMap, fs_in.ourTexCoord).rgb
								* u_material.m_emissiveStrength;

	//Light space Matrix
	o_lightSpacePos = fs_in.ourFragPosLightSpace;
}
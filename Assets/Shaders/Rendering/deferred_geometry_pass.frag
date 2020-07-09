#version 330 core

// 4 4 4 4
layout (location = 0) out vec4 o_position;	// (0) vec4(pos.xyz, n.x)
layout (location = 1) out vec4 o_normal;	//(1) vec4(albedo.xyz, n.y)
layout (location = 2) out vec4 o_albedo;	//(2) vec4(lightSpacePos, metallic.x)
layout (location = 3) out vec4 o_roughnessMetallic; //(3) vec4(emissive.xyz, roughness.x)
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
		normal.xz *= u_material.m_normalMultiplier;

		//Transform to world space
		normal = (fs_in.ourTBNMatrix * normal);
	}

	// Pack normal to only RG
	// sqrt(x^2 + y^2 + z^2) = 1
	// z^2 = (1 - x^2 - y^2)
	// z = sqrt(1 - x^2 - y^2)
	//normal = normalize(normal);
	//o_normal.xy = normal.xy;
	//o_normal = normal;
	//o_normal.z = 0.0;

	// (0) vec4(pos.xyz, n.x)
	o_position.xyz = fs_in.ourFragPos.xyz;
	o_position.w = normal.x;

	//(1) vec4(albedo.xyz, n.x)
	o_normal.rgb = texture(u_material.m_diffuseMap, fs_in.ourTexCoord).rgb
								* u_diffuseColor.rgb;
	o_normal.a = normal.y;

	//Roughness, Metallic
	float roughness = max(texture(u_material.m_roughnessMap, fs_in.ourTexCoord).r
								, u_material.m_roughnessValue);
	roughness = max(0.01, roughness);	//Somehow 0 roughness doesn't behave properly

	float metallic = max(texture(u_material.m_metallicMap, fs_in.ourTexCoord).r
								, u_material.m_metallicValue);

	//o_roughnessMetallic.r = roughness;
	//o_roughnessMetallic.g = metallic;

	//(2) vec4(lightSpacePos, metallic.x)
	vec3 lightSpacePos = fs_in.ourFragPosLightSpace.xyz / fs_in.ourFragPosLightSpace.w;
	o_albedo.xyz = lightSpacePos;
	o_albedo.w = metallic;

	//Emissive
	//o_emissive.rgb = texture(u_material.m_emissiveMap, fs_in.ourTexCoord).rgb
	//							* u_material.m_emissiveStrength;

	//(3) vec4(emissive.xyz, roughness.x)
	o_roughnessMetallic.rgb = texture(u_material.m_emissiveMap, fs_in.ourTexCoord).rgb
								* u_material.m_emissiveStrength;
	o_roughnessMetallic.a = roughness;

	//Light space Matrix
	//o_lightSpacePos = fs_in.ourFragPosLightSpace;
}
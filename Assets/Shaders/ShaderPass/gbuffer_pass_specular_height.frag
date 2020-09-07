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
	vec3 ourCameraPosTS;
	vec3 ourFragPosTS;
} fs_in;

//! brief Store Material information
struct Material
{
  sampler2D 	m_diffuseMap;	//albedo
  
  sampler2D 	m_bumpMap;
  float         m_bumpMultiplier;

  sampler2D 	m_specularMap;
  float 		m_specularMul;

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
uniform bool        u_useBumpmap = false;
uniform bool        u_useOpacityMap = false;

//https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
//https://forum.unity.com/threads/how-do-i-use-a-heightmap-in-shader-graph.538170/#post-3547729
vec2 GetParallaxOffsetUV(vec2 uv, vec3 viewDirTS)
{
	float heightMul = 0.05;
	float heightScale = u_material.m_bumpMultiplier;
	float height =  texture(u_material.m_bumpMap, uv).r; 
    heightScale = heightScale * height - (height * 0.5f);

    vec3 v = (viewDirTS);
    v.z += 0.42;
    return uv - (heightMul * heightScale * (v.xy / v.z));
}

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

	//Sample Bump map
	vec3 normal = fs_in.ourFragNormal;
	normal = normalize(normal);
	if(u_useBumpmap)
	{
		//Parallaxing map offset
		vec3 viewDirTS = normalize(fs_in.ourCameraPosTS - fs_in.ourFragPosTS);
		uv = GetParallaxOffsetUV(uv, viewDirTS.xyz);
	}

	//Roughness, Metallic
	float roughness = 1.1 - (texture(u_material.m_specularMap, uv).r * u_material.m_specularMul);
	roughness = max(0.01, roughness);	//Somehow 0 roughness doesn't behave properly
	roughness = clamp(roughness, 0.0, 1.0);

	float metallic = texture(u_material.m_metallicMap, uv).r
								* u_material.m_metallicValue;

	/////////////////////////////////////////////

	// (0) vec4(pos.xyz, n.x)
	o_positionAndNormalX.xyz = fs_in.ourFragPos.xyz;
	o_positionAndNormalX.w = normal.x;

	//(1) vec4(albedo.xyz, n.x)
	o_albedoAndNormalY.rgb = texture(u_material.m_diffuseMap, uv).rgb
								* u_diffuseColor.rgb;
	o_albedoAndNormalY.a = normal.y;

	//(2) vec4(lightSpacePos, metallic.x)
	vec3 lightSpacePos = fs_in.ourFragPosLightSpace.xyz / fs_in.ourFragPosLightSpace.w;
	o_lsPosAndMetallic.xyz = lightSpacePos;
	o_lsPosAndMetallic.w = metallic;

	//(3) vec4(emissive.xyz, roughness.x)
	o_emissiveAndRoughness.rgb = texture(u_material.m_emissiveMap, uv).rgb
								* u_material.m_emissiveStrength;
	o_emissiveAndRoughness.a = roughness;
}
#version 330 core

out vec4 o_fragColor;	//Final output Color

in vec2 OurTexCoords;

//***************************************
// Uniforms
//***************************************
struct CameraInfo
{
	vec3 m_position;
	//vec3 m_lookDir;
};
uniform CameraInfo u_cameraInfo;

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

struct LightInfo
{
  vec3 	m_position;
	vec3	m_direction;
  vec3 	m_color;
  float m_intensity;		//For directional/pointlight
	float m_innerCutOff;	//For spotlight
	float m_outerCutOff;
};

#define POINTLIGHT_NUM 4
#define SPOTLIGHT_NUM 4
uniform LightInfo u_dirLightInfo;
uniform LightInfo u_pointLightInfo[POINTLIGHT_NUM];
uniform LightInfo u_spotLightInfo[SPOTLIGHT_NUM];

uniform sampler2D   u_shadowMap2D;
uniform samplerCube u_shadowMap[POINTLIGHT_NUM];
uniform float       u_farPlane;

//IBL
uniform float				u_ambientStrength = 0.6f;
uniform samplerCube u_irradianceMap;

//Specular IBL
uniform samplerCube u_prefilterMap;
uniform sampler2D		u_brdfLUT;

//***************************************
// Global Variable
//***************************************
#define PI 3.14159265359

vec3 g_sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

//***************************************
// Function Declaration
//***************************************
float GetPointLightAttenuation(vec3 fragPos, vec3 lightPos, float intensity);
float GetSpotLightAttenuation(vec3 fragPos, LightInfo lightInfo);

//BRDF: D, G, F
float DistributionGGXTR(vec3 Normal, vec3 Halfway, float Roughness);
float GeometrySmith(vec3 Normal, vec3 ViewDir
	, vec3 LightDir, float Roughness);
vec3 FresnelSchlick(float HdotV, vec3 F0);
vec3 FresnelSchlick2(float HdotV, vec3 F0, float Roughness);

//Irradiance
vec3 CalculateDirectionalIrradiance(vec3 ViewDir,vec3 LightDir, vec3 Halfway, vec3 Normal
, float Attenuation, vec3 LightColor, vec3 Albedo
, float Roughness, float Metallic);
vec3 CalculateIrradiance(vec3 ViewDir,vec3 LightDir, vec3 Halfway, vec3 Normal
, float Attenuation, vec3 LightColor, vec3 Albedo
, float Roughness, float Metallic);

//Shadow
float CalculateDirectionShadow(vec4 fragLightSpacePos
, vec3 lightDir, vec3 normal);
float CalculatePointShadow(int lightIndex, vec3 fragPos
, vec3 lightPos, vec3 camPos);

//Specular Antialiasing
float GetFilteredRoughness(float roughness, vec3 worldNormal);

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
	vec4 diffuse = texture(u_gbufferResult.m_albedoTex, OurTexCoords);
	vec3 specularTex = texture(u_gbufferResult.m_specularTex, OurTexCoords).rgb;
	vec4 emissive = texture(u_gbufferResult.m_emissiveTex, OurTexCoords);
	vec4 fragPosLightSpace = texture(u_gbufferResult.m_lightSpacePos, OurTexCoords);

	//Dir to Camera
	vec3 ViewDir = normalize(u_cameraInfo.m_position - fragPos);

	//From GBuffer
	float roughness = GetFilteredRoughness(specularTex.r, normal); //0.2;
	float metallic = specularTex.g; //1.0;
	
	//Directional Light
	vec3 DirLightDir = normalize(u_dirLightInfo.m_direction);
	vec3 DirHalfway = normalize(ViewDir + DirLightDir);
	
	//Shadow
	float shadow = u_dirLightInfo.m_intensity - CalculateDirectionShadow(fragPosLightSpace, DirLightDir, normal);

	vec3 Lo = shadow * CalculateDirectionalIrradiance(ViewDir, DirLightDir, DirHalfway, normal
					, u_dirLightInfo.m_intensity, u_dirLightInfo.m_color
					, diffuse.rgb, roughness, metallic);

	//Pointlight
	for(int i=0; i < POINTLIGHT_NUM; ++i)
	{
		vec3 LightDir = normalize(u_pointLightInfo[i].m_position - fragPos);
		vec3 Halfway = normalize(ViewDir + LightDir);

		float attenuation = GetPointLightAttenuation(fragPos, u_pointLightInfo[i].m_position
									, u_pointLightInfo[i].m_intensity);

		//Shadow
		float shadow = 1.0 - CalculatePointShadow(i, fragPos
		, u_pointLightInfo[i].m_position, u_cameraInfo.m_position);

		Lo += shadow * CalculateIrradiance(ViewDir, LightDir, Halfway, normal
						, attenuation, u_pointLightInfo[i].m_color
						, diffuse.rgb, roughness, metallic);
	}
	
	//Spotlight
	for(int i=0; i < SPOTLIGHT_NUM; ++i)
	{
		vec3 LightDir = normalize(u_spotLightInfo[i].m_position - fragPos);
		vec3 Halfway = normalize(ViewDir + LightDir);

		float attenuation = GetSpotLightAttenuation(fragPos, u_spotLightInfo[i]);

		Lo += CalculateIrradiance(ViewDir, LightDir, Halfway, normal
						, attenuation, u_spotLightInfo[i].m_color
						, diffuse.rgb, roughness, metallic);
	}

	//Fresnel F
	float NdotV = max(dot(normal, ViewDir), 0.0);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, diffuse.rgb, metallic);
	vec3 F = FresnelSchlick2( NdotV, F0, roughness);

	//Energy Conservation
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 ReflectedDir = reflect(-ViewDir, normal);

	//Get specular IBL from BRDFlut and prefilteredMap (specular color)
	//BRDFLut store precomputed BRDF response for each (angle between N and lightDir, roughness)
	vec3 prefilteredColor = textureLod(u_prefilterMap
		, ReflectedDir, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(u_brdfLUT, vec2(NdotV, roughness)).rg;
	vec3 indirectSpecularColor = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	//IBL Indirect Diffuse part
	vec3 irradiance = texture(u_irradianceMap, normal).rgb;
	vec3 diffuseColor = kD * (irradiance * diffuse.rgb);
	
	//Ambient
	vec3 ambient = diffuseColor + indirectSpecularColor;
	vec3 color = (ambient * shadow * u_ambientStrength) + Lo;
	o_fragColor = vec4(color, 1.0);

	//Emissive
	o_fragColor += emissive;
	
	//o_fragColor = vec4(u_pointLightInfo[0].m_position,0.0);
  //o_fragColor = vec4(normal, 1.0);
} 

//**********************************************************
// Function Definition
//**********************************************************
//! brief Calculate falloff Attenuation
float GetPointLightAttenuation(vec3 fragPos, vec3 lightPos, float intensity)
{
  float dist = length(lightPos - fragPos);
	return ((intensity *intensity) / (dist * dist));
}

//! brief Calculate falloff Attenuation
float GetSpotLightAttenuation(vec3 fragPos, LightInfo lightInfo)
{
	vec3 lightDir = normalize(fragPos - lightInfo.m_position);
	float theta = dot(lightDir, normalize(lightInfo.m_direction));

	float epsilon = max(lightInfo.m_innerCutOff - lightInfo.m_outerCutOff, 0.001);
	float intensity = clamp ((theta - lightInfo.m_outerCutOff)/ epsilon, 0.0, 1.0);

  float dist = length(lightInfo.m_position - fragPos);
	return intensity * lightInfo.m_intensity* lightInfo.m_intensity;
}

//! @brief Calculate how much this fragment is in shadow
float CalculateDirectionShadow(vec4 fragLightSpacePos, vec3 lightDir, vec3 normal)
{
	//Perspective division
	vec3 projCoords = fragLightSpacePos.xyz / fragLightSpacePos.w;

	//Check for oversampling
	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

	//Convert to range [0,1] to sample the shadowMap
	projCoords = (projCoords * 0.5) + 0.5;
	//float closestDepth = texture(u_shadowMap2D, projCoords.xy).r;
	float currentDepth = projCoords.z;

	//bias value
	float bias = max(0.05* (1.0 - dot(normal, lightDir) ),0.005);

	//Size of single texel
	vec2 texelSize = 1.0/ textureSize(u_shadowMap2D, 0);
	float shadowIntensity = u_dirLightInfo.m_intensity;

	//PCF soft shadow
	float shadow = 0.0;
	for(int x= -1; x < 2; ++x)
	{
		for(int y= -1; y < 2; ++y)
		{
			float pcfDepth = texture(u_shadowMap2D, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? shadowIntensity : 0.0;
		}
	}
	shadow /= 9.0;

	//Check if in shadow or not
	return shadow ;
}

//! @brief Calculate how much this fragment is in shadow
float CalculatePointShadow(int lightIndex, vec3 fragPos, vec3 lightPos, vec3 camPos)
{
  //Dir for Sampling the cubemap 
  vec3  lightToFragDir = fragPos - lightPos;
  float currentDepth = length(lightToFragDir);

  float shadow = 0.0;
  float bias   = 0.15;
  int   samples  = 20;

  float camDist = length(camPos - fragPos);
  float diskRadius = (1.0 + (camDist / u_farPlane)) / 25.0;  
  for(int i = 0; i < samples; ++i)
  {
    //Sample shadowmap
    float closestDepth = texture(u_shadowMap[lightIndex]
			, lightToFragDir + g_sampleOffsetDirections[i] * diskRadius).r;
    
    //Remap [0,1] range to [0, u_farPlane]
    closestDepth *= u_farPlane;

    //Is in shadow?
    if(currentDepth - bias > closestDepth)
    {
      shadow += 1.0;
    }
  }
  shadow /= float(samples);  

	//Check if in shadow or not
  return shadow;
	
	// float closestDepth = texture(u_shadowMap[lightIndex], lightToFragDir).r;
  //   closestDepth *= u_farPlane;
	// return closestDepth/ u_farPlane;
}

//**********************************************************
// Function Definition
//**********************************************************
float DistributionGGXTR(vec3 Normal, vec3 Halfway, float Roughness)
{
  float a2 = Roughness * Roughness;

  float NdotH = max(dot(Normal, Halfway), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
  return NdotV / ( (NdotV) * (1.0 - k) + k );
}

float GeometrySmith(vec3 Normal, vec3 ViewDir
, vec3 LightDir, float Roughness)
{
	//Calculate K for light source
	float r = Roughness + 1.0;
	float k = (r * r) / 8.0;

  float NdotV = max(dot(Normal, ViewDir), 0.0);
  float NdotL = max(dot(Normal, LightDir), 0.0);
  float ggx1 = GeometrySchlickGGX(NdotV, k);
  float ggx2 = GeometrySchlickGGX(NdotL, k);

  return ggx1 * ggx2;
}

vec3 FresnelSchlick(float HdotV, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 FresnelSchlick2(float HdotV, vec3 F0, float Roughness)
{
  return F0 + (max( (1.0 - F0), F0) - F0 ) * pow(1.0 - HdotV, 5.0);
}

vec3 CalculateDirectionalIrradiance(vec3 ViewDir,vec3 LightDir, vec3 Halfway, vec3 Normal
, float Attenuation, vec3 LightColor, vec3 Albedo
, float Roughness, float Metallic)
{
	//Radiance (L)
	vec3 radiance = LightColor * Attenuation;

	//How much light hits normal
	float NdotL = max(dot(Normal, LightDir), 0.0);

	//BRDF Specular: D G terms
	float D = DistributionGGXTR(Normal,Halfway,Roughness);
	float G = GeometrySmith(Normal, ViewDir, LightDir, Roughness);

	//Fresnel F
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, Metallic);
	vec3 F = FresnelSchlick( max(dot(Halfway, ViewDir), 0.0), F0);

	//Specular term
	float denom = 4.0 * max(dot(Normal, ViewDir), 0.0) 
										* max(dot(Normal, LightDir), 0.0);
	vec3 specular = (D * G * F)/ max(denom, 0.001);

	//Diffuse term
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - Metallic;

	//Final BRDF
	vec3 Fr = (kD * (Albedo / PI) ) + specular;

	//Irradiance (Lo)
	return (Fr * radiance * NdotL);
}

vec3 CalculateIrradiance(vec3 ViewDir,vec3 LightDir, vec3 Halfway, vec3 Normal
, float Attenuation, vec3 LightColor, vec3 Albedo
, float Roughness, float Metallic)
{
	//Radiance (L)
	vec3 radiance = LightColor * Attenuation;

	//How much light hits normal
	float NdotL = max(dot(Normal, LightDir), 0.0);

	//BRDF Specular: D G terms
	float D = DistributionGGXTR(Normal,Halfway,Roughness);
	float G = GeometrySmith(Normal, ViewDir, LightDir, Roughness);

	//Fresnel F
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, Metallic);
	vec3 F = FresnelSchlick( max(dot(Halfway, ViewDir), 0.0), F0);

	//Specular term
	float denom = 4.0 * max(dot(Normal, ViewDir), 0.0) 
										* max(dot(Normal, LightDir), 0.0);
	vec3 specular = (D * G * F)/ max(denom, 0.001);

	//Diffuse term
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - Metallic;

	//Final BRDF
	vec3 Fr = (kD * (Albedo / PI) ) + specular;

	//Irradiance (Lo)
	return Fr * radiance * NdotL;
}

float GetFilteredRoughness(float roughness, vec3 worldNormal)
{
	//Specular Antialiasing to filter the roughness value
	//https://twitter.com/longbool/status/1221773633263165440
	//http://www.jp.square-enix.com/tech/library/pdf/ImprovedGeometricSpecularAA.pdf
	float roughness2 = roughness * roughness;
	vec3 dndu = dFdx(worldNormal);
	vec3 dndv = dFdy(worldNormal);
	float variance = 0.25 * (dot(dndu, dndu) + dot(dndv, dndv));
	float kernelRoughness2 = min(2.0 * variance, 0.18);
	return clamp(roughness2 + kernelRoughness2, 0.0, 1.0);
}
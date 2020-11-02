#define FLT_EPSILON     1.192092896e-07 // Smallest positive number, such that 1.0 + FLT_EPSILON != 1.0

#include "ShaderLibrary/common.glsl"

//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D gbuffer0;   //(0) vec4(n.xy)
	sampler2D gbuffer1;   //(1) vec4(albedo.xyz, metallic)
	sampler2D gbuffer2;	  //(3) vec4(emissive.xyz, roughness.x)
};
uniform GBufferResult u_gbuffer;

layout(binding=13) uniform sampler2D   u_depthTexture;
uniform mat4    u_invVP;
uniform mat4    u_lightSpaceMatrix;

struct MaterialData
{
	vec3 albedo;
	float roughness;
	float metallic;
	vec3 emissive;

	//TODO: remove this (viewpos from depth instead, lspos in deferred lighting pass)
	vec3 positionWS;
	vec3 positionLS;
};
   
//**********************************************************
// Helper Function
//**********************************************************
SurfaceData GetSurfaceData(vec3 normal, vec3 fragWorldPos)
{
	SurfaceData data;
	data.normal = normal;
	data.lightDir = u_dirLightInfo.m_direction;

	data.viewDir = normalize(u_cameraInfo.m_position - fragWorldPos);
	data.halfWayVector = normalize(data.viewDir + data.lightDir);

	return data;
}

void UnpackNormalFromRG(inout vec3 normal)
{
	// Unpack B value from only RG
	// sqrt(x^2 + y^2 + z^2) = 1
	// z^2 = (1 - x^2 - y^2)
	// z = sqrt(1 - x^2 - y^2)
	normal.z = sqrt(1 - (normal.r * normal.r) - (normal.g * normal.g));

	//TODO: Should use this one instead
	// the above one somehow could cause black pixel (probably due to negating the float on GPU)
	//normal.z = sqrt(1 - ((normal.r * normal.r) + (normal.g * normal.g)));
	//sqrt(1 - saturate(dot(unitVector.xy, unitVector.xy)));
}

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 sRGBLo = c * 12.92;
    vec3 sRGBHi = (PositivePow(c, vec3(1.0/2.4, 1.0/2.4, 1.0/2.4)) * 1.055) - 0.055;
    vec3 sRGB   = vec3((c.x <= 0.0031308) ? sRGBLo.x : sRGBHi.x
			, (c.y <= 0.0031308) ? sRGBLo.y : sRGBHi.y
			, (c.z <= 0.0031308) ? sRGBLo.z : sRGBHi.z);
    return sRGB;
}

void UnpackGBufferData(vec2 uv
    , out MaterialData matData, out SurfaceData surfaceData)
{
	//Unpack datas from GBuffer
	vec4 gbuffer1 = texture(u_gbuffer.gbuffer1, uv);
	matData.albedo = LinearToSRGB(gbuffer1.rgb);	//Need to convert back to SRGB, since SRGB8_ALPHA8 auto convert to linear on encode
	
	//matData.positionWS = gbuffer0.xyz;
  	float depth = textureLod(u_depthTexture, uv, 0.0f).r;
  	matData.positionWS = DepthToWorldSpacePosition(depth, uv, u_invVP).xyz;

	//Unpack normal
	vec2 gbuffer0 = texture(u_gbuffer.gbuffer0, uv).xy;
	vec3 normal = vec3(0.0);
	normal.xy = gbuffer0.xy;

	//Discard if normal is black
	UnpackNormalFromRG(normal);
	normal = normalize(normal);

	//Material data
	vec4 gbuffer2 = texture(u_gbuffer.gbuffer2, uv);
	
	matData.emissive = DecodeQuantization(gbuffer2.xyz, 12);
	matData.positionLS = (u_lightSpaceMatrix * vec4(matData.positionWS, 1.0)).xyz;
	
	matData.roughness = GetFilteredRoughness(gbuffer2.a, normal);
	matData.metallic = gbuffer1.a;

	/////////////////////////////////////////////////////////
	surfaceData = GetSurfaceData(normal, matData.positionWS.xyz);
}
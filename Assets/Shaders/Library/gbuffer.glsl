//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D positionAndNormalX;   //(0) vec4(pos.xyz, n.x)
	sampler2D albedoAndNormalY;     //(1) vec4(albedo.xyz, n.y)
	sampler2D lsPosAndMetallic;     //(2) vec4(lightSpacePos, metallic.x)
	sampler2D emissiveAndRoughness; //(3) vec4(emissive.xyz, roughness.x)
};
uniform GBufferResult u_gbufferResult;

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

void UnpackGBufferData(vec2 uv
    , out MaterialData matData, out SurfaceData surfaceData)
{
	//Unpack datas from GBuffer
	vec4 albedoAndNormalY = texture(u_gbufferResult.albedoAndNormalY, uv);
	matData.albedo = albedoAndNormalY.rgb;
	
	vec4 positionAndNormalX = texture(u_gbufferResult.positionAndNormalX, uv);
	matData.positionWS = positionAndNormalX.xyz;

	//Unpack normal
	vec3 normal = vec3(0.0);
	normal.x = positionAndNormalX.a;
	normal.y = albedoAndNormalY.a;

	//Discard if normal is black
	UnpackNormalFromRG(normal);
	normal = normalize(normal);

	//Material data
	vec4 lsPosAndMetallic = texture(u_gbufferResult.lsPosAndMetallic, uv);
	vec4 emissiveAndRoughness = texture(u_gbufferResult.emissiveAndRoughness, uv);
	
	matData.emissive = emissiveAndRoughness.xyz;
	matData.positionLS = lsPosAndMetallic.xyz;

	matData.roughness = GetFilteredRoughness(emissiveAndRoughness.a, normal);
	matData.metallic = lsPosAndMetallic.a;

	/////////////////////////////////////////////////////////
	surfaceData = GetSurfaceData(normal, matData.positionWS.xyz);
}
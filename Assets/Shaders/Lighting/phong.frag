#version 330 core

out vec4 FragColor;	//Final output Color

in VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
} fs_in;

//! brief Store Camera information
struct CameraInfo
{
	vec3 m_position;
	vec3 m_lookDir;
};
uniform CameraInfo u_cameraInfo;

//! brief Store Material information
struct Material
{
  sampler2D 	m_diffuseMap;

  sampler2D 	m_specularMap;
	float 			m_specularStrength;
  int 				m_shininess;
	
  sampler2D 	m_emissiveMap;
	float				m_emissiveStrength;
};
uniform Material u_material;

//! brief Store light information
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

//Function Declaration
float LinearizeDepth(float depth, float near, float far);
vec4 GetPhongColorDirectionalLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor);
vec4 GetPhongColorPointLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor);
vec4 GetPhongColorSpotLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor);

float g_near = 0.01;
float g_far = 100.0;

void main()
{
	//float depth = LinearizeDepth(gl_FragCoord.z, g_near, g_far) / g_far;
	//FragColor = vec4(vec3(depth), 1.0);
	//return;

	vec4 diffuseTex = texture(u_material.m_diffuseMap, fs_in.ourTexCoord);
	vec4 specularTex = texture(u_material.m_specularMap, fs_in.ourTexCoord);
	vec4 emissiveTex = texture(u_material.m_emissiveMap, fs_in.ourTexCoord);
	
	//Directional Light
	FragColor = GetPhongColorDirectionalLight(fs_in.ourFragPos,fs_in.ourFragNormal, u_dirLightInfo
	, diffuseTex, specularTex);

	//Pointlight
	for(int i=0; i < POINTLIGHT_NUM; ++i)
	{
		FragColor += GetPhongColorPointLight(fs_in.ourFragPos,fs_in.ourFragNormal, u_pointLightInfo[i]
		, diffuseTex, specularTex);
	}
	
	//Spotlight
	for(int i=0; i < SPOTLIGHT_NUM; ++i)
	{
		FragColor += GetPhongColorSpotLight(fs_in.ourFragPos,fs_in.ourFragNormal, u_spotLightInfo[i]
		, diffuseTex, specularTex);
	}

	//Emissive
	vec3 emission = emissiveTex.rgb * u_material.m_emissiveStrength;
	FragColor += vec4(emission, 1.0);
} 

//**********************************************************
// Function Definition
//**********************************************************

//! brief Linearize the depth value
float LinearizeDepth(float depth, float near, float far)
{
	float z = depth * 2.0 - 1.0; //to NDC [-1.0, 1.0]
	return (2.0 * near * far)/ (far + near - z* (far - near));
}

//! brief Calculate falloff intensity
float GetPointLightIntensity(vec3 fragPos, vec3 lightPos, float intensity)
{
  float dist = length(lightPos - fragPos);
	return intensity / (dist * dist);
}

//! brief Calculate falloff intensity
float GetSpotLightIntensity(vec3 fragPos, LightInfo lightInfo)
{
	vec3 lightDir = normalize(fragPos - lightInfo.m_position);
	float theta = dot(lightDir, normalize(lightInfo.m_direction));
	float epsilon = lightInfo.m_innerCutOff - lightInfo.m_outerCutOff;
	float intensity = clamp ((theta - lightInfo.m_outerCutOff)/ epsilon, 0.0, 1.0);

  float dist = length(lightInfo.m_position - fragPos);
	return (intensity * lightInfo.m_intensity) / (dist * dist);
	return intensity;
}

vec4 GetPhongColorDirectionalLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor)
{
	//ambient
	vec3 ambient = lightInfo.m_color * lightInfo.m_intensity * diffuseColor.rgb;

	//diffuse
	vec3 fragNorm = normalize(fragNormal);
	vec3 lightDir = normalize(lightInfo.m_direction);

	//How direct the light impact the fragment
	float diffuseImpact = max(dot(fragNorm, lightDir), 0.0);
	vec3	diffuse = diffuseImpact * lightInfo.m_color * diffuseColor.rgb;
	
	//specular
	vec3 camDir = normalize(u_cameraInfo.m_position - fragPos);
	vec3 reflectedLightDir = reflect(-lightDir, fragNorm);

	float specImpact = pow(max(dot(camDir, reflectedLightDir), 0.0), u_material.m_shininess);
	vec3 specular = specImpact * lightInfo.m_color 
									* specularColor.rgb * u_material.m_specularStrength;

	return vec4((ambient + diffuse + specular),1.0);
}

vec4 GetPhongColorPointLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor)
{
	//No Ambient
	//vec3 ambient =  diffuseColor.rgb;

	//diffuse
	vec3 fragNorm = normalize(fragNormal);
	vec3 lightDir = normalize(lightInfo.m_position - fragPos);

	//How direct the light impact the fragment
	float diffuseImpact = max(dot(fragNorm, lightDir), 0.0);
	vec3	diffuse = diffuseImpact * lightInfo.m_color * diffuseColor.rgb;
	
	//specular
	vec3 camDir = normalize(u_cameraInfo.m_position - fragPos);
	vec3 reflectedLightDir = reflect(-lightDir, fragNorm);

	float specImpact = pow(max(dot(camDir, reflectedLightDir), 0.0), u_material.m_shininess);
	vec3 specular = specImpact * lightInfo.m_color 
									* specularColor.rgb * u_material.m_specularStrength;

	float lightIntensity = GetPointLightIntensity(fragPos
														, lightInfo.m_position, lightInfo.m_intensity);
	vec4 result = vec4((diffuse + specular) * lightIntensity
						,1.0);
	 
	return result;
}

vec4 GetPhongColorSpotLight(vec3 fragPos, vec3 fragNormal, LightInfo lightInfo
,vec4 diffuseColor, vec4 specularColor)
{
	//No Ambient
	//vec3 ambient = diffuseColor.rgb;

	//diffuse
	vec3 fragNorm = normalize(fragNormal);
	vec3 lightDir = normalize(lightInfo.m_position - fragPos);

	//How direct the light impact the fragment
	float diffuseImpact = max(dot(fragNorm, lightDir), 0.0);
	vec3	diffuse = diffuseImpact * lightInfo.m_color * diffuseColor.rgb;
	
	//specular
	vec3 camDir = normalize(u_cameraInfo.m_position - fragPos);
	vec3 reflectedLightDir = reflect(-lightDir, fragNorm);

	float specImpact = pow(max(dot(camDir, reflectedLightDir), 0.0), u_material.m_shininess);
	vec3 specular = specImpact * lightInfo.m_color 
									* specularColor.rgb * u_material.m_specularStrength;

	float lightIntensity = GetSpotLightIntensity(fragPos, lightInfo);
	vec4 result = vec4((diffuse + specular) * lightIntensity,1.0);

	//No Ambient
	//result += vec4(ambient,0.0) * max(lightIntensity, 0.2);

	return result;
}
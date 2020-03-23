#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

//***************************************
// Constants
//***************************************
const float noiseSize = 4.0;
const vec2  g_noiseScale = vec2(1366.0/noiseSize, 768.0/noiseSize);

const int   kernelSize = 64;
//***************************************
// Uniforms
//***************************************
struct GBufferResult
{
	sampler2D m_positionTex;
	sampler2D m_normalTex;
};
uniform GBufferResult u_gbufferResult;

uniform sampler2D u_noiseTexture;
uniform vec3      u_sampleKernel[kernelSize];

uniform mat4      u_view;
uniform mat4      u_projection;

//***************************************
// Exposed Parameter
//***************************************
uniform int       u_power = 1;
uniform vec3      u_ssaoColor = vec3(1.0f);
uniform float     u_sampleRadius = 0.5f;
uniform float     u_bias = 0.025f;

void main()
{ 
  //Normal in view space
  vec3 normal = texture(u_gbufferResult.m_normalTex, OurTexCoords).rgb;
	if(normal == vec3(0.0,0.0,0.0))
	{
    FragColor = vec4(1.0);
		return;
	}
  normal = (u_view * vec4(normal, 0.0)).rgb;

  //FragPos in view space
  vec3 fragPos = (u_view * texture(u_gbufferResult.m_positionTex, OurTexCoords)).rgb;

  //Sample noise
  vec3 randomVector = texture(u_noiseTexture, OurTexCoords * g_noiseScale).xyz;

  //TBN from noise vector to simulate random rotation
  vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);
  
  //Calculate Occlusion Factor
  float occlusionFactor = 0.0;
  for(int i=0; i < kernelSize; ++i)
  {
    //Offset vector in view space (from tangent space)
    vec3 samplePos = TBN * u_sampleKernel[i];

    //Sampled Position in view space
    samplePos = fragPos + (samplePos * u_sampleRadius);

    //Transform sampled position to clip-space
    vec4 clipPos = vec4(samplePos, 1.0);
    clipPos = u_projection * clipPos;       //View to clip-space
    clipPos.xyz /= clipPos.w;               //Perspective division to NDC
    clipPos.xyz = clipPos.xyz * 0.5 + 0.5;  //Remap to Screen space [0.0,1.0]

    //Sample depth from world position
    float closestDepth = (u_view * texture(u_gbufferResult.m_positionTex, clipPos.xy) ).z;

    //Rangecheck scale
    float depthDiff = abs(fragPos.z - closestDepth);
    float rangeCheck = smoothstep(0.0, 1.0, u_sampleRadius/depthDiff );

    //Check if the sampled fragment is not visible to screen
    occlusionFactor += (closestDepth >= samplePos.z + u_bias? 1.0: 0.0) * rangeCheck;
  }
  occlusionFactor = (1 - (occlusionFactor/ kernelSize) );

  occlusionFactor = pow(occlusionFactor, u_power);

  FragColor = vec4(u_ssaoColor * occlusionFactor,1.0);
}
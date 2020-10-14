#version 420 core
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
//TODO: SSAO only need reference to a single Normal GBuffer
layout(binding=0) uniform sampler2D gbuffer0;
layout(binding=1) uniform sampler2D gbuffer2;
layout(binding=2) uniform sampler2D u_depthTexture;

layout(binding=3) uniform sampler2D u_noiseTexture;
uniform vec3      u_sampleKernel[kernelSize];

uniform mat4      u_view;
uniform mat4      u_projection;
uniform mat4      u_invProjection;

//***************************************
// Exposed Parameter
//***************************************
uniform float     u_intensity = 1.0;
uniform vec3      u_ssaoColor = vec3(1.0f);
uniform float     u_sampleRadius = 0.5f;
uniform float     u_bias = 0.025f;

void UnpackNormalFromRG(inout vec3 normal)
{
	// Unpack B value from only RG
	// sqrt(x^2 + y^2 + z^2) = 1
	// z^2 = (1 - x^2 - y^2)
	// z = sqrt(1 - x^2 - y^2)
	normal.z = sqrt(1 - (normal.r * normal.r) - (normal.g * normal.g));
}

vec3 GetViewSpacePositionFromDepth(vec2 uv)
{
  float normalizedDepth = texture(u_depthTexture, uv).r;
  float z = normalizedDepth * 2.0 - 1.0; //[-1, 1]
  vec4 positionCS = vec4(uv.xy * 2.0 - 1.0, z, 1.0);

  vec4 positionVS = u_invProjection * positionCS;
  positionVS.xyz /= positionVS.w;
  return positionVS.xyz;
}

void main()
{ 
  vec2 uv = OurTexCoords;

  //FragPos in view space
  vec4 fragPosNormalX = texture(gbuffer0, uv);
  vec3 positionVS = GetViewSpacePositionFromDepth(uv);//(u_view * vec4(fragPosNormalX.xyz, 1.0)).xyz;

  //Normal in view space
  vec4 posLSAndNormalY = texture(gbuffer2, uv);
	vec3 normal = vec3(0.0);
	normal.x = fragPosNormalX.a;
	normal.y = posLSAndNormalY.a;

	UnpackNormalFromRG(normal);
	normal = normalize(normal);
  normal = (u_view * vec4(normal, 0.0)).rgb;

  //Sample noise
  vec3 randomVector = texture(u_noiseTexture, uv * g_noiseScale).xyz;

  //TBN from noise vector to simulate random rotation
  vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);
  
  //Calculate Occlusion Factor
  float occlusionFactor = 0.0;
  for(int i=0; i < kernelSize; ++i)
  {
    //Offset vector in view space (from tangent space)
    //Sampled Position in view space
    vec3 samplePosVS = positionVS + ((TBN * u_sampleKernel[i]) * u_sampleRadius);

    //Transform sampled position to clip-space
    vec4 positionNDC = vec4(samplePosVS, 1.0);
    positionNDC = u_projection * positionNDC;       //View to clip-space
    positionNDC.xyz /= positionNDC.w;               //Perspective division to NDC
    positionNDC.xyz = positionNDC.xyz * 0.5 + 0.5;  //Remap to Screen space [0.0,1.0]

    //Sample depth from world position
    float closestDepth = GetViewSpacePositionFromDepth(positionNDC.xy).z;

    //Rangecheck scale
    float depthDiff = abs(positionVS.z - closestDepth);
    float rangeCheck = smoothstep(0.0, 1.0, u_sampleRadius/depthDiff );

    //Check if the sampled fragment is not visible to screen
    occlusionFactor += (closestDepth >= samplePosVS.z + u_bias? 1.0: 0.0) * rangeCheck;
  }

  occlusionFactor = (occlusionFactor/ kernelSize);
  occlusionFactor *= u_intensity;
  occlusionFactor = 1.0 - occlusionFactor;

  FragColor = vec4(u_ssaoColor * occlusionFactor,1.0);
}
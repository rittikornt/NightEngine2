
#version 330 core
out vec4 FragColor;
in vec3 OurLocalPos;

uniform samplerCube u_cubemap;
uniform float u_roughness;

//****************************************************
// Constant
//****************************************************
const float PI = 3.14159265359;
const uint SAMPLE_COUNT = 4096u;

//****************************************************
// Function Declarations
//****************************************************
vec2 Hammersley(uint i, uint SampleSize);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness);

void main()
{		
  vec3 normal = normalize(OurLocalPos);
  vec3 reflectedDir = normal;
  vec3 viewDir = reflectedDir;

  //Directions
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = cross(up, normal);
  up = cross(normal, right);
 
  vec3  color = vec3(0.0);  //Sum result
  float totalWeight = 0.0;
  for(uint i = 0u; i < SAMPLE_COUNT; ++i)
  {
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 Halfway = ImportanceSampleGGX(Xi, normal, u_roughness);
    vec3 L = normalize(2.0 * dot(viewDir, Halfway) * Halfway - viewDir);
  
    float NdotL = max(dot(normal, L), 0.0);
    if(NdotL > 0.0)
    {
      color += texture(u_cubemap, L).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  color = color / totalWeight;

  FragColor = vec4(color, 1.0);
}

//****************************************************
// Function Definitions
//****************************************************
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint SampleSize)
{
    return vec2(float(i)/float(SampleSize), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness)
{
    float a = Roughness*Roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  
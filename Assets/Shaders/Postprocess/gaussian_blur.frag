#version 420 core
//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
//https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms

out vec4 FragColor;
  
in vec2 OurTexCoords;

layout(binding=0) uniform sampler2D u_screenTexture;
uniform vec2      u_dir;
uniform vec2      u_uvScale = vec2(1.0, 1.0);

//float g_weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

const int k_stepCount = 3;
const float g_offset[k_stepCount] = float[](0.0, 1.3846153846, 3.2307692308);
const float g_weight[k_stepCount] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{ 
  vec2 offsetAmount = (1.0 / textureSize(u_screenTexture, 0));
  vec2 uv = OurTexCoords * u_uvScale;
  vec3 resultColor = texture(u_screenTexture, uv).rgb * g_weight[0]; 
  
  for(int i = 1; i < k_stepCount; ++i)
  {
    vec2 dir = ( u_dir * offsetAmount * g_offset[i]);
    resultColor += texture(u_screenTexture, uv + dir).rgb * g_weight[i];
    resultColor += texture(u_screenTexture, uv - dir).rgb * g_weight[i];
  }
  FragColor = vec4(resultColor, 1.0);
}
#version 330 core
//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
//https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms

out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture;
uniform vec2      u_dir;

//float g_weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

const int k_stepCount = 3;
const float g_offset[k_stepCount] = float[](0.0, 1.3846153846, 3.2307692308);
const float g_weight[k_stepCount] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{ 
  vec2 offsetAmount = 1.0 / textureSize(u_screenTexture, 0);
  vec3 resultColor = texture(u_screenTexture, OurTexCoords).rgb * g_weight[0]; 
  
  for(int i = 1; i < k_stepCount; ++i)
  {
    vec2 dir = ( u_dir * offsetAmount * g_offset[i]);
    resultColor += texture(u_screenTexture, OurTexCoords + dir).rgb * g_weight[i];
    resultColor += texture(u_screenTexture, OurTexCoords - dir).rgb * g_weight[i];
  }
  FragColor = vec4(resultColor, 1.0);
}
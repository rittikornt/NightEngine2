#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture;
uniform vec2      u_dir;

float g_weight[5] = float[] 
(0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{ 
  vec2 offsetAmount = 1.0 / textureSize(u_screenTexture, 0);
  vec3 resultColor = texture(u_screenTexture, OurTexCoords).rgb * g_weight[0]; 
  
  for(int i = 1; i < 5; ++i)
  {
    vec2 dir = ( i * u_dir * offsetAmount );
    resultColor += texture(u_screenTexture, OurTexCoords + dir).rgb * g_weight[i];
    resultColor += texture(u_screenTexture, OurTexCoords - dir).rgb * g_weight[i];
  }
  FragColor = vec4(resultColor, 1.0);
}
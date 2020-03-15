#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture[5];
uniform float u_intensity = 0.2;

void main()
{ 
  vec3 color = texture(u_screenTexture[0], OurTexCoords).rgb;
  color += texture(u_screenTexture[1], OurTexCoords).rgb;
  color += texture(u_screenTexture[2], OurTexCoords).rgb;
  color += texture(u_screenTexture[3], OurTexCoords).rgb;
  color += texture(u_screenTexture[4], OurTexCoords).rgb;
  color *= u_intensity;

  FragColor = vec4(color, 1.0);
}
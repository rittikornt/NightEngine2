#version 420 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

layout(binding=0) uniform sampler2D u_lowMipTexture;
layout(binding=1) uniform sampler2D u_highMipTexture;
uniform float u_scattering = 0.5;

void main()
{ 
  vec3 l_mip = texture(u_lowMipTexture, OurTexCoords).rgb;
  vec3 h_mip = texture(u_highMipTexture, OurTexCoords).rgb;

  FragColor = vec4(mix(h_mip, l_mip, u_scattering), 1.0);
}
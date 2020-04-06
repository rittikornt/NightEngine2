#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_lowMipTexture;
uniform sampler2D u_highMipTexture;
uniform float u_scattering = 0.5;

void main()
{ 
  vec3 l_mip = texture(u_lowMipTexture, OurTexCoords).rgb;
  vec3 h_mip = texture(u_highMipTexture, OurTexCoords).rgb;

  FragColor = vec4(mix(h_mip, l_mip, u_scattering), 1.0);
}
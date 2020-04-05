#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture[5];
uniform float u_intensity = 0.2;
uniform float u_scattering = 0.5;

void main()
{ 
  vec3 mip0 = texture(u_screenTexture[0], OurTexCoords).rgb;
  vec3 mip1 = texture(u_screenTexture[1], OurTexCoords).rgb;
  vec3 mip2 = texture(u_screenTexture[2], OurTexCoords).rgb;
  vec3 mip3 = texture(u_screenTexture[3], OurTexCoords).rgb;
  vec3 mip4 = texture(u_screenTexture[4], OurTexCoords).rgb;
  
  vec3 mip34 = mix(mip3, mip4, u_scattering);
  vec3 mip23 = mix(mip2, mip3, u_scattering);
  vec3 mip12 = mix(mip1, mip2, u_scattering);
  vec3 mip01 = mix(mip0, mip1, u_scattering);
  vec3 mip04 = mix(mip0, mip4, u_scattering);
  vec3 col = (mip01 + mip12 + mip23 + mip34 + mip04) * (0.2f * u_intensity);

  //Plain Average
  //vec3 col = (mip0 + mip1 + mip2 + mip3 + mip4) * (0.2f * u_intensity);

  FragColor = vec4(col, 1.0);
}
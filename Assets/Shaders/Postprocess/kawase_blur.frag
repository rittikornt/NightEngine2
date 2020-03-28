#version 330 core
out vec4 FragColor;
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture;
uniform int       u_iteration = 0;
uniform vec2      u_uvScale = vec2(1.0, 1.0);

float g_offset[4] = float[](0.5, 1.5, 2.5, 3.5);

void main()
{ 
  vec2 texelSize = 1.0 / textureSize(u_screenTexture, 0);
  vec2 uv = OurTexCoords * u_uvScale;
  float offset = g_offset[u_iteration];

  vec3 resultColor = texture(u_screenTexture, uv + vec2( offset, offset) * texelSize.xy).rgb;
  resultColor += texture(u_screenTexture, uv + vec2(-offset, offset) * texelSize.xy).rgb;
  resultColor += texture(u_screenTexture, uv + vec2( offset,-offset) * texelSize.xy).rgb;
  resultColor += texture(u_screenTexture, uv + vec2(-offset,-offset) * texelSize.xy).rgb;
  resultColor *= 0.25;
  
  FragColor = vec4(resultColor, 1.0);
}
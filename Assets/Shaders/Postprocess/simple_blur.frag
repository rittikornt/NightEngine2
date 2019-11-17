#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture;

void main()
{
  vec2 texelSize = 1.0 / vec2(textureSize(u_screenTexture, 0));
  float result = 0.0;
  for (int x = -2; x < 2; ++x) 
  {
      for (int y = -2; y < 2; ++y) 
      {
        vec2 offset = vec2(float(x), float(y)) * texelSize;
        result += texture(u_screenTexture, OurTexCoords + offset).r;
      }
  }
  result = result / (4.0 * 4.0);
  FragColor = vec4(result,result,result, 1.0);
}
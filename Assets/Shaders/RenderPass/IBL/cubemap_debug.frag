#version 420 core

out vec4 FragColor;

in vec3 ourTexCoords;

layout(binding=0) uniform samplerCube u_cubemap;

void main()
{
  FragColor = textureLod(u_cubemap, ourTexCoords, 0.0);
  
  //FragColor = texture(u_cubemap, ourTexCoords);
}
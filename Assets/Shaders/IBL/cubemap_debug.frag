#version 330 core

out vec4 FragColor;

in vec3 ourTexCoords;

uniform samplerCube u_cubemap;

void main()
{
  FragColor = textureLod(u_cubemap, ourTexCoords, 0.0);
  
  //FragColor = texture(u_cubemap, ourTexCoords);
}
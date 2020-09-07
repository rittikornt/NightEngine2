#version 330 core

out vec4 FragColor;

in vec3 ourTexCoords;

uniform samplerCube u_cubemap;

void main()
{    
    FragColor = texture(u_cubemap, ourTexCoords);
}
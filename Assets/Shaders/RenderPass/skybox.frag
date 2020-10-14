#version 420 core

out vec4 FragColor;

in vec3 ourTexCoords;

layout(binding=0) uniform samplerCube u_cubemap;

#define HALF_MAX 65504.0

void main()
{   
    vec3 col = texture(u_cubemap, ourTexCoords).rgb;
    col = min(col, HALF_MAX);   //Clamp to float16 max
    FragColor = vec4(col.rgb, 1.0);
}
#version 330 core
layout (location = 0) in vec3 inPos;

out vec3 ourTexCoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    ourTexCoords = inPos;
    vec4 pos = u_projection * u_view * vec4(inPos, 1.0);

    //Make depthvalue to equal 1.0
    gl_Position = pos.xyww;
}  
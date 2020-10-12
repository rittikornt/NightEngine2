#version 420 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

//***************************************
// Uniforms
//***************************************
layout(binding=0) uniform sampler2D u_screenTexture;
layout(binding=1) uniform sampler2D u_ssaoTexture;

void main()
{ 
    vec3 screenColor = texture(u_screenTexture, OurTexCoords).rgb;
    vec3 ssao = texture(u_ssaoTexture, OurTexCoords).rgb;

    //Multiply with ssao
    screenColor *= ssao;
    FragColor = vec4(screenColor.xyz, 1.0);
}
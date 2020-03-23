#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

//***************************************
// Uniforms
//***************************************
uniform sampler2D u_screenTexture;

void main()
{ 
    vec3 screenColor = texture(u_screenTexture, OurTexCoords).rgb;
    FragColor = vec4(screenColor, 1.0);
}
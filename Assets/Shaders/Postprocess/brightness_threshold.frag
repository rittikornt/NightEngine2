#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform sampler2D u_screenTexture;
uniform float     u_threshold;

void main()
{ 
  vec3 color = texture(u_screenTexture, OurTexCoords).rgb;
  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

  FragColor = brightness > u_threshold? vec4(color,1.0)
    : vec4(0.0, 0.0, 0.0, 1.0);
}
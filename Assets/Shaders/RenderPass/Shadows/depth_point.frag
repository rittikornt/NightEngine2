#version 330 core
in vec4 ourFragPos;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main()
{
  //Distance between light source and this fragment
  float lightDist = length(ourFragPos.xyz - u_lightPos);

  //Map into [0,1] range
  lightDist = lightDist / u_farPlane;

  gl_FragDepth = lightDist;
}
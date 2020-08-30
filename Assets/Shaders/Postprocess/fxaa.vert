#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec2 OurTexCoords;

out vec2 v_rgbNW;
out vec2 v_rgbNE;
out vec2 v_rgbSW;
out vec2 v_rgbSE;
out vec2 v_rgbM;

uniform vec2 u_resolution;

void texcoords(vec2 fragPosSS, vec2 resolution,
			out vec2 v_rgbNW, out vec2 v_rgbNE,
			out vec2 v_rgbSW, out vec2 v_rgbSE,
			out vec2 v_rgbM) 
{
	vec2 inverseRes = 1.0 / resolution.xy;
	v_rgbNW = (fragPosSS + vec2(-1.0, -1.0)) * inverseRes;
	v_rgbNE = (fragPosSS + vec2(1.0, -1.0)) * inverseRes;
	v_rgbSW = (fragPosSS + vec2(-1.0, 1.0)) * inverseRes;
	v_rgbSE = (fragPosSS + vec2(1.0, 1.0)) * inverseRes;
	v_rgbM = vec2(fragPosSS * inverseRes);
}

void main()
{
  gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
  OurTexCoords = inTexCoord;

  vec2 fragPosSS = OurTexCoords * u_resolution;
  texcoords(fragPosSS, u_resolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
}  
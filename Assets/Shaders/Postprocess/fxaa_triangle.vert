#version 330 core
layout (location = 0) in vec2 inPos;

out vec2 OurTexCoords;

out vec2 v_rgbNW;
out vec2 v_rgbNE;
out vec2 v_rgbSW;
out vec2 v_rgbSE;
out vec2 v_rgbM;

uniform vec2 u_resolution;

vec2 TriangleVertexPosToUV(vec2 vertexPos)
{
    //Assuming the input is v{(-1,-1), (1,-1), (-1,1)}
    // v + (1.0,1.0) = {(0,0), (2,0), (0,2)}
    return (vertexPos + vec2(1.0, 1.0));// * 0.5;
}

vec2 TriangleVertexPosToScreenPos(vec2 vertexPos)
{
    //Assuming the input is {(-1,-1), (1,-1), (-1,1)}
    return (((vertexPos + vec2(1.0, 1.0)) * 0.5) * 4.0) + vec2(-1.0,-1.0);
}

void texcoords(vec2 screenPixel, vec2 resolution,
			out vec2 v_rgbNW, out vec2 v_rgbNE,
			out vec2 v_rgbSW, out vec2 v_rgbSE,
			out vec2 v_rgbM) 
{
	vec2 inverseRes = 1.0 / resolution.xy;
	v_rgbNW = (screenPixel + vec2(-1.0, -1.0)) * inverseRes;
	v_rgbNE = (screenPixel + vec2(1.0, -1.0)) * inverseRes;
	v_rgbSW = (screenPixel + vec2(-1.0, 1.0)) * inverseRes;
	v_rgbSE = (screenPixel + vec2(1.0, 1.0)) * inverseRes;
	v_rgbM = vec2(screenPixel * inverseRes);
}

void main()
{
    gl_Position = vec4(TriangleVertexPosToScreenPos(inPos.xy).xy, 0.0, 1.0); 
    OurTexCoords = TriangleVertexPosToUV(inPos.xy);

  vec2 posSS = OurTexCoords * u_resolution;
  texcoords(posSS, u_resolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
}  
#version 330 core
layout (location = 0) in vec2 inPos;

out vec2 OurTexCoords;

vec2 TriangleVertexPosToUV(vec2 vertexPos)
{
    //Assuming the input is v{(-1,-1), (1,-1), (-1,1)}
    // v + (1.0,1.0) = {(0,0), (2,0), (0,2)}
    return (vertexPos + vec2(1.0, 1.0));
}

vec2 TriangleVertexPosToScreenPos(vec2 vertexPos)
{
    //Assuming the input is {(-1,-1), (1,-1), (-1,1)}
    // v + (1.0,1.0) = {(0,0), (2,0), (0,2)}
    // v * 0.5 = {(0,0), (1,0), (0,1)}
    // v * 4.0 = {(0,0), (4,0), (0,4)}
    // v + (-1.0,-1.0) = {(0,0), (3,0), (0,3)}
    return (((vertexPos + vec2(1.0, 1.0)) * 0.5) * 4.0) + vec2(-1.0,-1.0);
}

void main()
{
    gl_Position = vec4(TriangleVertexPosToScreenPos(inPos.xy).xy, 0.0, 1.0); 
    OurTexCoords = TriangleVertexPosToUV(inPos.xy);
}  
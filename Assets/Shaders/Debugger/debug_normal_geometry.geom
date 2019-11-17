#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
} gs_in[];

out VS_OUT
{
	vec2 ourTexCoord;
	vec3 ourFragPos;
	vec3 ourFragNormal;
} gs_out;

const float LINE_LENGTH = 0.4;

void GenerateLine(int index)
{
  gs_out.ourTexCoord = gs_in[index].ourTexCoord;
  gs_out.ourFragPos = gs_in[index].ourFragPos;
  gs_out.ourFragNormal = gs_in[index].ourFragNormal;

  gl_Position = gl_in[index].gl_Position;
  EmitVertex();
  gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].ourFragNormal, 0.0) * LINE_LENGTH;
  EmitVertex();
  
  EndPrimitive();
}

void main() 
{    
  GenerateLine(0);
  GenerateLine(1);
  GenerateLine(2);
}  
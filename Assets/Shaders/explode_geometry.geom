#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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

uniform float u_time;

vec4 Explode(vec4 position, vec3 normal) 
{
  float magnitude = 2.0;
  vec3  direction = normal * ((sin(u_time) + 1.0)/2.0) * magnitude;
  return position + vec4(direction, 0.0);
}

vec3 GetNormal() 
{ 
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a,b));
}

void GenerateVertex(int index, vec3 normal)
{
  gl_Position = Explode(gl_in[index].gl_Position, normal);
  gs_out.ourTexCoord = gs_in[index].ourTexCoord;
  gs_out.ourFragPos = gs_in[index].ourFragPos;
  gs_out.ourFragNormal = gs_in[index].ourFragNormal;
  EmitVertex();
}

void main() 
{    
  vec3 normal = GetNormal();
  
  GenerateVertex(0, normal);
  GenerateVertex(1, normal);
  GenerateVertex(2, normal);

  EndPrimitive();
}  
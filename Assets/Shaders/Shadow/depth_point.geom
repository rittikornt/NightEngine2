#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_lightSpaceMatrices[6];

out vec4 ourFragPos;

void main()
{
  for(int face = 0; face < 6; ++face)
  {
    //Specifies which cubemap face to emit primitive to
    gl_Layer = face;

    //For each triangle's vertices
    for(int i=0; i < 3; ++i)
    {
      ourFragPos = gl_in[i].gl_Position;
      gl_Position = u_lightSpaceMatrices[face] * ourFragPos;
      EmitVertex();
    }
    EndPrimitive();
  }

}  
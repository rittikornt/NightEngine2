#version 420 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

uniform mat4    u_prevUnJitteredVP;
uniform mat4    u_unjitteredVP;

uniform mat4    u_invVP;

layout(binding=0) uniform sampler2D gbuffer0;
layout(binding=1) uniform sampler2D u_depthTexture;

vec3 DepthToWorldSpacePosition(float normalizedDepth, vec2 uv) 
{
    float z = normalizedDepth * 2.0 - 1.0; //[-1, 1]
    vec4 positionCS = vec4(uv.xy * 2.0 - 1.0, z, 1.0);

    vec4 positionWS = u_invVP * positionCS;
    positionWS.xyz /= positionWS.w;
    return positionWS.xyz;
}

void main()
{
  //vec4 positionWS = vec4(textureLod(gbuffer0, OurTexCoords, 0.0f).rgb, 1.0);
  float depth = textureLod(u_depthTexture, OurTexCoords, 0.0f).r;
  vec4 positionWS = vec4(DepthToWorldSpacePosition(depth, OurTexCoords).xyz,1.0);

  //[-1, 1] Clip Space position
  vec4 positionCS = u_unjitteredVP * positionWS;
  positionCS.xy /= positionCS.w;
  vec4 prevPositionCS = u_prevUnJitteredVP * positionWS;
  prevPositionCS.xy /= prevPositionCS.w;

  //Velocity from prev to current in clip space pos
  vec2 velocity = positionCS.xy - prevPositionCS.xy;
  
  // Convert velocity from Clip Space [-1,1] into NDC [0, 1]
  // Note: ((positionCS * 0.5 + 0.5) - (prevPositionCS * 0.5 + 0.5)) = (velocity * 0.5)
  velocity *= 0.5f;
  
  FragColor = vec4(velocity, 0.0f, 1.0f);
}
//***************************************
// common.glsl
//***************************************
vec3 EncodeQuantization(vec3 val, int bits)
{
    return val / ((1 << bits)-1);
}

vec3 DecodeQuantization(vec3 val, int bits)
{
    return val * ((1 << bits)-1);
}

//***************************************
// Depth Conversion
//***************************************
vec3 DepthToWorldSpacePosition(float normalizedDepth, vec2 uv, mat4 invVP) 
{
    float z = normalizedDepth * 2.0 - 1.0; //[-1, 1]
    vec4 positionCS = vec4(uv.xy * 2.0 - 1.0, z, 1.0);

    vec4 positionWS = invVP * positionCS;
    positionWS.xyz /= positionWS.w;
    return positionWS.xyz;
}
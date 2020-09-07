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
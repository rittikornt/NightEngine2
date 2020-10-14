#version 420 core
out vec4 FragColor;
in vec2 OurTexCoords;

layout(binding=0) uniform sampler2D u_screenTexture;

float Luminance(vec3 linearRgb)
{
    //Digital ITU BT.601
    return dot(linearRgb, vec3(0.2126729, 0.7151522, 0.0721750));

    //Photometric/digital ITU BT.709:
    //return dot(linearRgb, vec3(0.299, 0.587, 0.114));
}

vec3 Fetch(vec2 coords, vec2 offset)
{
    vec2 uv = coords + offset;
    return texture2D(u_screenTexture, uv).xyz;
}

vec3 Load(vec2 positionSS, int offsetX, int offsetY, vec2 texelSize)
{
    vec2 uv = (positionSS + ivec2(offsetX, offsetY)) * texelSize.xy;
    return texture2D(u_screenTexture, uv).xyz;
}

#define FXAA_SPAN_MAX           (8.0)
#define FXAA_REDUCE_MUL         (1.0 / 8.0)
#define FXAA_REDUCE_MIN         (1.0 / 128.0)

#define rcp(x) (1.0 / (x))
#define VEC2(x) vec2(x, x)
#define saturate(x) clamp(x, 0.0, 1.0)

#define Min3(x, y, z) min(min(x, y), z)
#define Max3(x, y, z) max(max(x, y), z)

vec3 FXAA(in vec2 texelSize, in vec2 positionSS);

void main()
{ 
    vec2 res = textureSize(u_screenTexture, 0);
    vec2 texelSize = vec2(1.0 / res.x, 1.0 / res.y);
    vec2 positionSS = OurTexCoords * res; 
    
    vec3 color = FXAA(texelSize, positionSS);
    FragColor = vec4(color.xyz, 1.0);
}

vec3 FXAA(in vec2 texelSize, in vec2 positionSS)
{
    //Neigbors
    vec3 color = Load(positionSS, 0, 0, texelSize);
    vec3 rgbNW = Load(positionSS, -1, -1, texelSize);
    vec3 rgbNE = Load(positionSS,  1, -1, texelSize);
    vec3 rgbSW = Load(positionSS, -1,  1, texelSize);
    vec3 rgbSE = Load(positionSS,  1,  1, texelSize);

    rgbNW = saturate(rgbNW);
    rgbNE = saturate(rgbNE);
    rgbSW = saturate(rgbSW);
    rgbSE = saturate(rgbSE);
    color = saturate(color);

    float lumaNW = Luminance(rgbNW);
    float lumaNE = Luminance(rgbNE);
    float lumaSW = Luminance(rgbSW);
    float lumaSE = Luminance(rgbSE);
    float lumaM = Luminance(color);

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float lumaSum = lumaNW + lumaNE + lumaSW + lumaSE;
    float dirReduce = max(lumaSum * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = rcp(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(VEC2(FXAA_SPAN_MAX), max(VEC2(-FXAA_SPAN_MAX), dir * rcpDirMin)) * texelSize.xy;

    // Blur
    vec3 rgb03 = Fetch(OurTexCoords, dir * (0.0 / 3.0 - 0.5));
    vec3 rgb13 = Fetch(OurTexCoords, dir * (1.0 / 3.0 - 0.5));
    vec3 rgb23 = Fetch(OurTexCoords, dir * (2.0 / 3.0 - 0.5));
    vec3 rgb33 = Fetch(OurTexCoords, dir * (3.0 / 3.0 - 0.5));

    rgb03 = saturate(rgb03);
    rgb13 = saturate(rgb13);
    rgb23 = saturate(rgb23);
    rgb33 = saturate(rgb33);

    vec3 rgbA = 0.5 * (rgb13 + rgb23);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (rgb03 + rgb33);

    float lumaB = Luminance(rgbB);

    float lumaMin = Min3(lumaM, lumaNW, Min3(lumaNE, lumaSW, lumaSE));
    float lumaMax = Max3(lumaM, lumaNW, Max3(lumaNE, lumaSW, lumaSE));

    return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;
}
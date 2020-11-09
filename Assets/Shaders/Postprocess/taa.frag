#version 420 core
out vec4 o_FragColor;
in vec2 OurTexCoords;

//***************************************
// Uniforms
//***************************************
layout(binding=0) uniform sampler2D u_currTexture;
layout(binding=1) uniform sampler2D u_prevTexture;

layout(binding=2) uniform sampler2D u_depthTexture;
layout(binding=3) uniform sampler2D u_motionVectorTexture;

// TAA Frame Index ranges from 0 to 7.
uniform vec4 u_TAAFrameInfo; // { taaSharpenStrength, taaFrameIndex, jitterUV.x, jitterUV.y }
uniform bool u_beforeTonemapping = false;
uniform bool u_blendTofilteredColor = false;

//***************************************
// Macros
//***************************************
#define NEIGHBOR_TEXEL_OFFSET  1.0
#define FEEDBACK_MIN           0.91
#define FEEDBACK_MAX           0.96

#define CLAMP_MAX              65472.0 // HALF_MAX minus one (2 - 2^-9) * 2^15
#define FLT_EPS                (0.0001f)

#define COMPARE_DEPTH(a, b) step(a, b) // a <= b? 1.0 : 0.0
#define Min3(x, y, z) min(min(x, y), z)
#define Max3(x, y, z) max(max(x, y), z)
#define rcp(x) (1.0 / (x))
#define saturate(x) clamp(x, 0.0, 1.0)
#define lerp(a,b,t) mix(a, b, t)

#define MAP(x) FastTonemap(x)
#define UNMAP(x) FastTonemapInvert(x)

//***************************************
// Color Functions
//***************************************
float Luminance(vec3 linearRgb)
{
    //Digital ITU BT.601
    //return dot(linearRgb, vec3(0.2126729, 0.7151522, 0.0721750));

    //Photometric/digital ITU BT.709:
    return dot(linearRgb, vec3(0.299, 0.587, 0.114));
}

// Fast reversible tonemapper
// http://gpuopen.com/optimized-reversible-tonemapper-for-resolve/
vec3 FastTonemap(vec3 c)
{
    return c * (rcp(Max3(c.r, c.g, c.b) + 1.0));
}

vec3 FastTonemapInvert(vec3 c)
{
    return c * (rcp(1.0 - Max3(c.r, c.g, c.b)));
}

//https://software.intel.com/content/www/us/en/develop/documentation/ipp-dev-reference/top/volume-2-image-processing/image-color-conversion/color-models.html
vec3 RGB_YCoCg(vec3 c)
{
	float Y  = dot( c, vec3(  1, 2,  1 ) );
	float Co = dot( c, vec3(  2, 0, -2 ) );
	float Cg = dot( c, vec3( -1, 2, -1 ) );
    return vec3( Y, Co, Cg) * 0.25;

    // YCoCg([0,1], [-0.5,0.5], [-0.5,0.5])
    // Y = R/4 + G/2 + B/4
    // Co = R/2 - B/2
    // Cg = -R/4 + G/2 - B/4
    //return vec3(
    //        c.x/4.0 + c.y/2.0 + c.z/4.0,
    //        c.x/2.0 - c.z/2.0,
    //    -c.x/4.0 + c.y/2.0 - c.z/4.0
    //);
}

vec3 YCoCg_RGB(vec3 c)
{
    // R = Y + Co - Cg
    // G = Y + Cg
    // B = Y - Co - Cg
    return vec3(
            (c.x + c.y - c.z),
            (c.x + c.z),
            (c.x - c.y - c.z));
    //return vec3(
    //    saturate(c.x + c.y - c.z),
    //    saturate(c.x + c.z),
    //    saturate(c.x - c.y - c.z));
}

//***************************************
// Helper Functions
//***************************************
vec3 FetchCurrTexture(vec2 uv, float offsetX, float offsetY, vec2 texelSize)
{
    uv += (vec2(offsetX, offsetY) * texelSize.xy);
    return texture(u_currTexture, uv).xyz;
}

vec3 FetchPrevTexture(vec2 uv, float offsetX, float offsetY, vec2 texelSize)
{
    uv += (vec2(offsetX, offsetY) * texelSize.xy);
    return texture(u_prevTexture, uv).xyz;
}

float LoadCameraDepth(vec2 positionSS, float offsetX, float offsetY, vec2 texelSize)
{
    vec2 uv = (positionSS + vec2(offsetX, offsetY)) * texelSize.xy;
    return texture(u_depthTexture, uv).x;
}

vec2 LoadMotionVector(vec2 positionSS, vec2 texelSize)
{
    vec2 uv = (positionSS) * texelSize.xy;
    return texture(u_motionVectorTexture, uv).xy;
}

vec2 GetClosestNeighborPositionSS(vec2 positionSS, vec2 texelSize)
{
    const float offset = NEIGHBOR_TEXEL_OFFSET;

    //Neighbor Depth (2x2 Cross)
    float center  = LoadCameraDepth(positionSS, 0, 0, texelSize);
    float sw = LoadCameraDepth(positionSS, -offset, -offset, texelSize);
    float se = LoadCameraDepth(positionSS, offset, -offset, texelSize);
    float nw = LoadCameraDepth(positionSS, -offset, offset, texelSize);
    float ne = LoadCameraDepth(positionSS, offset, offset, texelSize);

    vec4 neighborhood = vec4(sw, se, nw, ne);

    //Find closest depth
    vec3 closest = vec3(0.0, 0.0, center);
    closest = lerp(closest, vec3(-offset, -offset, neighborhood.x), COMPARE_DEPTH(neighborhood.x, closest.z));
    closest = lerp(closest, vec3( offset, -offset, neighborhood.y), COMPARE_DEPTH(neighborhood.y, closest.z));
    closest = lerp(closest, vec3(-offset,  offset, neighborhood.z), COMPARE_DEPTH(neighborhood.z, closest.z));
    closest = lerp(closest, vec3( offset,  offset, neighborhood.w), COMPARE_DEPTH(neighborhood.w, closest.z));

    return positionSS + closest.xy;
}

float IntersectAABB(vec3 dir, vec3 org, vec3 scale)
{
	vec3 rcpDir = rcp(dir);
	vec3 TNeg = (  scale  - org) * rcpDir;
	vec3 TPos = ((-scale) - org) * rcpDir;
	return max(max(min(TNeg.x, TPos.x), min(TNeg.y, TPos.y)), min(TNeg.z, TPos.z));
}

float HistoryClipBlend(vec3 filteredColor, vec3 historyColor, vec3 minColor, vec3 maxColor)
{
	vec3 min_c = min(filteredColor, min(minColor, maxColor));
	vec3 max_c = max(filteredColor, max(minColor, maxColor));	
	vec3 avg_c = (max_c + min_c) * 0.5;

	vec3 dir = filteredColor - historyColor;
	vec3 org = historyColor - avg_c;
	vec3 scale = max_c - avg_c;
	return saturate(IntersectAABB(dir, org, scale));
}

vec3 ClampHistory(vec3 filteredColor, vec3 historyColor, vec3 minColor, vec3 maxColor)
{
    float clipBlend = HistoryClipBlend(filteredColor, historyColor, minColor, maxColor);
    return lerp(historyColor, filteredColor, clipBlend);
}

vec3 FilterColor(vec2 uv, in vec3 bl, in vec3 tr, in vec3 br, in vec3 tl, in vec3 cc)
{
    //Right now is just a plain average
    //TODO: normal distribution for spatialWeight, Sigma = 0.47
    const float k_spatialWeight = 0.2;
    float sampleWeight = k_spatialWeight;
    vec3 filteredColor = (sampleWeight * cc);
    float neighborsWeight = sampleWeight;
    
    //Neighbors samples weighting
    sampleWeight = k_spatialWeight;
    filteredColor += (sampleWeight * bl);
    neighborsWeight += sampleWeight;

    sampleWeight = k_spatialWeight;
    filteredColor += (sampleWeight * tr);
    neighborsWeight += sampleWeight;

    sampleWeight = k_spatialWeight;
    filteredColor += (sampleWeight * br);
    neighborsWeight += sampleWeight;
    
    sampleWeight = k_spatialWeight;
    filteredColor += (sampleWeight * tl);
    neighborsWeight += sampleWeight;

    //Unfiltered for border pixel
    bool isBorder = max(uv.x, uv.y) >= 1.0;
    return isBorder? cc: filteredColor;
}

vec3 TAA(in vec2 texelSize, in vec2 positionSS, in vec2 screenUV, in vec2 res)
{
    float sharpenStrength = u_TAAFrameInfo.x;
    vec2 uvJitterAmount = u_TAAFrameInfo.zw;

    // Sample History texture
    // using Motion Vector of the Closest Neighbor Depth
    vec2 closestNeighborPosSS = GetClosestNeighborPositionSS(positionSS, texelSize);
    vec2 motionVector01 = LoadMotionVector(closestNeighborPosSS, texelSize);
    float motionVectorLength = length(motionVector01);
    vec3 historyColor = FetchPrevTexture(screenUV - motionVector01, 0, 0, texelSize).xyz;

    //Sample Screen texture using unjittered uv
    vec2 uv = screenUV - uvJitterAmount;
    vec3 botLeft = FetchCurrTexture(uv, -NEIGHBOR_TEXEL_OFFSET, -NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 topRight = FetchCurrTexture(uv, NEIGHBOR_TEXEL_OFFSET, NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 botRight = FetchCurrTexture(uv, NEIGHBOR_TEXEL_OFFSET, -NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 topLeft = FetchCurrTexture(uv, -NEIGHBOR_TEXEL_OFFSET, NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 currColor = FetchCurrTexture(uv, 0, 0, texelSize).xyz;

    //Sharpen filter
    if(sharpenStrength > 0.0)
    {
        vec3 blur = (botLeft + botRight + topLeft + topRight) * 0.25;
        currColor += (currColor - blur) * sharpenStrength;
    }
    currColor.xyz = clamp(currColor.xyz, 0.0, CLAMP_MAX);

    //Tonemapping HDR value to [0,1]
    if(u_beforeTonemapping)
    {
        botLeft = MAP(botLeft);
        topRight = MAP(topRight);
        botRight = MAP(botRight);
        topLeft = MAP(topLeft);
        historyColor = MAP(historyColor);
        currColor = MAP(currColor);
    }

    //RGB to YCoCg
    //(Unreal Siggraph 2014: YCoCg box) 
    botLeft = RGB_YCoCg(botLeft);
    topRight = RGB_YCoCg(topRight);
    botRight = RGB_YCoCg(botRight);
    topLeft = RGB_YCoCg(topLeft);
    historyColor = RGB_YCoCg(historyColor);
    currColor = RGB_YCoCg(currColor);

    //Adjust min/max in RGB
    //vec3 filteredColor =  (botLeft + botRight + topLeft + topRight + currColor) * 0.2;
    //vec3 filteredColor =  (currColor * 0.5) + ((botLeft + botRight + topLeft + topRight) * 0.25 * 0.5);
    vec3 filteredColor =  FilterColor(screenUV, botLeft, topRight, botRight, topLeft, currColor);

    //R is the luminance in YCoCg
    float currColorLuma = currColor.r;
    float historyLuma = historyColor.r;

    vec3 minColor = min(min(min(min(botLeft, topRight), botRight), topLeft), filteredColor);
    vec3 maxColor = max(max(max(max(botLeft, topRight), botRight), topLeft), filteredColor);
    float lumaMin = minColor.x;
    float lumaMax = maxColor.x;
    
    // shrink chroma min-max (YCOCG)
    vec2 chroma_extent = vec2(0.25 * 0.5 * abs(maxColor.r - minColor.r));
    vec2 chroma_center = filteredColor.gb;
    minColor.yz = chroma_center - chroma_extent;
    maxColor.yz = chroma_center + chroma_extent;
    //filteredColor.yz = chroma_center;

    //Clamp reprojected prev frame to current Neighborhood to reject false reprojection
    historyColor = ClampHistory(filteredColor, historyColor, minColor, maxColor);

    //Setting target color to blend against
    vec3 targetColor = currColor.xyz;
    if(u_blendTofilteredColor)
    {
        //Smoothing transition at the high contrast area of currColor
        const float k_blurAmp = 2.0;
		float historyBlur = saturate(abs(motionVector01.x) * k_blurAmp + abs(motionVector01.y) * k_blurAmp);
        
        float aliasing = saturate(historyBlur) * 0.5;
        const float k_lumaContrastFactor = 32.0 * 4;
		float lumaContrast = lumaMax - lumaMin;
        aliasing = saturate(aliasing + rcp(1.0 + lumaContrast * k_lumaContrastFactor));
        targetColor = lerp(filteredColor, currColor, aliasing);
    }
    float targetColorLuma = targetColor.x;

    // Blend Color
    // Timothy Lottes (weighing by unbiased luminance diff; 
    // http://www.youtube.com/watch?v=WzpLWzGvFK4&t=18m)
    float diff = abs(targetColorLuma - historyLuma) / Max3(targetColorLuma, historyLuma, 0.2);
    float weight = 1.0 - diff;
    float feedback = lerp(FEEDBACK_MIN, FEEDBACK_MAX, weight * weight);
    currColor = lerp(targetColor.xyz, historyColor.xyz, feedback);

    //YCoCg to RGB
    currColor = YCoCg_RGB(currColor);

    //Revert tonemapping back to HDR range
    if(u_beforeTonemapping)
    {
        //force [0, 0.999] range so it doesn't goes to infinity when UNMAP
        currColor = UNMAP(currColor * 0.999);
    }

    currColor = clamp(currColor.xyz, 0.0, CLAMP_MAX);
    return currColor;
}

//***************************************
// Main
//***************************************
void main()
{ 
    vec2 res = textureSize(u_currTexture, 0);
    vec2 texelSize = vec2(1.0 / res.x, 1.0 / res.y);
    vec2 positionSS = OurTexCoords * res; 
    
    vec3 color = TAA(texelSize, positionSS, OurTexCoords, res);
    o_FragColor = vec4(color.xyz, 1.0);
}
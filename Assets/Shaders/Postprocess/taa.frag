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

//***************************************
// Macros
//***************************************
#define NEIGHBOR_TEXEL_OFFSET  1.0
#define FEEDBACK_MIN           0.91
#define FEEDBACK_MAX           0.96

#define CLAMP_MAX              65472.0 // HALF_MAX minus one (2 - 2^-9) * 2^15

#define COMPARE_DEPTH(a, b) step(a, b) // a <= b? 1.0 : 0.0
#define Min3(x, y, z) min(min(x, y), z)
#define Max3(x, y, z) max(max(x, y), z)
#define rcp(x) (1.0 / (x))
#define saturate(x) clamp(x, 0.0, 1.0)
#define lerp(a,b,t) mix(a, b, t)

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
    // Y = R/4 + G/2 + B/4
    // Co = R/2 - B/2
    // Cg = -R/4 + G/2 - B/4
    return vec3(
            c.x/4.0 + c.y/2.0 + c.z/4.0,
            c.x/2.0 - c.z/2.0,
        -c.x/4.0 + c.y/2.0 - c.z/4.0
    );
}

vec3 YCoCg_RGB(vec3 c)
{
    // R = Y + Co - Cg
    // G = Y + Cg
    // B = Y - Co - Cg
    return vec3(
        saturate(c.x + c.y - c.z),
        saturate(c.x + c.z),
        saturate(c.x - c.y - c.z));
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
    vec2 uv = positionSS + (vec2(offsetX, offsetY) * texelSize.xy);
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

    //Neighbor Depth
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

vec3 ClipToAABB(vec3 color, vec3 minColor, vec3 maxColor)
{
    // note: only clips towards aabb center (but fast!)
    vec3 center  = 0.5 * (maxColor + minColor);
    vec3 extents = 0.5 * (maxColor - minColor);
    
    // This is actually `distance`, however the keyword is reserved
    vec3 dist = (color - center);

    vec3 ts = abs(extents) / max(abs(dist), 1e-4);
    float t = clamp(Min3(ts.x, ts.y, ts.z), 0.0, 1.0);
    return center + (dist * t);
}

vec3 TAA(in vec2 texelSize, in vec2 positionSS, in vec2 screenUV)
{
    float sharpenStrength = u_TAAFrameInfo.x;
    vec2 uvJitterAmount = u_TAAFrameInfo.zw;

    // Sample History texture
    // using Motion Vector of the Closest Neighbor Depth
    vec2 closestNeighborPosSS = GetClosestNeighborPositionSS(positionSS, texelSize);
    vec2 motionVector = LoadMotionVector(closestNeighborPosSS, texelSize);
    float motionVectorLength = length(motionVector);
    vec3 historyColor = FetchPrevTexture(screenUV - motionVector, 0, 0, texelSize).xyz;

    //Sample Screen texture using unjittered uv
    vec2 uv = screenUV - uvJitterAmount;
    vec3 botLeft = FetchCurrTexture(uv, -NEIGHBOR_TEXEL_OFFSET, -NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 topRight = FetchCurrTexture(uv, NEIGHBOR_TEXEL_OFFSET, NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 botRight = FetchCurrTexture(uv, NEIGHBOR_TEXEL_OFFSET, -NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    vec3 topLeft = FetchCurrTexture(uv, -NEIGHBOR_TEXEL_OFFSET, NEIGHBOR_TEXEL_OFFSET, texelSize).xyz;
    
    vec3 currColor = FetchCurrTexture(uv, 0, 0, texelSize).xyz;
    currColor.xyz = clamp(currColor.xyz, 0.0, CLAMP_MAX);

    //Tonemapping HDR value (to avoid big outlier)
    botLeft = FastTonemap(botLeft);
    topRight = FastTonemap(topRight);
    botRight = FastTonemap(botRight);
    topLeft = FastTonemap(topLeft);
    historyColor = FastTonemap(historyColor);
    currColor = FastTonemap(currColor);

    //Sharpen filter
    if(sharpenStrength > 0.0)
    {
        vec3 blur = (botLeft + botRight + topLeft + topRight) * 0.25;
        currColor += (currColor - blur) * sharpenStrength;
    }

    vec3 corners = 4.0 * (botLeft + topRight) - 2.0 * currColor;
    vec3 average = ((corners.xyz + currColor.xyz) * 0.14285714285);
    //vec3 average =  (botLeft + botRight + topLeft + topRight + currColor) * 0.2;

    //AABB Clipping
    float currColorLuma = Luminance(currColor.xyz);
    float averageLuma = Luminance(average.xyz);
    float nudge = lerp(4.0, 0.25, saturate(motionVectorLength * 100.0)) * abs(averageLuma - currColorLuma);
    
    //Clamp reprojected prev frame to current Neighborhood to reject false reprojection
    vec3 minColor = min(topRight, botLeft) - nudge;
    vec3 maxColor = max(botLeft, topRight) + nudge;
    historyColor = ClipToAABB(historyColor, minColor, maxColor);
    //historyColor = clamp(historyColor, minColor, maxColor);
    
    // Blend Color
    // Timothy Lottes (weighing by unbiased luminance diff; 
    // http://www.youtube.com/watch?v=WzpLWzGvFK4&t=18m)
    float historyLuma = Luminance(historyColor.xyz);
    float diff = abs(currColorLuma - historyLuma) / Max3(currColorLuma, historyLuma, 0.2);
    float weight = 1.0 - diff;
    float feedback = lerp(FEEDBACK_MIN, FEEDBACK_MAX, weight * weight);

    currColor = FastTonemapInvert(lerp(currColor.xyz, historyColor.xyz, feedback));
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
    
    vec3 color = TAA(texelSize, positionSS, OurTexCoords);
    o_FragColor = vec4(color.xyz, 1.0);
}
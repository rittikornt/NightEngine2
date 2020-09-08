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
uniform vec4 u_TAAFrameInfo;               // { taaSharpenStrength, taaFrameIndex, jitterUV.x, jitterUV.y }

//***************************************
// Macros
//***************************************
#define RADIUS              0.75
#define FEEDBACK_MIN        0.96
#define FEEDBACK_MAX        0.91

#define CLAMP_MAX           65472.0 // HALF_MAX minus one (2 - 2^-9) * 2^15

#define COMPARE_DEPTH(a, b) step(a, b) // a <= b? 1.0 : 0.0
#define Min3(x, y, z) min(min(x, y), z)
#define Max3(x, y, z) max(max(x, y), z)
#define rcp(x) 1.0 / (x)
#define saturate(x) clamp(x, 0.0, 1.0)
#define lerp(a,b,t) mix(a, b, t)

//***************************************
// Helper Functions
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
    return c * rcp(Max3(c.r, c.g, c.b) + 1.0);
}

vec3 FastTonemapInvert(vec3 c)
{
    return c * rcp(1.0 - Max3(c.r, c.g, c.b));
}

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

float LoadCameraDepth(vec2 positionSS, int offsetX, int offsetY, vec2 texelSize)
{
    vec2 uv = (positionSS + ivec2(offsetX, offsetY)) * texelSize.xy;
    return texture(u_depthTexture, uv).x;
}

vec2 LoadMotionVector(vec2 positionSS, vec2 texelSize)
{
    vec2 uv = (positionSS) * texelSize.xy;
    return texture(u_motionVectorTexture, uv).xy;
}

vec2 GetClosestNeighborPositionSS(vec2 positionSS, vec2 texelSize)
{
    //Neighbor Depth
    float center  = LoadCameraDepth(positionSS, 0, 0, texelSize);
    float sw = LoadCameraDepth(positionSS, -1, -1, texelSize);
    float se = LoadCameraDepth(positionSS, 1, -1, texelSize);
    float nw = LoadCameraDepth(positionSS, -1, 1, texelSize);
    float ne = LoadCameraDepth(positionSS, 1, 1, texelSize);

    vec4 neighborhood = vec4(sw, se, nw, ne);

    //Find closest depth
    vec3 closest = vec3(0.0, 0.0, center);
    closest = lerp(closest, vec3(-1.0, -1.0, neighborhood.x), COMPARE_DEPTH(neighborhood.x, closest.z));
    closest = lerp(closest, vec3( 1.0, -1.0, neighborhood.y), COMPARE_DEPTH(neighborhood.y, closest.z));
    closest = lerp(closest, vec3(-1.0,  1.0, neighborhood.z), COMPARE_DEPTH(neighborhood.z, closest.z));
    closest = lerp(closest, vec3( 1.0,  1.0, neighborhood.w), COMPARE_DEPTH(neighborhood.w, closest.z));

    return positionSS + closest.xy;
}

vec3 ClipToAABB(vec3 color, vec3 minColor, vec3 maxColor)
{
    // note: only clips towards aabb center (but fast!)
    vec3 center  = 0.5 * (maxColor + minColor);
    vec3 extents = 0.5 * (maxColor - minColor);

    // This is actually `distance`, however the keyword is reserved
    vec3 offset = color - center;
    
    vec3 ts = abs(extents) / max(abs(offset), 1e-4);
    float t = saturate(Min3(ts.x, ts.y,  ts.z));
    return center + offset * t;
}

vec3 TAA(in vec2 texelSize, in vec2 positionSS)
{
    float sharpenStrength = u_TAAFrameInfo.x;
    vec2 uvJitterAmount = u_TAAFrameInfo.zw;

    // Sample History texture
    // using Motion Vector of the Closest Neighbor Depth
    vec2 closestNeighborPosSS = GetClosestNeighborPositionSS(positionSS, texelSize);
    vec2 motionVector = LoadMotionVector(closestNeighborPosSS, texelSize);
    float motionVectorLength = length(motionVector);
    vec3 historyColor = FetchPrevTexture(OurTexCoords - motionVector, 0, 0, texelSize).xyz;

    //Sample Screen texture using unjittered uv
    vec2 uv = OurTexCoords - uvJitterAmount;
    vec3 currColor = FetchCurrTexture(uv, 0, 0, texelSize).xyz;
    vec3 topLeft = FetchCurrTexture(uv, -RADIUS, -RADIUS, texelSize).xyz;
    vec3 bottomRight = FetchCurrTexture(uv, RADIUS, RADIUS, texelSize).xyz;

    vec3 corners = 4.0 * (topLeft + bottomRight) - (2.0 * currColor);

    //All pixels should be executing the same branch, so it should be fine
    if(sharpenStrength > 0.0)
    {
        vec3 topRight = FetchCurrTexture(uv, RADIUS, -RADIUS, texelSize).xyz;
        vec3 bottomLeft = FetchCurrTexture(uv, -RADIUS, RADIUS, texelSize).xyz;
        vec3 blur = (topLeft + topRight + bottomLeft + bottomRight) * 0.25;
        currColor += (currColor - blur) * sharpenStrength;
    }

    //AABB Clipping
    currColor.xyz = clamp(currColor.xyz, 0.0, CLAMP_MAX);
    vec3 average = (corners.xyz + currColor.xyz) * 0.14285714285;

    float currColorLuma = Luminance(currColor.xyz);
    float averageLuma = Luminance(average.xyz);
    float nudge = lerp(4.0, 0.25, saturate(motionVectorLength * 100.0)) * abs(averageLuma - currColorLuma);
    
    vec3 minColor = min(bottomRight, topLeft) - nudge;
    vec3 maxColor = max(topLeft, bottomRight) + nudge;
    historyColor = ClipToAABB(historyColor, minColor, maxColor);

    // Blend Color
    // Timothy Lottes (weighing by unbiased luminance diff; 
    // http://www.youtube.com/watch?v=WzpLWzGvFK4&t=18m)
    float historyLuma = Luminance(historyColor.xyz);
    float diff = abs(currColorLuma - historyLuma) / Max3(currColorLuma, historyLuma, 0.2);
    float weight = 1.0 - diff;
    float feedback = lerp(FEEDBACK_MIN, FEEDBACK_MAX, weight * weight);

    currColor = lerp(currColor.xyz, historyColor.xyz, feedback);
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
    
    vec3 color = TAA(texelSize, positionSS);
    o_FragColor = vec4(color.xyz, 1.0);
}
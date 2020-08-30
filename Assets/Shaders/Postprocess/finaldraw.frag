#version 330 core
out vec4 FragColor;
  
in vec2 OurTexCoords;

//***************************************
// Uniforms
//***************************************
uniform sampler2D u_screenTexture;
uniform sampler2D u_bloomTexture;
uniform sampler2D u_ssaoTexture;

uniform float     u_exposure;
uniform float     u_time;

vec4 GammaCorrection(vec3 color, float gammaValue)
{
    return vec4(pow(color, vec3(1.0/gammaValue)), 1.0);
}

vec4 GammaCorrection(vec3 color)
{
    float gammaValue = 2.2;
    return vec4(pow(color, vec3(1.0/gammaValue)), 1.0);
}

vec3 Tonemap(vec3 color)
{
    return color/ (color + vec3(1.0));
}

vec3 TonemapWithExposure(vec3 color, float exposure)
{
    return vec3(1.0) - exp(-color * exposure);
}

float InterleavedGradientNoise( vec2 uv )
{
    const vec3 magic = vec3( 0.06711056, 0.00583715, 52.9829189 );
    return fract( magic.z * fract( dot( uv, magic.xy ) ) );
}

//Unreal Filmic Tone Mapping
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0f, 1.0f);
}

void main()
{ 
    vec3 screenColor = texture(u_screenTexture, OurTexCoords).rgb;
    vec3 bloomColor = texture(u_bloomTexture, OurTexCoords).rgb;
    vec3 ssao = texture(u_ssaoTexture, OurTexCoords).rgb;

    //Multiply with ssao
    screenColor *= ssao;

    //Addictive Blend
    screenColor += bloomColor;
    
    //Tonemapping + GammaCorrection
    //screenColor = TonemapWithExposure(screenColor, u_exposure);
    screenColor = ACESFilm(screenColor * u_exposure);
    FragColor = GammaCorrection(screenColor);

    //Dithering
    //https://www.shadertoy.com/view/MslGR8
    vec2 seed = gl_FragCoord.xy;
    seed += 1337.0*fract(u_time);
    float rnd = InterleavedGradientNoise( seed );
    FragColor += vec4(vec3(rnd, 1.0-rnd, rnd)/255.0, 0.0);

    //FragColor = vec4(1.0,0.0,0.0,1.0);
    //float depthValue = texture(u_screenTexture, OurTexCoords).r;
    //FragColor = vec4( vec3(depthValue), 1.0);

    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);
}
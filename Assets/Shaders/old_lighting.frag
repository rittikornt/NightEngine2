/////////////////////////////////////////////////////////////////////////
// Pixel shader for lighting
////////////////////////////////////////////////////////////////////////
#version 330

// These definitions agree with the ObjectIds enum in scene.h
const int     nullId	= 0;
const int     skyId	= 1;
const int     seaId	= 2;
const int     groundId	= 3;
const int     wallId	= 4;
const int     boxId	= 5;
const int     frameId	= 6;
const int     lPicId	= 7;
const int     rPicId	= 8;
const int     teapotId	= 9;
const int     spheresId	= 10;

//Modes
const int	MODE_COLOR = 0;
const int	MODE_TEXTURE = 1;
const int	MODE_NORMALMAP = 2;
const int	MODE_SKYDOME = 3;

in vec3 normalVec, lightVec, eyeVec, tanVec;
in vec2 texCoord;

uniform int objectId;
uniform vec3 diffuse, specular, Light, Ambient;
uniform float shininess;
uniform sampler2D tex, normalTex;
uniform int mode;


vec3 MicroFacetBRDF(vec3 NormalVec,vec3 LightVec,vec3 EyeVec, vec3 Kd, vec3 Ks, vec3 Ia, vec3 I, float a);
vec3 PhoneLighting(vec3 LightVec, vec3 NormalVec, vec3 EyeVec, vec3 Ia, vec3 Kd, vec3 Ks, vec3 I, float a);

void main()
{

	if(mode == MODE_COLOR)
	{
		vec3 Kd = diffuse;
		if (objectId==groundId || objectId==seaId) 
		{
			ivec2 uv = ivec2(floor(200.0*texCoord));
			if ((uv[0]+uv[1])%2==0)
			{
				Kd *= 0.9; 
			}
		}
		gl_FragColor.xyz = MicroFacetBRDF(normalVec, lightVec, eyeVec, Kd, specular,Ambient, Light, shininess);
	}
	else if(mode == MODE_TEXTURE)
	{
		vec2 uv = texCoord;
		switch(objectId)
		{
			case boxId:
				uv = texCoord.yx;
				uv *= 2.0f;
			break;
			case wallId:
				uv = texCoord.yx;
				uv *= 22.0f;
			break;
			case groundId:
				uv *= 50.0f;
			break;
			case frameId:
				uv *= 20.0f;
			break;
			case lPicId:
			break;
			case rPicId:
				uv -= 0.925f;
				uv *= 1.2f;
			break;
		}

		vec4 color = texture(tex, uv);

		switch(objectId)
		{
			case rPicId:
				if( (texCoord.x < 0.1f) || (texCoord.x > 0.9f) 
				|| (texCoord.y > 0.9f) || (texCoord.y < 0.1f))
				{
					color.xyz = vec3(0.5f,0.5f,0.5f);
				}
			break;
			case lPicId:
				
				ivec2 uv = ivec2(floor(10.0*texCoord));
				if ((uv.x + uv.y) % 2 == 0)
				{
					color.xyz = vec3(1.0f,1.0f,1.0f);
				}
				else
				{
					color.xyz = vec3(0.0f,0.0f,0.0f);
				}
			break;
			case teapotId:
					color.xyz = vec3(0.7f,0.7f,0.7f);
			break;
			case seaId:
					color.xyz = diffuse;
			break;
		}
		gl_FragColor.xyz = MicroFacetBRDF(normalVec, lightVec, eyeVec, color.xyz, specular,Ambient, Light, shininess);
	}
	else if(mode == MODE_NORMALMAP)
	{
		vec2 uv = texCoord;
		
		switch(objectId)
		{
			case boxId:
				uv = texCoord.yx;
				uv *= 2.0f;
			break;
			case wallId:
				uv = texCoord.yx;
				uv *= 22.0f;
			break;
			case groundId:
				uv *= 50.0f;
			break;
			case frameId:
				uv *= 20.0f;
			break;
			case lPicId:
			break;
			case rPicId:
				uv -= 0.925f;
				uv *= 1.2f;
			break;
		}

		vec3 delta = texture(normalTex, uv).xyz;
		delta = normalize( (delta * 2.0) - vec3(1,1,1));

		vec3 N = normalize(normalVec);
		vec3 T = normalize(tanVec);
		//T = normalize(T - dot(T, N) * N);
		vec3 B = normalize(cross(T,N));
		gl_FragColor.xyz = abs(T);
		return;
		N = (delta.x * T) - (delta.y *B) + (delta.z *N);

		vec4 color = texture(tex, uv);
		gl_FragColor.xyz = MicroFacetBRDF(normalVec, lightVec, eyeVec, color.xyz, specular,Ambient, Light, shininess);
	}
	else
	{
		vec3 V = normalize(eyeVec);
		vec2 uv =  vec2(0.5 - atan(V.y, V.x) * (0.15915) , acos(V.z) * 0.31831 );
		
		vec4 color = texture(tex, uv);
		gl_FragColor = color;
	}
}


vec3 MicroFacetBRDF(vec3 NormalVec,vec3 LightVec,vec3 EyeVec, vec3 Kd, vec3 Ks, vec3 Ia, vec3 I, float a)
{
	vec3 N = normalize(NormalVec);
    vec3 L = normalize(LightVec);
	vec3 V = normalize(EyeVec);
	vec3 H = normalize(L + V);
	float LN = max(dot(L, N), 0.0);
	float HN = max(dot(H, N), 0.0);
	float LH = max(dot(L, H), 0.0);

	//Hardcoded 1/pi and 2*pi
	vec3 brdf = (Kd*0.31830988618) + 
	(
		(
			//F(L,H)
			(Ks + (vec3(1,1,1) - Ks)*pow( (1.0-LH),5 ) ) *
			//G(L,V,H)/ (LN)(VN)
			( 1/ (pow(LH, 2)) ) *
			//D(H)
			( ( (a + 2)/(6.283185) ) * pow(HN , a) )
		)/4.0 
	);

	return vec3 ( (Ia * Kd) + (I * LN)  * brdf  );
}


vec3 PhoneLighting(vec3 LightVec, vec3 NormalVec, vec3 EyeVec, vec3 Ia, vec3 Kd, vec3 Ks, vec3 I, float a)
{
	vec3 N = normalize(NormalVec);
    vec3 L = normalize(LightVec);
	vec3 V = normalize(EyeVec);
	vec3 H = normalize(L + V);
	float LN = max(dot(L, N), 0.0);
	float HN = max(dot(H, N), 0.0);
	float LH = max(dot(L, H), 0.0);

	//Phong Lighting
	return vec3( (Ia*Kd) + (I * Kd)*(LN) + (I * Ks) * pow(HN, a) );

}

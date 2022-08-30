#include "Shader_Defines.hlsli"

texture2D g_texDiffuse;


texture2D	g_texDefault;
texture2D	g_texAlphaOne;
texture2D	g_texMask;
texture2D	g_texDissolve;

float		g_fFadeIn;
int			g_iIsFadeIn;
float		g_fFadeOut;
int			g_iIsFadeOut;


float3		g_fIsTexture;
float		g_fDissolve;

float2		g_vUVAnimation;
int			g_iIsUVAnimation = 0;
int			g_iIsGlow = 0;
int			g_iIsDistortion = 0;

sampler DefaultSampler = sampler_state {
	//AddressU = wrap;
	//AddressV = wrap;
};

struct VS_IN {
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN {
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT {
	vector		vDiffuse : SV_TARGET0;
	vector		vGlow : SV_TARGET1;
	vector		vDistortion : SV_TARGET2;
	vector		vDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	//if (Out.vDiffuse.a <= 0.1f)
	//	discard;

	if (0 != g_iIsGlow)
	{
		Out.vGlow = Out.vDiffuse;
	}

	if (0 != g_iIsDistortion)
	{
		Out.vDistortion = Out.vDiffuse;
		Out.vDiffuse *= 0.2f;
	}

	//if (Out.vDiffuse.a <= 0.1f)
	//	discard;


	if (g_iIsFadeIn)
	{
		if (Out.vDiffuse.a > g_fFadeIn)
		{
			Out.vDiffuse.a = g_fFadeIn;
		}
	}

	if (g_iIsFadeOut)
	{
		if (Out.vDiffuse.a > g_fFadeOut) Out.vDiffuse.a = g_fFadeOut;
	}

	return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	//Out.vGlow = In.vPosition;

	vector vDissolve = g_texDissolve.Sample(DefaultSampler, In.vTexUV);

	if (Out.vDiffuse.a <= 0.1f)
		discard;


	if (vDissolve.r < g_fDissolve)
		discard;

	if (vDissolve.r < g_fDissolve)
		Out.vDiffuse.a = 1.f;
	else
		Out.vDiffuse.a = 0.f;

	if (g_fDissolve >= vDissolve.r - 0.09f && g_fDissolve <= vDissolve.r + 0.09)
		Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.05f && g_fDissolve <= vDissolve.r + 0.05)
		Out.vDiffuse = vector(1.f, 1.f, 0.f, 1.f);

	if (g_fDissolve >= vDissolve.r - 0.01f && g_fDissolve <= vDissolve.r + 0.01)
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);


	return Out;
}


PS_OUT PS_IMAGELESS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector vAlphaOneColor = (vector)0;
	vector vMaskColor = (vector)0;

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	if (g_fIsTexture.x)
	{
		if (g_iIsUVAnimation)
		{
			float2 vUV;

			vUV.xy = In.vTexUV.xy + g_vUVAnimation.xy;
			if (vUV.x > 1.f)
				vUV.x -= 1.f;

			if (vUV.y > 1.f)
				vUV.y -= 1.f;

			Out.vDiffuse = g_texDefault.Sample(DefaultSampler, vUV);
		}
		else
		{
			Out.vDiffuse = g_texDefault.Sample(DefaultSampler, In.vTexUV);
		}
	}

	if (Out.vDiffuse.a == 0.f)
	{
		Out.vDiffuse.rgb = 0.f;
	}


	if (g_fIsTexture.y)
		vAlphaOneColor = g_texAlphaOne.Sample(DefaultSampler, In.vTexUV);

	if (g_fIsTexture.z)
		vMaskColor = g_texMask.Sample(DefaultSampler, In.vTexUV);

	// if Default Texture is NOT null
	if (g_fIsTexture.x != 0.f)
	{
		// If MaskTexture exists
		if (g_fIsTexture.z != 0.f)
		{
			if (vMaskColor.a == 0.f)
			{
				discard;
			}
		}
		// If MaskTexture exists
		if (g_fIsTexture.y != 0.f)
		{
			if (vAlphaOneColor.a != 0.f)
			{
				Out.vDiffuse.rgb = Out.vDiffuse.rgb * 0.2 + vAlphaOneColor.rgb * 0.8;
			}
		}
	}
	// else if Default Texture is null
	else
	{
		if (In.vTexUV.x <= 0.02f || In.vTexUV.x >= 0.98f
			|| In.vTexUV.y <= 0.02f || In.vTexUV.y >= 0.98f
			)
		{
			Out.vDiffuse.r = 0.5f;
			Out.vDiffuse.g = 0.f;
			Out.vDiffuse.b = 0.f;
			Out.vDiffuse.a = 0.5f;
		}
		else
		{
			discard;
		}

	}


	if (0 != g_iIsGlow)
	{
		Out.vGlow = Out.vDiffuse;
	}
	if (0 != g_iIsDistortion)
	{
		Out.vDistortion = Out.vDiffuse;
		Out.vDiffuse.rgb *= 0.5f;
	}


	if (g_iIsFadeIn)
	{
		if (Out.vDiffuse.a > g_fFadeIn)
		{
			Out.vDiffuse.a = g_fFadeIn;
		}
	}

	if (g_iIsFadeOut)
	{
		if (Out.vDiffuse.a > g_fFadeOut) Out.vDiffuse.a = g_fFadeOut;
	}

	if (Out.vDiffuse.a <= 0.1f)
		discard;

	return Out;
}


technique11		DefaultTechnique {
	pass ImageEffect {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass ImagelessEffect {
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_IMAGELESS();
	}

	pass ImagelessEffect_WireFrame {
		SetRasterizerState(RS_WireFrame);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_IMAGELESS();
	}

	pass ImageEffect_Dissolve {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}

};

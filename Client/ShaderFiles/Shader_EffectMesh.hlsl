#include "Shader_Defines.hlsli"

texture2D g_texDiffuse;


texture2D	g_texDefault;
texture2D	g_texAlphaOne;
texture2D	g_texMask;

float3		g_fIsTexture;


sampler DefaultSampler = sampler_state {
	AddressU = wrap;
	AddressV = wrap;
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
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	if (Out.vDiffuse.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_IMAGELESS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector vAlphaOneColor = (vector)0;
	vector vMaskColor = (vector)0;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);

	if (g_fIsTexture.x)
		Out.vDiffuse = g_texDefault.Sample(DefaultSampler, In.vTexUV);

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
				Out.vDiffuse.rgb += vAlphaOneColor.rgb;
				Out.vDiffuse.rgb *= 0.5f;
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
		SetRasterizerState(RS_Default);
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
	
};

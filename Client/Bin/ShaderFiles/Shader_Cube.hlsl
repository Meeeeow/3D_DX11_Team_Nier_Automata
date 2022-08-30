#include "Shader_Defines.hlsli"


textureCUBE	g_tex3D_Diffuse;
texture2D	g_texDiffuse;
texture2D	g_texDepth;

bool		g_bAlpha = false;
bool		g_bAppear = false;

cbuffer tagAlphaDesc {
	float		g_fAlpha;
	float		g_fIncreaseAlpha;
	int			g_iIsGlow = 0;
	int			g_iIsColorGlow = 0;
	float4		g_vGlowColor = (float4)0;
};


sampler DefaultSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN {
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT {
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN {
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT {
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_tex3D_Diffuse.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


struct VS_IN_Decal {
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT_Decal {
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_OUT_Decal VS_DECAL(VS_IN_Decal In)
{
	VS_OUT_Decal		Out = (VS_OUT_Decal)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_Decal {
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT_Decal {
	vector		vColor : SV_TARGET0;
	vector		vGlow : SV_TARGET1;
};

PS_OUT_Decal PS_DECAL(PS_IN_Decal In)
{
	PS_OUT_Decal Out = (PS_OUT_Decal)0;

	float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepth = g_texDepth.Sample(DefaultSampler, vTexUV);
	float	fViewZ = vDepth.y * g_fFar;

	vector vProjPos = (vector)0.f;

	vProjPos.x = (vTexUV.x *  2.f  - 1.f) * fViewZ;
	vProjPos.y = (vTexUV.y * -2.f + 1.f) * fViewZ;
	vProjPos.z = (vDepth.x) * fViewZ;
	vProjPos.w = fViewZ;

	vector vViewPos = mul(vProjPos, g_matProjInv);
	vector vWorldPos = mul(vViewPos, g_matViewInv);
	vector vLocalPos = mul(vWorldPos, g_matWorldInv);

	float3 DecalAbsPos = abs(vLocalPos.xyz);

	clip(0.5f - DecalAbsPos);

	float2 vDecalUV = vLocalPos.xz + 0.5f;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, vDecalUV);

	
	[flatten]
	if (Out.vColor.a <= 0.01f)
		discard;

	[flatten]
	if (g_bAlpha)
	{
		Out.vColor.a -= g_fAlpha;
	}

	if (0 != g_iIsGlow && 0 == g_iIsColorGlow) 
	{
		Out.vGlow = Out.vColor;
	}
	else if (0 != g_iIsColorGlow && 0 == g_iIsGlow)
	{
		Out.vGlow.rgb = (g_vGlowColor.rgb + Out.vColor.rgb) / 2.f;
		Out.vGlow.a = Out.vColor.a;
	}

	return Out;
}

PS_OUT_Decal PS_DECAL_INCREASE(PS_IN_Decal In)
{
	PS_OUT_Decal Out = (PS_OUT_Decal)0;

	float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepth = g_texDepth.Sample(DefaultSampler, vTexUV);
	float	fViewZ = vDepth.y * g_fFar;

	vector vProjPos = (vector)0.f;

	vProjPos.x = (vTexUV.x * 2.f - 1.f) * fViewZ;
	vProjPos.y = (vTexUV.y * -2.f + 1.f) * fViewZ;
	vProjPos.z = (vDepth.x) * fViewZ;
	vProjPos.w = fViewZ;

	vector vViewPos = mul(vProjPos, g_matProjInv);
	vector vWorldPos = mul(vViewPos, g_matViewInv);
	vector vLocalPos = mul(vWorldPos, g_matWorldInv);

	float3 DecalAbsPos = abs(vLocalPos.xyz);
	clip(0.5f - DecalAbsPos);

	float2 vDecalUV = vLocalPos.xz + 0.5f;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, vDecalUV);


	[flatten]
	if (Out.vColor.a <= 0.01f)
		discard;

	(g_fIncreaseAlpha < 1.f) ? (Out.vColor.a = (1.f - Out.vColor.a) + g_fIncreaseAlpha) : (Out.vColor.a = Out.vColor.a - g_fIncreaseAlpha + 1.f);
	// true		- Decrease;
	// false	- Increase;


	/*if (g_fIncreaseAlpha < 1.f)
	{
		float fAlpha = 1.f - Out.vColor.a;
		Out.vColor.a = fAlpha + g_fIncreaseAlpha;

		if (Out.vColor.a <= 0.01f)
			discard;
	}
	else if(g_fIncreaseAlpha >= 1.f)
	{
		float fAlpha = g_fIncreaseAlpha - 1.f;
		Out.vColor.a = saturate(Out.vColor.a - fAlpha);
	}*/

	if (0 != g_iIsGlow && 0 == g_iIsColorGlow)
	{
		Out.vGlow = Out.vColor;
	}
	else if (0 != g_iIsColorGlow && 0 == g_iIsGlow)
	{
		Out.vGlow.rgb = (g_vGlowColor.rgb + Out.vColor.rgb) / 2.f;
		Out.vGlow.a = Out.vColor.a;
	}

	return Out;
}

technique11	DefaultTechnique {
	pass Sky
	{
		SetRasterizerState(RS_CounterClockWise);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Decal {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_DECAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DECAL();
	}

	pass Decal_Increase {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_DECAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DECAL_INCREASE();
	}
};
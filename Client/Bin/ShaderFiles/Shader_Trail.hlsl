#include "Shader_Defines.hlsli"

texture2D		g_texDiffuse;

int				g_iIsDistortion = 0;

sampler DefaultSampler = sampler_state {

};
struct VS_IN {
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT {
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_NON_ANIM(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	//matrix		matWV, matWVP;

	//matWV = mul(g_matWorld, g_matView);
	//matWVP = mul(matWV, g_matProj);


	// World Already Computed in Client
	matrix		matVP;
	matVP = mul(g_matView, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}
VS_OUT VS_ANIM(VS_IN In)
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
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT {
	vector		vColor : SV_TARGET0;
	vector		vGlow : SV_TARGET1;
	vector		vDistortion : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	if (g_iIsDistortion)
	{
		Out.vDistortion = Out.vColor * 0.5f;
		Out.vColor = Out.vColor * 0.5f;
	}

	return Out;
}


PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}


technique11 DefaultTechnique {
	pass Non_Anim {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_NON_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Anim {
		SetRasterizerState(RS_None);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_ANIM();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};
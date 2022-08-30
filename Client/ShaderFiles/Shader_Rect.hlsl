#include "Shader_Defines.hlsli"

texture2D		g_texDiffuse;

cbuffer TextureAlpha {
	float		fAlpha;
};

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
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT {
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

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

PS_OUT PS_MAIN_TEXTALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.a = fAlpha;

	return Out;
}

PS_OUT PS_MAIN_ALPHAMAX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_MAIN_ATLAS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Get Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.r += g_fColorOverlay / 255.f;
	Out.vColor.g += g_fColorOverlay / 255.f;
	Out.vColor.b += g_fColorOverlay / 255.f;

	// Alpha Test
	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_DIFFUSE_ANIM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Atlas UV Diffuse Animation
	float fAnimTickX = g_fAtlasSizeX - g_fAtlasPosX / 100;
	float fAnimCursorX = g_fAtlasPosX + (fAnimTickX * g_fUVAnimX);

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.r += g_fColorOverlay / 255.f;
	Out.vColor.g += g_fColorOverlay / 255.f;
	Out.vColor.b += g_fColorOverlay / 255.f;

	if (fAnimCursorX < In.vTexUV.x)
		Out.vColor.rgb = 0.f;

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_HPBAR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Atlas UV Animation
	float fAnimTickX = g_fAtlasSizeX - g_fAtlasPosX / 100;
	float fAnimCrntHPUV = g_fAtlasPosX + (fAnimTickX * g_fCrntHPUV);
	float fAnimPrevHPUV = g_fAtlasPosX + (fAnimTickX * g_fPrevHPUV);

	if (In.vTexUV.x < fAnimCrntHPUV)
	{
		Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

		Out.vColor.r += g_fColorOverlay / 255.f;
		Out.vColor.g += g_fColorOverlay / 255.f;
		Out.vColor.b += g_fColorOverlay / 255.f;
	}
	else if (fAnimCrntHPUV < In.vTexUV.x && In.vTexUV.x < fAnimPrevHPUV)
	{
		Out.vColor.r = 255.f;
		Out.vColor.g = 0.f;
		Out.vColor.b = 0.f;
	}
	else if (fAnimPrevHPUV < In.vTexUV.x && In.vTexUV.x < g_fAtlasSizeX)
	{
		Out.vColor.rgb = 0.f;
	}
	//else if (Out.vColor.a < 0.1f)
	//{
	//	discard;
	//}

	return Out;
}

PS_OUT PS_MAIN_End(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor.a = fAlpha;

	return Out;
}

technique11 DefaultTechnique {
	pass Default_Pass {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass UI_Pass {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	pass UI_Alpha_Pass {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass Text_Alpha_Pass {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEXTALPHA();
	}
	pass AlphaMaxTexture {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHAMAX();
	}
	pass Atlas
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS();
	}

	pass AtlasDiffuseAnim
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_DIFFUSE_ANIM();
	}

	pass AtlasHpBarAnim
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_HPBAR();
	}

	pass PassEnd {
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_End();
	}

};
#include "Shader_Defines.hlsli"

/*
<<<<< Pass Index >>>>>
0  Default_Pass
1  Atlas_Default
2  Atlas_Button_Activate
3  Atlas_Right2Left
4  Atlas_Title
5  Atlas_Bottom2Top
6  Atlas_UVA_HPBAR
7  Atlas_UVA_TriGrid
8  Atlas_UVA_RectGrid
9  Atlas_UVA_Circle
10 Atlas_UVA_Engage
11 Atlas_Color_Overlay_On_Inverse
12 Atlas_Color_Overlay_On_One
13 Atlas_Color_Overlay_Two_Tone
14 Atlas_3D_Default
15 Atlas_Dot_Background
16 Pass_End
17 Atlas_UVA_HPBAR_Reverse
*/

texture2D		g_texDiffuse;
float			g_fCrntDegree;
bool			g_bGrayScale = false;
bool			g_bAlpha = false;
float			g_fAlpha = 1.f;
cbuffer	Atlas
{
	float	g_fAtlasPosX;
	float	g_fAtlasPosY;
	float	g_fAtlasSizeX;
	float	g_fAtlasSizeY;
}

cbuffer	UVAnim
{
	float	g_fUVAnimX;
	float	g_fUVAnimY;
	bool	g_bIsRtoL;
}

cbuffer	Overlay
{
	float4	g_vecColorOverlay;
	float4	g_vecColorOverlay2;
	float	g_fAlphaOverlay;
}

cbuffer	HPBar
{
	float	g_fCrntHPUV;
	float	g_fPrevHPUV;
}

sampler DefaultSampler = sampler_state
{

};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
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

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_ATLAS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;

	// Alpha Test
	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	if (g_bGrayScale)
	{
		Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_BUTTON_ACTIVATE(PS_IN In)
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

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	
	if (fAnimCursorX < In.vTexUV.x)
	{
		Out.vColor += g_vecColorOverlay / 255.f;
	}
	else
	{
		Out.vColor += g_vecColorOverlay2 / 255.f;
	}

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_RIGHT2LEFT(PS_IN In)
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

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	if (0.3f < Out.vColor.r && 0.4f < Out.vColor.a)
	{
		Out.vColor.r = g_vecColorOverlay.r / 255.f;
		Out.vColor.g = g_vecColorOverlay.g / 255.f;
		Out.vColor.b = g_vecColorOverlay.b / 255.f;
		Out.vColor.a = g_vecColorOverlay.a / 255.f;
	}

	// Set UV Anim Direction
	if (true == g_bIsRtoL)
	{
		if (In.vTexUV.x < fAnimCursorX)
		{
			discard;
		}
		if (Out.vColor.a < 0.1f)
		{
			discard;
		}
	}
	else
	{
		if (fAnimCursorX < In.vTexUV.x)
		{
			discard;
		}
		if (Out.vColor.a < 0.1f)
		{
			discard;
		}
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_TITLE(PS_IN In)
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

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;

	// Set UV Anim Direction
	//if (fAnimCursorX < In.vTexUV.x)
	//{
	//	discard;
	//}
	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_BOTTOM2TOP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Atlas UV Diffuse Animation
	float fAnimTickY = g_fAtlasSizeY - g_fAtlasPosY / 100;
	float fAnimCursorY = g_fAtlasPosY + (fAnimTickY * g_fUVAnimY);

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;

	if (In.vTexUV.y < fAnimCursorY)
	{
		discard;
	}
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

	// Set Color
	if (In.vTexUV.x < fAnimCrntHPUV)
	{
		Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
		Out.vColor += g_vecColorOverlay / 255.f;
	}
	else if (fAnimCrntHPUV < In.vTexUV.x && In.vTexUV.x < fAnimPrevHPUV)
	{
		Out.vColor.r += 0.5f;
	}
	else if (fAnimPrevHPUV < In.vTexUV.x && In.vTexUV.x < g_fAtlasSizeX)
	{
		Out.vColor.rgb -= 0.5f;
	}
	else if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	if (g_bGrayScale)
	{
		Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
	}

	return Out;
}

PS_OUT PS_MAIN_ATLAS_TRIGRID(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;

	// 이게 여기 들어올때마다 바뀌는거 같겠지만 Out.vColor는 텍스처 원래 색상 그대로.
	// 그래서 g_vecColorOverlay가 바뀌지 않는 한 일정한 색깔로 유지된다.
	// 마찬가지로 여기서 Out.vColor 값을 0 등으로 초기화해버리면
	// 매 틱마다 완전 백지 상태로 시작되므로 패러미터를 아무리 조작해도 모든 픽셀이
	// 똑같이 작동하게 된다.

	if (Out.vColor.a < 0.7f)
	{
		discard;
	}
	// 0 ~ 1이라곤하지만 1이 되는 순간은 없음에 주의

	return Out;
}

PS_OUT PS_MAIN_ATLAS_RECTGRID(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;
	Out.vColor.a = 0.1f;

	return Out;
}

PS_OUT PS_MAIN_ATLAS_CIRCLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	
	if (Out.vColor.r < g_fCrntDegree)
	{
		Out.vColor.r = 0.29f;
		Out.vColor.g = 0.29f;
		Out.vColor.b = 0.29f;
	}
	else if (Out.vColor.a > 0.1f)
	{
		Out.vColor.r = g_vecColorOverlay.r / 255.f;
		Out.vColor.g = g_vecColorOverlay.g / 255.f;
		Out.vColor.b = g_vecColorOverlay.b / 255.f;
	}
	if (Out.vColor.a < 0.9f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_ENGAGE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	
	if (0.3f < Out.vColor.r && 0.4f < Out.vColor.a)
	{
		//Out.vColor.r += g_vecColorOverlay.r / 255.f;
		//Out.vColor.g += g_vecColorOverlay.g / 255.f;
		//Out.vColor.b += g_vecColorOverlay.b / 255.f;
		//Out.vColor.a = g_vecColorOverlay.a / 255.f;

		Out.vColor.r = g_vecColorOverlay.r / 255.f;
		Out.vColor.g = g_vecColorOverlay.g / 255.f;
		Out.vColor.b = g_vecColorOverlay.b / 255.f;
		Out.vColor.a = g_vecColorOverlay.a / 255.f;
	}

	// Alpha Test
	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_COLOR_OVERLAY_INVERSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);


	// 모든 경우에 대해 
	if (Out.vColor.a > 0.4f)
	{
		Out.vColor = g_vecColorOverlay / 255.f;
		Out.vColor.a = 1.f;
	}
	else
	{
		Out.vColor = g_vecColorOverlay2 / 255.f;
		Out.vColor.a = 1.f;
	}
	return Out;
}

PS_OUT PS_MAIN_COLOR_OVERLAY_ON_ONE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	// 뭐라도 색깔이 있는 픽셀은 색상 보정
	if (0.1f < Out.vColor.r)
	{
		Out.vColor += g_vecColorOverlay / 255.f;
	}
	return Out;
}

PS_OUT PS_MAIN_COLOR_OVERLAY_TWO_TONE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Atlas UV Diffuse Animation
	float fAnimTickY = g_fAtlasSizeY - g_fAtlasPosY / 100;
	float fAnimCursorY = g_fAtlasPosY + (fAnimTickY * g_fUVAnimY);

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	if (In.vTexUV.y < fAnimCursorY)
	{
		Out.vColor += g_vecColorOverlay / 255.f;
	}
	else
	{
		Out.vColor += g_vecColorOverlay2 / 255.f;
	}

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_ATLAS_3D_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
	Out.vColor += g_vecColorOverlay / 255.f;

	// Alpha Test
	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	return Out;
}

PS_OUT PS_MAIN_DOT_BACKGROUND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Set Color
	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	if (0.1f < Out.vColor.a)
	{
		Out.vColor.a = 1.f - Out.vColor.r;
	}

	// Alpha Test
	//if (Out.vColor.a < 0.3f)
	//{
	//	discard;
	//}
	return Out;
}

PS_OUT PS_MAIN_END(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);

	[flatten]
	if (Out.vColor.a <= 0.1f)
		discard;

	[flatten]
	if (g_bAlpha)
	{
		Out.vColor.a = saturate(Out.vColor.a - g_fAlpha);
	}

	return Out;
}

PS_OUT PS_MAIN_ATLAS_HPBAR_REVERSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Atlas UV PickUP
	if (In.vTexUV.x < g_fAtlasPosX || In.vTexUV.y < g_fAtlasPosY)
		discard;
	if (g_fAtlasSizeX < In.vTexUV.x || g_fAtlasSizeY < In.vTexUV.y)
		discard;

	// Atlas UV Animation
	float fAnimTickX = g_fAtlasSizeX - g_fAtlasPosX / 100;
	//float fAnimCrntHPUV = g_fAtlasPosX + (fAnimTickX * g_fCrntHPUV);
	//float fAnimPrevHPUV = g_fAtlasPosX + (fAnimTickX * g_fPrevHPUV);

	float fAnimCrntHPUV = g_fAtlasSizeX - (fAnimTickX * g_fCrntHPUV);
	float fAnimPrevHPUV = g_fAtlasSizeX - (fAnimTickX * g_fPrevHPUV);

	// Set Color
	//if (In.vTexUV.x < fAnimCrntHPUV)
	if (fAnimCrntHPUV < In.vTexUV.x)
	{
		Out.vColor = g_texDiffuse.Sample(DefaultSampler, In.vTexUV);
		Out.vColor += g_vecColorOverlay / 255.f;
	}
	//else if (fAnimCrntHPUV < In.vTexUV.x && In.vTexUV.x < fAnimPrevHPUV)
	else if (fAnimPrevHPUV < In.vTexUV.x && In.vTexUV.x < fAnimCrntHPUV)
	{
		Out.vColor.r += 0.5f;
	}
	else if (g_fAtlasPosX < In.vTexUV.x && In.vTexUV.x < fAnimPrevHPUV)
	{
		Out.vColor.rgb -= 0.5f;
	}
	else if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	if (g_bGrayScale)
	{
		Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass_Default
	{
		SetRasterizerState(RS_None);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Atlas_Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_DEFAULT();
	}
	pass Atlas_Button_Activate
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_BUTTON_ACTIVATE();
	}
	pass Atlas_Right2Left
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_RIGHT2LEFT();
	}
	pass Atlas_Title
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_TITLE();
	}
	pass Atlas_Bottom2Top
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_BOTTOM2TOP();
	}
	pass Atlas_UVA_HpBar
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_HPBAR();
	}
	pass Atlas_UVA_TriGrid
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_TRIGRID();
	}
	pass Atlas_UVA_RectGrid
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_RECTGRID();
	}
	pass Atlas_UVA_Circle
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_CIRCLE();
	}
	pass Atlas_UVA_Engage
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_ENGAGE();
	}
	pass Atlas_Color_Overlay_On_Inverse
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR_OVERLAY_INVERSE();
	}
	pass Atlas_Color_Overlay_On_One
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR_OVERLAY_ON_ONE();
	}
	pass Atlas_Color_Overlay_Two_Tone
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR_OVERLAY_TWO_TONE();
	}
	pass Atlas_3D_Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_3D_DEFAULT();
	}
	pass Atlas_Dot_Background
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOT_BACKGROUND();
	}	
	pass Pass_End
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_END();
	}
	pass Atlas_UVA_HpBar_Reverse
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ATLAS_HPBAR_REVERSE();
	}
};
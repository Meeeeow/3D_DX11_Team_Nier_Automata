
cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
	int			g_isRed = 0;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float4		vColor : COLOR0;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	/* XMVector3TransformCoord : °ö¼À ÈÄ, w°ªÀ¸·Î xyzw¸¦ ³ª´«´Ù. */
	/* mul : °ö¼À¿¬»ê¸¸ ¼öÇàÇÑ´Ù. */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vColor = In.vColor;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : COLOR0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;	
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = vector(g_isRed - In.vColor.r, In.vColor.g - g_isRed, 0.f, 1.f);

	return Out;
}

RasterizerState CullMode_Solid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

DepthStencilState Depth_Stencil_NonZEnable
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

BlendState Blend_None
{
	BlendEnable[0] = false;
};


DepthStencilState Depth_Stencil_Priority
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};


technique11		DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(CullMode_Solid);
		SetBlendState(Blend_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(Depth_Stencil_NonZEnable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};



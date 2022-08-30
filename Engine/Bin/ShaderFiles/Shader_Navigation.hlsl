
cbuffer Matrices {
	matrix		g_matWorld;
	matrix		g_matView;
	matrix		g_matProj;
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
	In.vPosition.y += 0.1f;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vColor = In.vColor;

	return Out;
}

VS_OUT VS_MAIN_SELECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	matrix		matWV, matWVP;
	In.vPosition.y += 0.2f;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

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
	Out.vColor = In.vColor;

	return Out;
}

PS_OUT PS_MAIN_SELECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = vector(1.f, 0.f, 0.f, 1.f);

	return Out;
}

DepthStencilState DSS_Priority {
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockWise = false;
};

BlendState BS_None {
	BlendEnable[0] = false;
};


technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Select
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN_SELECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SELECT();
	}
};


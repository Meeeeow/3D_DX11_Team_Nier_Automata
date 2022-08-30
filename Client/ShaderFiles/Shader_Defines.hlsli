
cbuffer Matrices {
	matrix		g_matWorld;
	matrix		g_matView;
	matrix		g_matProj;
};

cbuffer CameraDesc {
	vector		g_vCamPos;
	vector		g_vCamLook;
	float		g_fFar;
};

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
}
cbuffer	ColorOverlay
{
	float4	g_fColorOverlay;
}

cbuffer	HPBar
{
	float	g_fCrntHPUV;
	float	g_fPrevHPUV;
}

cbuffer LightDesc {
	vector		g_vLightDir;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;
};

cbuffer MaterialDesc {
	vector		g_vMtrlDiffuse = (vector)1.f;
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
	vector		g_vMtrlEmissive = (vector)1.f;
	float		g_fPower = 30.f;
};

DepthStencilState DSS_Priority {
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

DepthStencilState DSS_Default {
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

BlendState BS_None {
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlending {
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
};

BlendState BS_Add {
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

RasterizerState RS_WireFrame {
	FillMode = WireFrame;
	CullMode = Back;
	FrontCounterClockWise = false;
};

RasterizerState RS_CounterClockWise {
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockWise = false;
};

RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockWise = false;
};

RasterizerState RS_None {
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockWise = false;
};
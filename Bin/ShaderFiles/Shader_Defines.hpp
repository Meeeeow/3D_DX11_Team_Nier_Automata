
cbuffer Matrices
{
	matrix		g_WorldMatrix;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
};



/* D3D11_RASTERIZER_DESC */
/* 
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/

/* D3D11_BLEND_DESC */

//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;


/*
D3D11_DEPTH_STENCIL_DESC

BOOL DepthEnable;
D3D11_DEPTH_WRITE_MASK DepthWriteMask;
D3D11_COMPARISON_FUNC DepthFunc;
BOOL StencilEnable;
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/

DepthStencilState Depth_Stencil_Priority
{
	DepthEnable = false;
	DepthWriteMask = zero;
	DepthFunc = less;
};

DepthStencilState Depth_Stencil_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

BlendState Blend_None	
{
	BlendEnable[0] = false;
};

BlendState Blend_AlphaBlending
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
};


BlendState Blend_Add
{
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = one;
	DestBlend = one;
};

RasterizerState CullMode_Wireframe
{
	FillMode = wireframe;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState CullMode_CW
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState CullMode_Solid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};
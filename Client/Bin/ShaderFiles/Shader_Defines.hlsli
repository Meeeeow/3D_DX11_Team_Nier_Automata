
cbuffer Matrices {
	matrix		g_matWorld;
	matrix		g_matWorldInv;

	matrix		g_matView;
	matrix		g_matViewInv;

	matrix		g_matProj;
	matrix		g_matProjInv;
};

cbuffer OldMatrices {
	matrix		g_matOldWorld;
	matrix		g_matOldView;
	matrix		g_matOldProj;
};

cbuffer LightMatrices {
	matrix		g_matLightView;
	matrix		g_matLightProj;
};

cbuffer CameraDesc {
	vector		g_vCamPos;
	vector		g_vCamLook;
	float		g_fFar;
	float3		CameraDescPadding;
};

cbuffer LightDesc {
	vector		g_vLightDir;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;
	float		g_fLightFar;
	float3		LightDescPadding;
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

BlendState BS_AlphaBlendingTest {
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

BlendState BS_Test {
	BlendEnable[0] = true;
	BlendOp = Add;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
};

BlendState BS_Test2 {
	BlendEnable[0] = true;
	BlendOp = add;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
};

RasterizerState RS_WireFrame {
	FillMode = WireFrame;
	CullMode = Back;
	FrontCounterClockWise = false;
};

RasterizerState RS_CounterClockWise {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockWise = true;
};

RasterizerState RS_Front {
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

// D3D11_BLEND_OP : https://docs.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_blend_op?redirectedfrom=MSDN&f1url=%3FappId%3DDev14IDEF1%26l%3DKO-KR%26k%3Dk(D3D11%252FD3D11_BLEND_OP_SUBTRACT);k(D3D11_BLEND_OP_SUBTRACT);k(SolutionItemsProject);k(DevLang-C%252B%252B);k(TargetOS-Windows)%26rd%3Dtrue
// D3D11_BLEND : https://docs.microsoft.com/ko-kr/windows/win32/api/d3d11/ne-d3d11-d3d11_blend?redirectedfrom=MSDN&f1url=%3FappId%3DDev14IDEF1%26l%3DKO-KR%26k%3Dk(D3D11%252FD3D11_BLEND_ZERO);k(D3D11_BLEND_ZERO);k(SolutionItemsProject);k(DevLang-C%252B%252B);k(TargetOS-Windows)%26rd%3Dtrue
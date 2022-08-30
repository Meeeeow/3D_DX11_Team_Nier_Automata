#pragma once
#ifndef __CLIENT_SHADER_PATH_KEY_H__
#define __CLIENT_SHADER_PATH_KEY_H__



#ifdef _AFX
/* Shader - Client */

static const wchar_t*	Shader_Model			= L"../Client/Bin/ShaderFiles/Shader_Model.hlsl";
static const wchar_t*	Shader_Terrain			= L"../Client/Bin/ShaderFiles/Shader_Terrain.hlsl";
static const wchar_t*	Shader_Rect				= L"../Client/Bin/ShaderFiles/Shader_Rect.hlsl";
static const wchar_t*	Shader_Navigation		= L"../Client/Bin/ShaderFiles/Shader_Navigation.hlsl";
static const wchar_t*	Shader_Viewport			= L"../Client/Bin/ShaderFiles/Shader_Viewport.hlsl";
static const wchar_t*	Shader_PointInstance	= L"../Client/Bin/ShaderFiles/Shader_PointInstance.hlsl";
static const wchar_t*	Shader_RectInstance		= L"../Client/Bin/ShaderFiles/Shader_RectInstance.hlsl";
static const wchar_t*	Shader_NavigationTex	= L"../Client/Bin/ShaderFiles/Shader_NavigationTexture.hlsl";

#else
/* Shader - Tool */

static const wchar_t*	Shader_Model			= L"../Bin/ShaderFiles/Shader_Model.hlsl";
static const wchar_t*	Shader_Terrain			= L"../Bin/ShaderFiles/Shader_Terrain.hlsl";
static const wchar_t*	Shader_Rect				= L"../Bin/ShaderFiles/Shader_Rect.hlsl";
static const wchar_t*	Shader_Navigation		= L"../Bin/ShaderFiles/Shader_Navigation.hlsl";
static const wchar_t*	Shader_Viewport			= L"../Bin/ShaderFiles/Shader_Viewport.hlsl";
static const wchar_t*	Shader_PointInstance	= L"../Bin/ShaderFiles/Shader_PointInstance.hlsl";
static const wchar_t*	Shader_RectInstance		= L"../Bin/ShaderFiles/Shader_RectInstance.hlsl";
static const wchar_t*	Shader_NavigationTex	= L"../Bin/ShaderFiles/Shader_NavigationTexture.hlsl";

#endif // !_AFX

/* Technique */
static const char*		DefaultTechnique		= "DefaultTechnique";

#endif // !__CLIENT_SHADER_PATH_KEY_H__
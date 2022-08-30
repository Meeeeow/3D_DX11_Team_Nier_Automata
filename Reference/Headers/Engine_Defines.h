#pragma once
#ifndef __ENGINE_DEFINES_H__
#define __ENGINE_DEFINES_H__

#pragma warning (disable : 4251)

// STL ////////////////////////////

#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <list>
#include <vector>
#include <typeinfo.h>
#include <process.h>
#include <map>
#include <functional>
#include <random>
#include <ctime>
#include <mutex>
#include <queue>
#include <memory>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include <thread>

using namespace std;

///////////////////////////////////

// DX 11 //////////////////////////

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <DirectXCollision.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include "d3dx11effect.h"
#include "DirectXTex.h"

using namespace DirectX;
using namespace PackedVector;

///////////////////////////////////

// Direct Input ///////////////////

#define DIRECTINPUT_VERSION	0x0800

#include <dinput.h>


///////////////////////////////////

// Assimp /////////////////////////

#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

///////////////////////////////////

// CRT DEBUG //////////////////////

#ifndef _AFX
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif
#endif // _DEBUG
#endif // !_AFX

///////////////////////////////////

// ENGINE DEFINES /////////////////

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Color.h"

#define GRAVITY 9.f


#define SHADOWMAP_WIDTH 4096
#define SHADOWMAP_HEIGHT 4096
//#define SHADOWMAP_WIDTH 1280
//#define SHADOWMAP_HEIGHT 720

using namespace Engine;

//enum class SHADER_VIEWPORT_PASS_INDEX : int
//{
//	DEFFERED = 0,
//	LIGHT_DIR = 1,
//	LIGHT_POINT = 2,
//	BLEND = 3,
//	POSTPROCESSING_BEFOREUI_FOCUS = 4,
//	POSTPROCESSING_BEFOREUI_FILTER = 5,
//	POSTPROCESSING_BEFOREUI_FOCUS_FILTER = 6,
//	POSTPROCESSING_AFTERUI = 7,
//	COUNT = 8,
//}



///////////////////////////////////

#endif // !__ENGINE_DEFINES_H__
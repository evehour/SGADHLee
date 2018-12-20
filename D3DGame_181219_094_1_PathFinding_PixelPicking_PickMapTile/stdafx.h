#pragma once

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of _CLIENT_BLOCK type _NORMAL_BLOCK
#else
#define DBG_NEW new

#endif

///////////////////////////////////////////////////////////////////////////////
//#define __USE_BLENDMAP__
///////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <thread>
using namespace std;

//DirectWrite
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "effects11d.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

//Fbx SDK
#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

const wstring Shaders = L"../_Shaders/";
const wstring Contents = L"../_Contents/";
const wstring Textures = L"../_Textures/";
const wstring Assets = L"../_Assets/";
const wstring Models = L"../_SkinnedModels/";
const wstring Materials = L"../_Materials/";


#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

#include "./Systems/D3D.h"
#include "./Systems/DirectWrite.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/CBuffer.h"
#include "./Renders/Shader.h"
#include "./Renders/Texture.h"
#include "./Renders/Context.h"
#include "./Renders/RenderTarget.h"
#include "./Renders/Render2D.h"
#include "./Renders/Material.h"
#include "./Renders/CSResource.h"

#include "./Executes/Execute.h"

#include "./Viewer/Viewport.h"
#include "./Viewer/RenderTargetView.h"
#include "./Viewer/DepthStencilView.h"

#include "./Utilities/Point.h"
#include "./Utilities/Rect.h"
#include "./Utilities/String.h"
#include "./Utilities/Math.h"
#include "./Utilities/Path.h"

#include "./Boundings/Ray.h"
#include "./Boundings/BBox.h"
#include "./Boundings/BSphere.h"

#include "./Draw/DebugLine.h"
#include "./Draw/MeshCube.h"
#include "./Draw/MeshCylinder.h"
#include "./Draw/MeshGrid.h"
#include "./Draw/MeshSphere.h"
#include "./Draw/MeshQuad.h"

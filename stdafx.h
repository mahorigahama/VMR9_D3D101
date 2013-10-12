#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// #define D3D_DEBUG_INFO
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <tchar.h>

// ATL
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS// some CString constructors will be explicit
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlcoll.h>

// DirectX
#include <dshow.h>
#include <d3d9.h>
#include <vmr9.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d10_1.h>
#pragma comment(lib, "d3d10_1.lib")
#include <d3dX10.h>
#pragma comment(lib, "d3dx10.lib")
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")

#include <dxerr.h>
#pragma comment(lib, "dxerr.lib")

// Direct2D
#include <d2d1.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1.lib")

// DirectWrite
#include <Dwrite.h>
#pragma comment(lib, "Dwrite.lib")

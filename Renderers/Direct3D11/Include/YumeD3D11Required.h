///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeD3D11Required.h
/// Date : 9.2.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#pragma once
//---------------------------------------------------------------------------------
#ifndef __YumeD3D11Required_h__
#define __YumeD3D11Required_h__
//---------------------------------------------------------------------------------
#include "Core/YumeRequired.h"

#undef NOMINMAX
#define NOMINMAX

//D3D11 Includes
#include <d3d11.h>
#include <d3dx11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
//~

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

namespace YumeEngine
{

	class YumeD3D11Device;
	class YumeD3D11Adapter;
	class YumeD3D11AdapterList;
	class YumeD3D11AdapterInfo;
	class YumeD3D11AdapterInfoList;
	class YumeD3D11RenderWindow;

	class YumeD3D11Renderer;
	

#if (YUME_PLATFORM == YUME_PLATFORM_WIN32) && !defined(YUME_STATIC_LIB)
#	ifdef YUME_D3D_EXPORT
#		define YumeD3DApiExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define YumeD3DApiExport
#       else
#    		define YumeD3DApiExport __declspec(dllimport)
#       endif
#	endif
#else
#	define YumeD3DApiExport
#endif
}
#endif
//---------------------------------------------------------------------------------

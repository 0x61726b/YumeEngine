///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

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
/// File : YumeD3D11AdapterList.h
/// Date : 3.9.2015
/// Comments : 
///--------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeD3D11AdapterList_h__
#define __YumeD3D11AdapterList_h__
///--------------------------------------------------------------------------------
#include "YumeD3D11Required.h"
///--------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11AdapterList
	{
	private:
		YumeVector<YumeD3D11Adapter*>::type mDriverList;

	public:
		YumeD3D11AdapterList(IDXGIFactory1*	pDXGIFactory);
		~YumeD3D11AdapterList();

		BOOL Enumerate(IDXGIFactory1*	pDXGIFactory);
		size_t GetCount() const;
		YumeD3D11Adapter* Get(size_t index);

		YumeD3D11Adapter* Get(const YumeString &name);

	};
}

///--------------------------------------------------------------------------------
//End of __YumeD3D11AdapterList_h__
#endif
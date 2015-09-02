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
/// File : YumeD3D11Device.h
/// Date : 8.27.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeD3D11Device_h__
#define __YumeD3D11Device_h__
//--------------------------------------------------------------------------------
#include "YumeD3D11Required.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11Device
	{
	public:
		YumeD3D11Device();

	private:
		ID3D11Device*				m_pD3DDevice;
		ID3D11DeviceContext*		m_pImmediateContext;
		ID3D11InfoQueue *			m_pInfoQueue;

	public:
		YumeD3D11Device(ID3D11Device* dev);

		virtual ~YumeD3D11Device();

		inline ID3D11DeviceContext* GetImmediateContext()
		{
			return m_pImmediateContext;
		}

		inline ID3D11Device * operator->() const
		{
			assert(m_pD3DDevice);
			if (D3D_NO_EXCEPTION != mExceptionsErrorLevel)
			{
				clearStoredErrorMessages();
			}
			return m_pD3DDevice;
		}

		const void clearStoredErrorMessages() const;

		ID3D11Device * operator=(ID3D11Device * device);
		const bool isNull();
		const YumeString getErrorDescription(const HRESULT hr = NO_ERROR) const;

		inline const bool isError() const
		{
			if (D3D_NO_EXCEPTION == mExceptionsErrorLevel)
			{
				return  false;
			}

			return _getErrorsFromQueue();
		}

		enum eExceptionsErrorLevel
		{
			D3D_NO_EXCEPTION,
			D3D_CORRUPTION,
			D3D_ERROR,
			D3D_WARNING,
			D3D_INFO,
		};


		const bool _getErrorsFromQueue() const;
		void Release();
		ID3D11Device* Get();

		static eExceptionsErrorLevel mExceptionsErrorLevel;
		static void SetExceptionsErrorLevel(const eExceptionsErrorLevel exceptionsErrorLevel);
		static const eExceptionsErrorLevel GetExceptionsErrorLevel();
	};
}

//--------------------------------------------------------------------------------
#endif
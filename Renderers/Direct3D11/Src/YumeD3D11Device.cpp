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
/// File : YumeD3D11Device.h
/// Date : 8.27.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#include "YumeD3D11Device.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{
	YumeD3D11Device::eExceptionsErrorLevel YumeD3D11Device::mExceptionsErrorLevel = YumeD3D11Device::D3D_NO_EXCEPTION;
	//--------------------------------------------------------------------------------
	YumeD3D11Device::YumeD3D11Device() : m_pD3DDevice(0),m_pImmediateContext(0)
	{
	}
	//--------------------------------------------------------------------------------
	YumeD3D11Device::YumeD3D11Device(ID3D11Device* Device)
		: m_pD3DDevice(Device)
	{
		if (Device)
		{
			m_pD3DDevice->GetImmediateContext(&m_pImmediateContext);

			HRESULT hr = m_pD3DDevice->QueryInterface(__uuidof(ID3D11InfoQueue), (LPVOID*)&m_pInfoQueue);

			if (SUCCEEDED(hr))
			{
				m_pInfoQueue->ClearStoredMessages();
				m_pInfoQueue->ClearRetrievalFilter();
				m_pInfoQueue->ClearStorageFilter();

				D3D11_INFO_QUEUE_FILTER filter;
				ZeroMemory(&filter, sizeof(D3D11_INFO_QUEUE_FILTER));
				std::vector<D3D11_MESSAGE_SEVERITY> severityList;

				switch (mExceptionsErrorLevel)
				{
				case D3D_NO_EXCEPTION:
					severityList.push_back(D3D11_MESSAGE_SEVERITY_CORRUPTION);
				case D3D_CORRUPTION:
					severityList.push_back(D3D11_MESSAGE_SEVERITY_ERROR);
				case D3D_ERROR:
					severityList.push_back(D3D11_MESSAGE_SEVERITY_WARNING);
				case D3D_WARNING:
				case D3D_INFO:
					severityList.push_back(D3D11_MESSAGE_SEVERITY_INFO);
				default:
					break;
				}


				if (severityList.size() > 0)
				{
					filter.DenyList.NumSeverities = severityList.size();
					filter.DenyList.pSeverityList = &severityList[0];
				}

				m_pInfoQueue->AddStorageFilterEntries(&filter);
				m_pInfoQueue->AddRetrievalFilterEntries(&filter);
			}
		}

	}
	//--------------------------------------------------------------------------------
	YumeD3D11Device::~YumeD3D11Device()
	{
	}
	//--------------------------------------------------------------------------------
	//---------------------------------------------------------------------
	ID3D11Device * YumeD3D11Device::operator=(ID3D11Device * D3D11device)
	{
		m_pD3DDevice = D3D11device;
		if (D3D11device)
		{
			D3D11device->GetImmediateContext(&m_pImmediateContext);
		}
		return m_pD3DDevice;
	}
	//---------------------------------------------------------------------
	const bool YumeD3D11Device::isNull()
	{
		return m_pD3DDevice == 0;
	}
	//---------------------------------------------------------------------
	const YumeString YumeD3D11Device::getErrorDescription(const HRESULT lastResult /* = NO_ERROR */) const
	{
		if (!m_pD3DDevice)
		{
			return "NULL device";
		}

		if (D3D_NO_EXCEPTION == mExceptionsErrorLevel)
		{
			return "infoQ exceptions are turned off";
		}

		YumeString res;

		switch (lastResult)
		{
		case NO_ERROR:
			break;
		case E_INVALIDARG:
			res = res + "invalid parameters were passed.\n";
			break;
		default:
			assert(false); // unknown HRESULT
		}

		if (m_pInfoQueue != NULL)
		{
			UINT64 numStoredMessages = m_pInfoQueue->GetNumStoredMessages();
			for (UINT64 i = 0; i < numStoredMessages; i++)
			{
				// Get the size of the message
				SIZE_T messageLength = 0;
				m_pInfoQueue->GetMessage(i, NULL, &messageLength);
				// Allocate space and get the message
				D3D11_MESSAGE * pMessage = (D3D11_MESSAGE*)malloc(messageLength);
				m_pInfoQueue->GetMessage(i, pMessage, &messageLength);
				res = res + pMessage->pDescription + "\n";
				free(pMessage);
			}
		}

		return res;
	}
	//---------------------------------------------------------------------
	void YumeD3D11Device::Release()
	{
		SAFE_RELEASE(m_pInfoQueue);
		SAFE_RELEASE(m_pD3DDevice);
		SAFE_RELEASE(m_pImmediateContext);
	}
	//---------------------------------------------------------------------
	ID3D11Device * YumeD3D11Device::Get()
	{
		return m_pD3DDevice;
	}
	//---------------------------------------------------------------------
	void YumeD3D11Device::SetExceptionsErrorLevel(const eExceptionsErrorLevel exceptionsErrorLevel)
	{
		mExceptionsErrorLevel = exceptionsErrorLevel;
	}
	//---------------------------------------------------------------------
	const YumeD3D11Device::eExceptionsErrorLevel YumeD3D11Device::GetExceptionsErrorLevel()
	{
		return mExceptionsErrorLevel;
	}
	//---------------------------------------------------------------------
	const bool YumeD3D11Device::_getErrorsFromQueue() const
	{
		if (m_pInfoQueue != NULL)
		{
			UINT64 numStoredMessages = m_pInfoQueue->GetNumStoredMessages();

			if (D3D_INFO == mExceptionsErrorLevel && numStoredMessages > 0)
			{
				// if D3D_INFO we don't need to loop if the numStoredMessages > 0
				return true;
			}
			for (UINT64 i = 0; i < numStoredMessages; i++)
			{
				// Get the size of the message
				SIZE_T messageLength = 0;
				m_pInfoQueue->GetMessage(i, NULL, &messageLength);
				// Allocate space and get the message
				D3D11_MESSAGE * pMessage = (D3D11_MESSAGE*)malloc(messageLength);
				m_pInfoQueue->GetMessage(i, pMessage, &messageLength);

				bool res = false;
				switch (pMessage->Severity)
				{
				case D3D11_MESSAGE_SEVERITY_CORRUPTION:
					if (D3D_CORRUPTION == mExceptionsErrorLevel)
					{
						res = true;
					}
					break;
				case D3D11_MESSAGE_SEVERITY_ERROR:
					switch (mExceptionsErrorLevel)
					{
					case D3D_INFO:
					case D3D_WARNING:
					case D3D_ERROR:
						res = true;
					}
					break;
				case D3D11_MESSAGE_SEVERITY_WARNING:
					switch (mExceptionsErrorLevel)
					{
					case D3D_INFO:
					case D3D_WARNING:
						res = true;
					}
					break;
				}

				free(pMessage);
				if (res)
				{
					// we don't need to loop anymore...
					return true;
				}

			}

			clearStoredErrorMessages();

			return false;

		}
		else
		{
			return false;
		}
	}

	const void YumeD3D11Device::clearStoredErrorMessages() const
	{
		if (m_pD3DDevice && D3D_NO_EXCEPTION != mExceptionsErrorLevel)
		{
			if (m_pInfoQueue != NULL)
			{
				m_pInfoQueue->ClearStoredMessages();
			}
		}
	}

	//---------------------------------------------------------------------
}
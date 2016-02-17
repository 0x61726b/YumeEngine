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
/// File : YumeD3D11Renderer.cpp
/// Date : 9.3.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#include "Core/YumeHeaders.h"

#include "YumeD3D11Renderer.h"
#include "YumeD3D11AdapterList.h"
#include "YumeD3D11Adapter.h"
#include "YumeD3D11AdapterInfo.h"
#include "YumeD3D11AdapterInfoList.h"
#include "YumeD3D11RenderWindow.h"



#include "Core/YumeCentrum.h"
#include "Logging/logging.h"
//Arrrr


namespace YumeEngine
{
	extern "C" void YumeD3DApiExport dllStartPlugin(void) throw()
	{
		YUMELOG_DEBUG("Loading Yume_Direct3D11.dll...");

		HINSTANCE hInst = GetModuleHandle(L"YUME_DIRECT3D11.dll");

		YumeRenderer* r = new YumeD3D11Renderer(hInst);

		if(!gYume)
			return;


		gYume->pYume->AddRenderer(r);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeD3DApiExport dllStop(void) throw()
	{
		YumeRenderer* renderer = gYume->pYume->GetRenderer();
		delete renderer;
	}
	//---------------------------------------------------------------------
	YumeD3D11Renderer::YumeD3D11Renderer(HINSTANCE hInst)
		: m_Device(0)
	{
		

		m_hInstance = hInst;

		m_bRendererInitialized = false;
		Init();
		Configure();
	}
	//---------------------------------------------------------------------
	YumeD3D11Renderer::~YumeD3D11Renderer()
	{
		YUMELOG_DEBUG("Direct3D 11 is being shutdown...");
		Shutdown();
	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::Init()
	{
		if (m_bRendererInitialized)
			return;
		m_bRendererInitialized = true;

		m_pDXGIFactory = NULL;
		HRESULT hr;
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&m_pDXGIFactory);
		if (FAILED(hr))
		{
			YUMELOG_DEBUG(
				"Failed to create Direct3D11 DXGIFactory1 D3D11RenderSystem::D3D11RenderSystem");
		}
		m_AdapterList = NULL;
		m_CurrAdapter = NULL;
		m_CurrentWindow = NULL;

		UINT deviceFlags = 0;
		if (YumeD3D11Device::D3D_NO_EXCEPTION != YumeD3D11Device::GetExceptionsErrorLevel() && YUME_DEBUG_MODE)
		{
			deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		ID3D11Device * device;
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &device, 0, 0);

		if (FAILED(hr))
		{
			std::stringstream error;
			/*error << "Failed to create Direct3D11 object." << std::endl << DXGetErrorDescription(hr) << std::endl;*/

			YUMELOG_DEBUG("Error creating D3D11 device");
		}

		m_Device = YumeD3D11Device(device);
	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::Configure()
	{
		YumeD3D11AdapterList* adapterList;
		YumeD3D11Adapter* adapter;

		YumeConfigOption optDevice;
		YumeConfigOption optVideoMode;
		YumeConfigOption optFullScreen;
		YumeConfigOption optVSync;
		YumeConfigOption optExceptionsErrorLevel;
		YumeConfigOption optDriverType;

		adapterList = this->GetAdapters();

		optDevice.name = "Rendering Device";
		optDevice.currentValue.clear();
		optDevice.possibleValues.clear();
		optDevice.immutable = false;

		optVideoMode.name = "Video Mode";
		optVideoMode.currentValue = "800 x 600 @ 32-bit colour";
		optVideoMode.immutable = false;

		optFullScreen.name = "Full Screen";
		optFullScreen.possibleValues.push_back("Yes");
		optFullScreen.possibleValues.push_back("No");
		optFullScreen.currentValue = "No";
		optFullScreen.immutable = false;

		optExceptionsErrorLevel.name = "Information Queue Exceptions Bottom Level";
		optExceptionsErrorLevel.possibleValues.push_back("No information queue exceptions");
		optExceptionsErrorLevel.possibleValues.push_back("Corruption");
		optExceptionsErrorLevel.possibleValues.push_back("Error");
		optExceptionsErrorLevel.possibleValues.push_back("Warning");
		optExceptionsErrorLevel.possibleValues.push_back("Info (exception on any message)");
#ifdef YUME_DEBUG_MODE
		optExceptionsErrorLevel.currentValue = "Info (exception on any message)";
#else
		optExceptionsErrorLevel.currentValue = "No information queue exceptions";
#endif
		optExceptionsErrorLevel.immutable = false;

		optDriverType.name = "Driver type";
		optDriverType.possibleValues.push_back("Hardware");
		optDriverType.possibleValues.push_back("Software");
		optDriverType.possibleValues.push_back("Warp");
		optDriverType.currentValue = "Hardware";
		optDriverType.immutable = false;



		for (unsigned j = 0; j < adapterList->GetCount(); j++)
		{
			adapter = adapterList->Get(j);
			optDevice.possibleValues.push_back(adapter->DriverDescription());
			// Make first one default
			if (j == 0)
				optDevice.currentValue = adapter->DriverDescription();
		}
		m_Options[optDevice.name] = optDevice;
		m_Options[optVideoMode.name] = optVideoMode;
		m_Options[optFullScreen.name] = optFullScreen;
		m_Options[optExceptionsErrorLevel.name] = optExceptionsErrorLevel;
		m_Options[optDriverType.name] = optDriverType;

		RefreshD3DSettings();
	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::RefreshD3DSettings()
	{
		YumeConfigOption* optVideoMode;
		YumeD3D11Adapter* driver = 0;
		YumeD3D11AdapterInfo* videoMode;

		ConfigOptionMap::iterator opt = m_Options.find("Rendering Device");
		if (opt != m_Options.end())
		{
			for (unsigned j = 0; j < GetAdapters()->GetCount(); j++)
			{
				driver = GetAdapters()->Get(j);
				if (driver->DriverDescription() == opt->second.currentValue)
					break;
			}

			if (driver)
			{
				opt = m_Options.find("Video Mode");
				optVideoMode = &opt->second;
				optVideoMode->possibleValues.clear();
				// get vide modes for this device
				for (unsigned k = 0; k < driver->getVideoModeList()->count(); k++)
				{
					videoMode = driver->getVideoModeList()->item(k);
					optVideoMode->possibleValues.push_back(videoMode->GetDescription());
				}

				// Reset video mode to default if previous doesn't avail in new possible values
				StringVector::const_iterator itValue =
					std::find(optVideoMode->possibleValues.begin(),
						optVideoMode->possibleValues.end(),
						optVideoMode->currentValue);
				if (itValue == optVideoMode->possibleValues.end())
				{
					optVideoMode->currentValue = "800 x 600 @ 32-bit colour";
				}
			}
		}

	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::SetRenderTarget(YumeRenderTarget * target)
	{
		m_pActiveRenderTarget = target;
		if (m_pActiveRenderTarget)
		{
			ID3D11RenderTargetView ** pRTView = NULL;
			target->GetCustomAttribute("ID3D11RenderTargetView", &pRTView);

			unsigned int numberOfViews;
			target->GetCustomAttribute("numberOfViews", &numberOfViews);

			////Retrieve depth buffer
			//D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());

			//if (target->getDepthBufferPool() != DepthBuffer::POOL_NO_DEPTH && !depthBuffer)
			//{
			//	//Depth is automatically managed and there is no depth buffer attached to this RT
			//	//or the Current D3D device doesn't match the one this Depth buffer was created
			//	setDepthBufferFor(target);
			//}

			////Retrieve depth buffer again (it may have changed)
			//depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());


			// we need to clear the state 
			m_Device.GetImmediateContext()->ClearState();

			if (m_Device.isError())
			{
				YumeString errorDescription = m_Device.getErrorDescription();
				YUMELOG_DEBUG("Can't clear immediate context state!");
			}



			// now switch to the new render target
			m_Device.GetImmediateContext()->OMSetRenderTargets(
				numberOfViews,
				pRTView,
				/*depthBuffer ? depthBuffer->getDepthStencilView() :*/ 0);

			if (m_Device.isError())
			{
				YumeString errorDescription = m_Device.getErrorDescription();
				YUMELOG_DEBUG("Can't set render targets!");
			}
		}
		// TODO - support MRT

	}
	//---------------------------------------------------------------------
	const YumeString& YumeD3D11Renderer::GetName()
	{
		return "Yume Direct3D 11";
	}
	//---------------------------------------------------------------------
	YumeRenderWindow* YumeD3D11Renderer::Initialize(bool autoCreate, const YumeString& Title)
	{
		YUMELOG_DEBUG("Initializing D3D11 Renderer!");
		YumeConfigOption* optVideoMode;
		m_CurrAdapter = 0;
		YumeD3D11AdapterInfo* videoMode;

		YumeRenderWindow* AutoWindow = NULL;

		ConfigOptionMap::iterator opt = m_Options.find("Rendering Device");
		if (opt != m_Options.end())
		{
			for (unsigned j = 0; j < GetAdapters()->GetCount(); j++)
			{
				m_CurrAdapter = GetAdapters()->Get(j);
				if (m_CurrAdapter->DriverDescription() == opt->second.currentValue)
					break;
			}

			if (m_CurrAdapter)
			{
				opt = m_Options.find("Video Mode");
				optVideoMode = &opt->second;
				optVideoMode->possibleValues.clear();
				// get vide modes for this device
				for (unsigned k = 0; k < m_CurrAdapter->getVideoModeList()->count(); k++)
				{
					videoMode = m_CurrAdapter->getVideoModeList()->item(k);
					optVideoMode->possibleValues.push_back(videoMode->GetDescription());
				}

				// Reset video mode to default if previous doesn't avail in new possible values
				StringVector::const_iterator itValue =
					std::find(optVideoMode->possibleValues.begin(),
						optVideoMode->possibleValues.end(),
						optVideoMode->currentValue);
				if (itValue == optVideoMode->possibleValues.end())
				{
					optVideoMode->currentValue = "800 x 600 @ 32-bit colour";
				}
			}
		}


		if (!m_Device.isNull())
		{
			m_Device.Release();
		}

		opt = m_Options.find("Information Queue Exceptions Bottom Level");
		if (opt == m_Options.end())
			YUMELOG_DEBUG("Can't find option: Information Queue Exceptions Bottom Level");
		YumeString infoQType = opt->second.currentValue;

		if ("No information queue exceptions" == infoQType)
		{
			YumeD3D11Device::SetExceptionsErrorLevel(YumeD3D11Device::D3D_NO_EXCEPTION);
		}
		else if ("Corruption" == infoQType)
		{
			YumeD3D11Device::SetExceptionsErrorLevel(YumeD3D11Device::D3D_CORRUPTION);
		}
		else if ("Error" == infoQType)
		{
			YumeD3D11Device::SetExceptionsErrorLevel(YumeD3D11Device::D3D_ERROR);
		}
		else if ("Warning" == infoQType)
		{
			YumeD3D11Device::SetExceptionsErrorLevel(YumeD3D11Device::D3D_WARNING);
		}
		else if ("Info (exception on any message)" == infoQType)
		{
			YumeD3D11Device::SetExceptionsErrorLevel(YumeD3D11Device::D3D_INFO);
		}

		opt = m_Options.find("Driver type");
		if (opt == m_Options.end())
			YUMELOG_DEBUG("Can't find option:Driver type");
		YumeString driverTypeName = opt->second.currentValue;

		m_DriverType = HARDWARE;
		if ("Hardware" == driverTypeName)
		{
			m_DriverType = HARDWARE;
		}
		if ("Software" == driverTypeName)
		{
			m_DriverType = SOFTWARE;
		}
		if ("Warp" == driverTypeName)
		{
			m_DriverType = WARP;
		}

		UINT deviceFlags = 0;
		if (YumeD3D11Device::D3D_NO_EXCEPTION != YumeD3D11Device::GetExceptionsErrorLevel())
		{
			deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;

		IDXGIAdapter1* pSelectedAdapter = m_CurrAdapter->getDeviceAdapter();

		D3D_FEATURE_LEVEL RequestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
		unsigned int RequestedLevelsSize = sizeof(RequestedLevels) / sizeof(RequestedLevels[0]);
		ID3D11Device * device;
		// But, if creating WARP or software, don't use a selected adapter, it will be selected automatically
		if (FAILED(D3D11CreateDevice(m_DriverType != WARP ? pSelectedAdapter : NULL,
			driverType,
			NULL,
			deviceFlags,
			RequestedLevels,
			RequestedLevelsSize,
			D3D11_SDK_VERSION,
			&device,
			0,
			0)))
		{
			YUMELOG_DEBUG("Failed to create D3D11 Device");
		}

		if (m_DriverType != HARDWARE)
		{
			// get the IDXGIFactory1 from the device for software drivers
			// Remark(dec-09):
			//  Seems that IDXGIFactory1::CreateSoftwareAdapter doesn't work with
			// D3D11CreateDevice - so I needed to create with pSelectedAdapter = 0.
			// If pSelectedAdapter == 0 then you have to get the IDXGIFactory1 from
			// the device - else CreateSwapChain fails later.
			//  Update (Jun 12, 2012)
			// If using WARP driver, get factory from created device
			SAFE_RELEASE(m_pDXGIFactory);

			IDXGIDevice1 * pDXGIDevice;
			device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&pDXGIDevice);

			IDXGIAdapter1 * pDXGIAdapter;
			pDXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void **)&pDXGIAdapter);

			pDXGIAdapter->GetParent(__uuidof(IDXGIFactory1), (void **)&m_pDXGIFactory);

			SAFE_RELEASE(pDXGIAdapter);
			SAFE_RELEASE(pDXGIDevice);
		}


		m_Device = YumeD3D11Device(device);

		if (autoCreate)
		{
			bool fullScreen;
			opt = m_Options.find("Full Screen");
			if (opt == m_Options.end())
				YUMELOG_DEBUG("Can't find full-screen options!");
			fullScreen = opt->second.currentValue == "Yes";

			YumeD3D11AdapterInfo* videoMode = NULL;
			unsigned int width, height;
			YumeString temp;

			opt = m_Options.find("Video Mode");
			if (opt == m_Options.end())
				YUMELOG_DEBUG("Can't find adapter info options!");

			// The string we are manipulating looks like this :width x height @ colourDepth
			// Pull out the colour depth by getting what comes after the @ and a space
			YumeString colourDepth = opt->second.currentValue.substr(opt->second.currentValue.rfind('@') + 1);
			// Now we know that the width starts a 0, so if we can find the end we can parse that out
			YumeString::size_type widthEnd = opt->second.currentValue.find(' ');
			// we know that the height starts 3 characters after the width and goes until the next space
			YumeString::size_type heightEnd = opt->second.currentValue.find(' ', widthEnd + 3);
			// Now we can parse out the values
			width = YumeStringConverter::parseInt(opt->second.currentValue.substr(0, widthEnd));
			height = YumeStringConverter::parseInt(opt->second.currentValue.substr(widthEnd + 3, heightEnd));

			for (unsigned j = 0; j < m_CurrAdapter->getVideoModeList()->count(); j++)
			{
				temp = m_CurrAdapter->getVideoModeList()->item(j)->GetDescription();

				// In full screen we only want to allow supported resolutions, so temp and opt->second.currentValue need to 
				// match exacly, but in windowed mode we can allow for arbitrary window sized, so we only need
				// to match the colour values
				if (fullScreen && (temp == opt->second.currentValue) ||
					!fullScreen && (temp.substr(temp.rfind('@') + 1) == colourDepth))
				{
					videoMode = m_CurrAdapter->getVideoModeList()->item(j);
					break;
				}
			}

			if (!videoMode)
				YUMELOG_DEBUG("Can't find requested video mode!");

			// sRGB window option
			bool hwGamma = false;
			opt = m_Options.find("sRGB Gamma Conversion");
			if (opt == m_Options.end())
				YUMELOG_DEBUG("Can't find sRGB options!");
			/*hwGamma = opt->second.currentValue == "Yes";*/
			UINT fsaa = 0;
			YumeString fsaaHint;


			StrKeyValuePair miscParams;
			miscParams["colourDepth"] = YumeStringConverter::toString(videoMode->GetColourDepth());
			miscParams["FSAA"] = YumeStringConverter::toString(fsaa);
			miscParams["FSAAHint"] = fsaaHint;
			miscParams["vsync"] = YumeStringConverter::toString(mVSync);
			miscParams["vsyncInterval"] = YumeStringConverter::toString(mVSyncInterval);
			miscParams["gamma"] = YumeStringConverter::toString(hwGamma);

			AutoWindow = this->CreateRenderWindow(Title, width, height,
				fullScreen, &miscParams);

			// If we have 16bit depth buffer enable w-buffering.
			assert(AutoWindow);
			if (AutoWindow->GetColourDepth() == 16)
			{
				mWBuffer = true;
			}
			else
			{
				mWBuffer = false;
			}
		}
		
		YUMELOG_DEBUG("D3D11 renderer is ready!");

		YumeRenderer::Initialize(autoCreate);

		return AutoWindow;
	}
	//---------------------------------------------------------------------
	YumeRendererCapabilities* YumeD3D11Renderer::CreateRendererCapabilities() const
	{
		YumeRendererCapabilities* rsc = new YumeRendererCapabilities();
		rsc->setDriverVersion(mDriverVersion);
		rsc->setDeviceName(m_CurrAdapter->DriverDescription());
		rsc->setRenderSystemName("Yume Direct3D 11 Renderer");

		// Does NOT support fixed-function!
		//rsc->setCapability(RSC_FIXED_FUNCTION);

		rsc->setCapability(RSC_HWSTENCIL);
		rsc->setStencilBufferBitDepth(8);

		// Set number of texture units, always 16
		rsc->setNumTextureUnits(16);
		rsc->setCapability(RSC_ANISOTROPY);
		rsc->setCapability(RSC_AUTOMIPMAP);
		rsc->setCapability(RSC_BLENDING);
		rsc->setCapability(RSC_DOT3);
		// Cube map
		rsc->setCapability(RSC_CUBEMAPPING);

		// We always support compression, D3DX will decompress if device does not support
		rsc->setCapability(RSC_TEXTURE_COMPRESSION);
		rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
		rsc->setCapability(RSC_VBO);
		rsc->setCapability(RSC_SCISSOR_TEST);
		rsc->setCapability(RSC_TWO_SIDED_STENCIL);
		rsc->setCapability(RSC_STENCIL_WRAP);
		rsc->setCapability(RSC_HWOCCLUSION);

		//convertVertexShaderCaps(rsc);
		//convertPixelShaderCaps(rsc);
		//convertGeometryShaderCaps(rsc);

		rsc->setCapability(RSC_USER_CLIP_PLANES);
		rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

		rsc->setCapability(RSC_RTT_SEPARATE_DEPTHBUFFER);
		rsc->setCapability(RSC_RTT_MAIN_DEPTHBUFFER_ATTACHABLE);


		// Adapter details
		const DXGI_ADAPTER_DESC1& adapterID = m_CurrAdapter->getAdapterIdentifier();

		switch (m_DriverType) {
		case HARDWARE:
			// determine vendor
			// Full list of vendors here: http://www.pcidatabase.com/vendors.php?sort=id
			switch (adapterID.VendorId)
			{
			case 0x10DE:
				rsc->setVendor(GPU_NVIDIA);
				break;
			case 0x1002:
				rsc->setVendor(GPU_ATI);
				break;
			case 0x163C:
			case 0x8086:
				rsc->setVendor(GPU_INTEL);
				break;
			case 0x5333:
				rsc->setVendor(GPU_S3);
				break;
			case 0x3D3D:
				rsc->setVendor(GPU_3DLABS);
				break;
			case 0x102B:
				rsc->setVendor(GPU_MATROX);
				break;
			default:
				rsc->setVendor(GPU_UNKNOWN);
				break;
			};
			break;
		case SOFTWARE:
			rsc->setVendor(GPU_MS_SOFTWARE);
			break;
		case WARP:
			rsc->setVendor(GPU_MS_WARP);
			break;
		default:
			rsc->setVendor(GPU_UNKNOWN);
			break;
		}

		rsc->setCapability(RSC_INFINITE_FAR_PLANE);

		rsc->setCapability(RSC_TEXTURE_3D);
		rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
		rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
		rsc->setCapability(RSC_TEXTURE_FLOAT);

		rsc->setNumMultiRenderTargets(std::min(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, 8));
		rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);

		rsc->setCapability(RSC_POINT_SPRITES);
		rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
		rsc->setMaxPointSize(256); // TODO: guess!

		rsc->setCapability(RSC_VERTEX_TEXTURE_FETCH);
		rsc->setNumVertexTextureUnits(4);
		rsc->setVertexTextureUnitsShared(false);

		rsc->setCapability(RSC_MIPMAP_LOD_BIAS);

		// actually irrelevant, but set
		rsc->setCapability(RSC_PERSTAGECONSTANT);

		rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
		rsc->setCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER);

		rsc->Log();

		return rsc;
	}
	//---------------------------------------------------------------------
	YumeRenderWindow* YumeD3D11Renderer::CreateRenderWindow(const YumeString &name, unsigned int width, unsigned int height,
		bool fullScreen, const StrKeyValuePair *miscParams)
	{

		// Check we're not creating a secondary window when the primary
		// was fullscreen
		if (m_CurrentWindow && m_CurrentWindow->IsFullScreen())
		{
			YUMELOG_DEBUG("Can't create a window when there is a fullscreen one!");
		}
		if (m_CurrentWindow && fullScreen)
		{
			YUMELOG_DEBUG("Can't create a window when there is a fullscreen one! !!");
		}

		// Log a message
		YumeStringStream ss;
		ss << "D3D11RenderSystem::_createRenderWindow \"" << name << "\", " <<
			width << "x" << height << " ";
		if (fullScreen)
			ss << "fullscreen ";
		else
			ss << "windowed ";
		if (miscParams)
		{
			ss << " miscParams: ";
			StrKeyValuePair::const_iterator it;
			for (it = miscParams->begin(); it != miscParams->end(); ++it)
			{
				ss << it->first << "=" << it->second << " ";
			}
			YUMELOG_DEBUG(ss.str());
		}

		YumeString msg;

		// Make sure we don't already have a render target of the 
		// sam name as the one supplied
		if (mRenderTargets.find(name) != mRenderTargets.end())
		{
			YUMELOG_DEBUG("Can't create a window because there is an existing one with the same name");
		}

		YumeRenderWindow* win = new YumeD3D11RenderWindow(m_hInstance, m_Device, m_pDXGIFactory);

		win->Create(name, width, height, fullScreen, miscParams);

		AttachRenderTarget(*win);

		// If this is the first window, get the D3D device and create the texture manager
		if (!m_CurrentWindow)
		{
			m_CurrentWindow = (YumeD3D11RenderWindow *)win;
			win->GetCustomAttribute("D3DDEVICE", &m_Device);



			// if we are using custom capabilities, then 
			// mCurrentCapabilities has already been loaded
			/*if (!mUseCustomCapabilities)
				mCurrentCapabilities = mRealCapabilities;*/

				/*CreateRendererCapabilities(mCurrentCapabilities, m_CurrentWindow);*/
			m_pRealCapabilities = CreateRendererCapabilities();
			m_pRealCapabilities->addShaderProfile("hlsl");
		}
		else
		{
			mSecondaryWindows.push_back(static_cast<YumeD3D11RenderWindow *>(win));
		}

		return win;

	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::Clear(unsigned int buffers, float color[], float depth, unsigned short stencil)
	{
		if (m_pActiveRenderTarget)
		{
			ID3D11RenderTargetView ** pRTView;
			m_pActiveRenderTarget->GetCustomAttribute("ID3D11RenderTargetView", &pRTView);

			if (buffers & FBT_COLOUR)
			{
				unsigned int numberOfViews;
				m_pActiveRenderTarget->GetCustomAttribute("numberOfViews", &numberOfViews);
				if (numberOfViews == 1)
					m_Device.GetImmediateContext()->ClearRenderTargetView(pRTView[0], color);
				else
				{
					for (unsigned int i = 0; i < numberOfViews; ++i)
						m_Device.GetImmediateContext()->ClearRenderTargetView(pRTView[i], color);
				}
			}

			UINT ClearFlags = 0;
			if (buffers & FBT_DEPTH)
			{
				ClearFlags |= D3D11_CLEAR_DEPTH;
			}
			if (buffers & FBT_STENCIL)
			{
				ClearFlags |= D3D11_CLEAR_STENCIL;
			}

			if (ClearFlags)
			{
				/*D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(mActiveRenderTarget->
					getDepthBuffer());*/
				/*if (depthBuffer)
				{
					mDevice.GetImmediateContext()->ClearDepthStencilView(
						depthBuffer->getDepthStencilView(),
						ClearFlags, depth, static_cast<UINT8>(stencil));
				}*/
			}
		}
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterList* YumeD3D11Renderer::GetAdapters()
	{
		if (!m_AdapterList)
			m_AdapterList = new YumeD3D11AdapterList(m_pDXGIFactory);

		return m_AdapterList;
	}
	//---------------------------------------------------------------------
	void YumeD3D11Renderer::Shutdown()
	{
		YumeRenderer::Shutdown();

		m_bRendererInitialized = false;

		m_CurrentWindow = NULL;

		m_Device.Release();

		m_Device = NULL;

		SAFE_DELETE(m_AdapterList);
		SAFE_RELEASE(m_pDXGIFactory);
		m_CurrAdapter = NULL;


	}
	//---------------------------------------------------------------------
}
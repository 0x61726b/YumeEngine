//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeDebugOverlay.h"
#include "YumeUIBinding.h"
#include "Renderer/YumeRHI.h"
#include "Engine/YumeEngine.h"
#include "Core/YumeDefaults.h"

#include "Renderer/YumeCamera.h"
#include "Renderer/YumeRenderPipeline.h"
#include "Scene/YumeSceneNode.h"

static const char* qualityTexts[] =
{
	"Low",
	"Med",
	"High"
};

static const char* shadowQualityTexts[] =
{
	"16bit Simple",
	"24bit Simple",
	"16bit PCF",
	"24bit PCF",
	"VSM",
	"Blurred VSM"
};

namespace YumeEngine
{
	YumeDebugOverlay::YumeDebugOverlay()
		: YumeUIElement("Overlay")
	{
		Initialize();
	}
	YumeDebugOverlay::~YumeDebugOverlay()
	{
	}



	void YumeDebugOverlay::OnDomEvent(const YumeString& domElement,DOMEvents event,const YumeString& data)
	{
		switch(event)
		{
		case DOMEvents::InputChecked:
		{
			if(domElement == ("bloomSwitch"))
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
					fx->SetEnabled("Bloom",true);
				else
					fx->SetEnabled("Bloom",false);
			}
			if(domElement == ("bloomHDRSwitch"))
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
					fx->SetEnabled("BloomHDR",true);
				else
					fx->SetEnabled("BloomHDR",false);
			}
			if(domElement == ("blurSwitch"))
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
					fx->SetEnabled("Blur",true);
				else
					fx->SetEnabled("Blur",false);
			}
			if(domElement == ("fxaaSwitch"))
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
					fx->SetEnabled("FXAA2",true);
				else
					fx->SetEnabled("FXAA2",false);
			}
			if(domElement == "autoexposureSwitch")
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
					fx->SetEnabled("AutoExposure",true);
				else
					fx->SetEnabled("AutoExposure",false);
			}
			if(domElement == "ssaoInput")
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

				if(data == ("true"))
				{
					fx->SetEnabled("LinearDepthSSAO",true);
					fx->SetEnabled("BlurGaussianDepth",true);
				}
				else
				{
					fx->SetEnabled("LinearDepthSSAO",false);
					fx->SetEnabled("BlurGaussianDepth",false);
				}

			}
		}
		break;
		case DOMEvents::InputValueChanged:
		{
			if(domElement == "bloomHDRThreshold")
			{
				YumeRenderPipeline* fxBloom = gYume->pRenderer->GetViewport(0)->GetRenderPath();
				float threshold = atof(data.c_str());
				threshold = threshold / 10.0f;
				fxBloom->SetShaderParameter("BloomHDRThreshold",threshold);
			}
			if(domElement == "bloomThreshold")
			{
				YumeRenderPipeline* fxBloom = gYume->pRenderer->GetViewport(0)->GetRenderPath();
				float threshold = atof(data.c_str());
				threshold = threshold / 10.0f;
				fxBloom->SetShaderParameter("BloomThreshold",threshold);
			}
			if(domElement == "blurSigma")
			{
				YumeRenderPipeline* fxBloom = gYume->pRenderer->GetViewport(0)->GetRenderPath();
				float sigma = atof(data.c_str());
				fxBloom->SetShaderParameter("BlurSigma",sigma);
			}
			if(domElement == "AEadaptRate")
			{
				YumeRenderPipeline* fxBloom = gYume->pRenderer->GetViewport(0)->GetRenderPath();
				float threshold = atof(data.c_str());
				threshold = threshold / 10.0f;
				fxBloom->SetShaderParameter("AutoExposureAdaptRate",threshold);
			}
			if(domElement == "ssaoRadius")
			{
				YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();
				float value = atof(data.c_str());
				fx->SetShaderParameter("Radius",value);
			}



		}
		break;
		default:
			break;
		}

	}

	bool YumeDebugOverlay::Initialize()
	{
		FsPath binaryRoot = gYume->pIO->GetBinaryRoot();
		std::string overlayPath("file:///");
		overlayPath.append(binaryRoot.generic_string());
		overlayPath.append("/Engine/Assets/UI/Overlay/Overlay.html");

		SetURL(overlayPath.c_str());


		elapsedTime_ = new YumeUIBinding(this,"ElapsedTime",0.0f,BINDING_DYNAMIC);
		sampleName_ = new YumeUIBinding(this,"SampleName","Overlay",BINDING_STATIC);

		AddBinding(elapsedTime_);
		AddBinding(sampleName_);

		gYume->pUI->AddDOMListener(this);

		return true;
	}

	void YumeDebugOverlay::OnContextReady()
	{
		sampleName_->SetDirty();


	}


	void YumeDebugOverlay::Update()
	{
		float elapsedTime = (gYume->pTimer->GetTimeStep());
		float fps = 1.0f / gYume->pEngine->GetSmoothedTimestep();
		int batchCount = gYume->pRHI->GetNumBatches();
		int primitives = gYume->pRHI->GetNumPrimitives();
		int lights = gYume->pRenderer->GetNumLights(true);
		int occluders = gYume->pRenderer->GetNumOccluders(true);
		int shadowMaps = gYume->pRenderer->GetNumShadowMaps(true);
		unsigned long long totalMemory = gYume->pResourceManager->GetTotalMemoryUse();

		YumeString shadowInfo;
		shadowInfo.AppendWithFormat("%s , Size: %i , Quality: %s ",
			gYume->pRenderer->GetDrawShadows() ? "On" : "Off",
			gYume->pRenderer->GetShadowMapSize(),
			shadowQualityTexts[gYume->pRenderer->GetShadowQuality()]);



		YumeString ret;
		ret.append("{");
		ret.AppendWithFormat("\"TotalMemory\": \"%f\",",totalMemory);
		ret.AppendWithFormat("\"ElapsedTime\": \"%f\",",elapsedTime);
		ret.AppendWithFormat("\"FrameRate\": \"%f\",",fps);
		ret.AppendWithFormat("\"ShadowInfo\": \"%s\",",shadowInfo.c_str());
		ret.AppendWithFormat("\"MaterialQuality\": \"%s\",",qualityTexts[gYume->pRenderer->GetMaterialQuality()]);
		ret.AppendWithFormat("\"TextureQuality\": \"%s\",",qualityTexts[gYume->pRenderer->GetTextureQuality()]);
		ret.AppendWithFormat("\"PrimitiveCount\": \"%i\",",primitives);
		ret.AppendWithFormat("\"BatchCount\": \"%i\",",batchCount);
		ret.AppendWithFormat("\"LightCount\": \"%i\",",lights);
		ret.AppendWithFormat("\"Occluders\": \"%i\",",occluders);
		ret.AppendWithFormat("\"ShadowMaps\": \"%i\"",shadowMaps);



		ret.append("}");
		gYume->pUI->SendEvent("setFrameInfo",ret);


		YumeCamera* cam = gYume->pRenderer->GetViewport(0)->GetCamera();
		YumeString camera;
		camera.append("{");
		camera.AppendWithFormat("\"CameraPos\": \"%s\",",cam->GetNode()->GetWorldPosition().ToString().c_str());
		camera.AppendWithFormat("\"CameraRot\": \"%s\",",cam->GetNode()->GetWorldRotation().ToString().c_str());
		camera.AppendWithFormat("\"CameraFov\": \"%f\"",cam->GetFov());
		camera.append("}");
		gYume->pUI->SendEvent("setCameraInfo",camera);

		YumeUIElement::Update();

	}




}














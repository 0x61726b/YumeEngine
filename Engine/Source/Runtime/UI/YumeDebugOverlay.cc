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

#include "Renderer/YumeMiscRenderer.h"
#include "Renderer/YumeLPVCamera.h"
#include "Renderer/YumePostProcess.h"

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

#define SOMETHING_STRANGE

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
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetBloomEnabled(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetBloomEnabled(false);
			}

			if(domElement == ("fxaaSwitch"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetFxaaEnabled(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetFxaaEnabled(false);
			}

			if(domElement == ("dofSwitch"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetDofEnabled(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetDofEnabled(false);
			}


			if(domElement == "ssaoSwitch")
			{
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetSSAOEnabled(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetSSAOEnabled(false);
			}

			if(domElement == "godraysSwitch")
			{
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetGodraysEnabled(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetGodraysEnabled(false);
			}

			if(domElement == "autoexposureSwitch")
			{
				if(data == ("true"))
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetExposureAdapt(true);
				else
					gYume->pRenderer->GetMiscRenderer()->GetPP()->SetExposureAdapt(false);
			}

			if(domElement == "debugIndirect")
			{
				if(data == "true")
				{
					gYume->pRenderer->GetMiscRenderer()->SetGIDebug(true);
				}
				else
				{
					gYume->pRenderer->GetMiscRenderer()->SetGIDebug(false);
				}
			}

		}
		break;
		case DOMEvents::InputValueChanged:
		{
			if(domElement == "bloomThreshold")
			{
				float threshold = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetBloomThreshold(threshold);

			}
			if(domElement == "bloomSigma")
			{
				float bloomSigma = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetBloomSigma(bloomSigma);
			}


			if(domElement == "ssaoScale")
			{
				float ssaoScale = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetSSAOScale(ssaoScale);
			}


			if(domElement == "aeExposureKey")
			{
				float aeExposureKey = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetAutoExposureKey(aeExposureKey);
			}

			if(domElement == "aeAdaptSpeed")
			{
				float aeAdaptSpeed = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetAutoExposureSpeed(aeAdaptSpeed);
			}


			if(domElement == "dofCocScale")
			{
				float dofCocScale = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetDoFCoCScale(dofCocScale);
			}

			if(domElement == "dofFocalPlane")
			{
				float dofFocalPlane = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetDoFFocalPlane(dofFocalPlane);
			}
			if(domElement == "gTau")
			{
				float gTau = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->GetPP()->SetGodraysTAU(gTau);
			}


			/* End Post Process Parameters */

			/* LPV GI Parameters */

			if(domElement == "iGiScale")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetGIScale(value);
			}
			if(domElement == "iGiLPVFlux")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetGIScale(value);
			}

			if(domElement == "iLpvX")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetLPVPos(value,-1,-1);
			}
			if(domElement == "iLpvY")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetLPVPos(-1,value,-1);
			}
			if(domElement == "iLpvZ")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetLPVPos(-1,-1,value);
			}

			if(domElement == "iGiLightFlux")
			{
				float value = atof(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetLightFlux(value);
			}

			if(domElement == "iGiPropagations")
			{
				int value = atoi(data.c_str());
				gYume->pRenderer->GetMiscRenderer()->SetLPVNumberIterations(value);
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


#ifndef SOMETHING_STRANGE
		YumeCamera* cam = gYume->pRenderer->GetViewport(0)->GetCamera();
		YumeString camera;
		camera.append("{");
		camera.AppendWithFormat("\"CameraPos\": \"%s\",",cam->GetNode()->GetWorldPosition().ToString().c_str());
		camera.AppendWithFormat("\"CameraRot\": \"%s\",",cam->GetNode()->GetWorldRotation().ToString().c_str());
		camera.AppendWithFormat("\"CameraFov\": \"%f\"",cam->GetFov());
		camera.append("}");
		gYume->pUI->SendEvent("setCameraInfo",camera);
#else
		YumeLPVCamera* cam = gYume->pRenderer->miscRenderer_->GetCamera();

		YumeString camera;
		camera.append("{");
		camera.AppendWithFormat("\"CameraPos\": \"%s\",",XMVectorToString(cam->GetEyePt()).c_str());
		/*camera.AppendWithFormat("\"CameraRot\": \"%s\",",cam->GetNode()->GetWorldRotation().ToString().c_str());*/
		camera.AppendWithFormat("\"CameraFov\": \"%f\"",45.0f);
		camera.append("}");
		gYume->pUI->SendEvent("setCameraInfo",camera);
#endif
		YumeUIElement::Update();

	}




}














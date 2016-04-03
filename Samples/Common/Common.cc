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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "Common.h"
#include "YumeHeaders.h"

#include "Engine/YumeEngine.h"
#include "Renderer/YumeRHI.h"
#include "Core/YumeEnvironment.h"
#include "Scene/YumeOctree.h"
#include "Renderer/YumeAuxRenderer.h"
#include "Renderer/YumeRenderer.h"
#include "Renderer/YumeRenderPipeline.h"
#include "Renderer/YumeCamera.h"

#include "Input/YumeInput.h"

#include "UI/YumeDebugOverlay.h"

namespace YumeEngine
{
	BaseApplication::BaseApplication()
		: yaw_(0.0f),
		pitch_(0.0f),
		drawDebug_(false),
		ssaoDebug_(false),
		gbufferDebugIndex_(0)
	{
		REGISTER_ENGINE_LISTENER;
	}

	BaseApplication::~BaseApplication()
	{
	}

	void BaseApplication::SetupWindowProperties()
	{
		//engine_->GetRenderer()->SetWindowTitle("Yume Engine Sample App");
	}

	void BaseApplication::Setup()
	{
		//Set engine parameters

		engineVariants_["ResourceTree"] = YumeString("Engine/Assets");
	}


	void BaseApplication::HandleKeyDown(unsigned key,unsigned mouseButton,int repeat)
	{
		YumeInput* input = gYume->pInput;

		if(key == KEY_ESC)
		{
			engine_->Exit();
		}

		if(key == KEY_M)
		{
			input->SetMouseVisible(!input->IsMouseVisible());
		}

		else if(key == '1')
		{
			int quality = gYume->pRenderer->GetTextureQuality();
			++quality;
			if(quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			gYume->pRenderer->SetTextureQuality(quality);
		}

		// Material quality
		else if(key == '2')
		{
			int quality = gYume->pRenderer->GetMaterialQuality();
			++quality;
			if(quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			gYume->pRenderer->SetMaterialQuality(quality);
		}
		else if(key == '3')
			gYume->pRenderer->SetDrawShadows(!gYume->pRenderer->GetDrawShadows());
		else if(key == '4')
		{
			int shadowMapSize = gYume->pRenderer->GetShadowMapSize();
			shadowMapSize *= 2;
			if(shadowMapSize > 2048)
				shadowMapSize = 512;
			gYume->pRenderer->SetShadowMapSize(shadowMapSize);
		}
		else if(key == '5')
		{
			ShadowQuality quality = gYume->pRenderer->GetShadowQuality();
			quality = (ShadowQuality)(quality + 1);
			if(quality > SHADOWQUALITY_BLUR_VSM)
				quality = SHADOWQUALITY_SIMPLE_16BIT;
			gYume->pRenderer->SetShadowQuality(quality);
		}
		else if(key == '6')
		{
			bool occlusion = gYume->pRenderer->GetMaxOccluderTriangles() > 0;
			occlusion = !occlusion;
			gYume->pRenderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
		}

		if(input->GetKeyPress(KEY_U))
			gYume->pUI->SetUIEnabled(!gYume->pUI->GetUIEnabled());

		if(input->GetKeyPress(KEY_SPACE))
			drawDebug_ = !drawDebug_;

		if(input->GetKeyPress(KEY_G))
		{
			YumeRenderer* renderer = gYume->pRenderer;
			renderer->SetGBufferDebugRendering(!renderer->GetGBufferDebugRendering());
		}

		//FX Debug

		if(input->GetKeyPress(KEY_F2))
		{
			ssaoDebug_ = !ssaoDebug_;

			YumeViewport* viewport = gYume->pRenderer->GetViewport(0);
			YumeRenderPipeline* pipeline = viewport->GetRenderPath();

			RenderCommand p = pipeline->commands_[ssaoCommandIndex_];

			if(ssaoDebug_)
				p.pixelShaderDefines_ = "DEBUG_AO";
			else
				p.pixelShaderDefines_ = "COMBINE";

			pipeline->RemoveCommand(ssaoCommandIndex_);
			pipeline->InsertCommand(ssaoCommandIndex_,p);
		}

		if(input->GetKeyPress(KEY_F1))
		{
			YumeViewport* viewport = gYume->pRenderer->GetViewport(0);
			YumeRenderPipeline* pipeline = viewport->GetRenderPath();

			pipeline->ToggleEnabled("DebugGBuffer");
			gYume->pRenderer->SetGBufferDebugRendering(!gYume->pRenderer->GetGBufferDebugRendering());
		}
	}

	void BaseApplication::HandlePostRenderUpdate(float timeStep)
	{
		if(drawDebug_)
		{
			gYume->pRenderer->DrawDebugGeometry(false);
			//scene_->GetComponent<Octree>()->DrawDebugGeometry(false);
		}
	}

	void BaseApplication::Start()
	{
		SetupWindowProperties();

		gYume->pInput->AddListener(this);

		overlay_ = new YumeDebugOverlay;
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);

		SharedPtr<YumeViewport> viewport(new YumeViewport(scene_,cameraNode_->GetComponent<YumeCamera>()));

		YumeRenderPipeline* pipeline = viewport->GetRenderPath();
		pipeline->Append(gYume->pResourceManager->PrepareResource<YumeXmlFile>("PostFX/Bloom.xml"));
		pipeline->Append(gYume->pResourceManager->PrepareResource<YumeXmlFile>("PostFX/BloomHDR.xml"));
		pipeline->Append(gYume->pResourceManager->PrepareResource<YumeXmlFile>("PostFX/Blur.xml"));
		pipeline->Append(gYume->pResourceManager->PrepareResource<YumeXmlFile>("PostFX/FXAA2.xml"));
		pipeline->Append(gYume->pResourceManager->PrepareResource<YumeXmlFile>("PostFX/AutoExposure.xml"));
		pipeline->SetShaderParameter("BloomMix",Vector2(0.9f,0.6f));
		pipeline->SetShaderParameter("BloomHDRThreshold",0.8f);
		pipeline->SetEnabled("Bloom",false);
		pipeline->SetEnabled("AutoExposure",false);
		pipeline->SetEnabled("BloomHDR",false);
		pipeline->SetEnabled("Blur",false);
		pipeline->SetEnabled("AutoExposure",false);
		pipeline->SetEnabled("FXAA2",false);
		viewport->SetRenderPath(pipeline);

		YumeRenderer* renderer = gYume->pRenderer;
		renderer->SetViewport(0,viewport);

		for(int i=0; i < pipeline->GetNumCommands(); ++i)
		{
			if(pipeline->commands_[i].tag_ == "LinearDepthSSAO")
				ssaoCommandIndex_ = i;
			if(pipeline->commands_[i].tag_ == "DebugGBuffer")
				gbufferDebugIndex_ = i;
		}

		Vector3 ao_radius = Vector3(1.0f,0.0f,4.0f);
		Vector3 ao_intensity = Vector3(0.15f,0.0f,2.0f);
		Vector3 ao_projscale = Vector3(0.3f,0.0f,1.0f);
		Vector3 ao_bias = Vector3(0.01f,0.0f,0.1f);

		pipeline->SetShaderParameter("Radius",ao_radius.x_);
		pipeline->SetShaderParameter("ProjScale2",ao_projscale.x_);
		pipeline->SetShaderParameter("IntensityDivR6",ao_intensity.x_);
		pipeline->SetShaderParameter("Bias",ao_bias.x_);

		pipeline->SetEnabled("LinearDepthSSAO",true);
		pipeline->SetEnabled("BlurGaussianDepth",false);
		

	}

	void BaseApplication::Exit()
	{
	}
}

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

#include "Input/YumeInput.h"

#include "UI/YumeDebugOverlay.h"

namespace YumeEngine
{
	BaseApplication::BaseApplication()
		: yaw_(0.0f),
		pitch_(0.0f),
		drawDebug_(false)
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
	}

	void BaseApplication::Exit()
	{
	}
}

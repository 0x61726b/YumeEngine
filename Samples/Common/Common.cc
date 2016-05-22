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


#include "Input/YumeInput.h"

#include "UI/YumeDebugOverlay.h"
#include "UI/YumeOptionsMenu.h"
#include "Renderer/RenderPass.h"

#include "Renderer/Scene.h"
namespace YumeEngine
{
	BaseApplication::BaseApplication()
		: yaw_(0.0f),
		pitch_(0.0f),
		drawDebug_(false),
		ssaoDebug_(false),
		toggleGDebug_(false),
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

		YumeVector<YumeString>::type params = GetArguments();

		if(params.size() > 1)
		{
			for(int i=1; i < params.size(); ++i)
			{
				YumeString param = params[i];

				

				if(param.Contains('='))
				{
					YumeVector<YumeString>::type split = param.Split('=');
					YumeString left = split[0];
					YumeString right = split[1];

					if(left == "WindowWidth")
						engineVariants_["WindowWidth"] = atoi(right.c_str());
					if(left == "WindowHeight")
						engineVariants_["WindowHeight"] = atoi(right.c_str());
					if(left == "GI")
						engineVariants_["GI"] = atoi(right.c_str());
					if(left == "CamMoveSpeed")
						engineVariants_["CamMoveSpeed"] = atoi(right.c_str());
				}

			}
		}
		else
		{
			engineVariants_["WindowWidth"] = 1024;;
			engineVariants_["WindowHeight"] = 768;
		}
	}


	StaticModel* BaseApplication::CreateModel(const YumeString& name,DirectX::XMFLOAT3 Pos,DirectX::XMFLOAT4 rot,DirectX::XMFLOAT3 scale)
	{
		StaticModel* sm_ = new StaticModel(name);
		sm_->Initialize();
		sm_->SetName("Cornell");
		sm_->SetPosition(DirectX::XMLoadFloat3(&Pos),true);
		sm_->SetRotation(DirectX::XMLoadFloat4(&rot));
		sm_->SetScale(scale.x,scale.y,scale.z);


		gYume->pRenderer->GetScene()->AddNode(sm_);
		gYume->pRenderer->UpdateMeshBb(sm_,sm_->GetTransformation());

		return sm_;
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

		if(input->GetKeyPress(KEY_U))
			gYume->pUI->SetUIEnabled(!gYume->pUI->GetUIEnabled());
		if(input->GetKeyPress(KEY_SPACE))
			drawDebug_ = !drawDebug_;

		if(input->GetKeyPress(KEY_F1))
		{
			toggleGDebug_ = !toggleGDebug_;
			if(toggleGDebug_)
				gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("ShowGBuffer");
			else
				gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("ShowGBuffer");
		}


		if(input->GetKeyPress(KEY_1))
			ToggleShaderParameter("showDiffuse");
		if(input->GetKeyPress(KEY_2))
			ToggleShaderParameter("showNormals");
		if(input->GetKeyPress(KEY_3))
			ToggleShaderParameter("showSpec");
		if(input->GetKeyPress(KEY_4))
			ToggleShaderParameter("showDepth");


	}

	void BaseApplication::ToggleShaderParameter(YumeHash param)
	{
		RenderPass* dp = gYume->pRenderer->GetDefaultPass();

		bool c = dp->GetShaderParameter(param).Get<bool>();

		dp->SetShaderParameter(param,!c);
	}

	void BaseApplication::HandlePostRenderUpdate(float timeStep)
	{

	}

	void BaseApplication::Start()
	{
		SetupWindowProperties();

		gYume->pInput->AddListener(this);
	}

	void BaseApplication::Exit()
	{
	}
}

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
			gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("ShowGBuffer");



	
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

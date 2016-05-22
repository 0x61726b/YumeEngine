//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"
#include "Lighting.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"

#include <boost/shared_ptr.hpp>

#include "Input/YumeInput.h"

#include "Renderer/YumeTexture2D.h"

#include "Renderer/YumeResourceManager.h"

#include "Engine/YumeEngine.h"

#include "UI/YumeDebugOverlay.h"

#include "Renderer/Light.h"
#include "Renderer/StaticModel.h"
#include "Renderer/Scene.h"

#include "UI/YumeOptionsMenu.h"

YUME_DEFINE_ENTRY_POINT(YumeEngine::LightingDemo);

namespace YumeEngine
{



	LightingDemo::LightingDemo()
		: drawDebug(false)
	{

	}

	LightingDemo::~LightingDemo()
	{

	}

	void LightingDemo::Start()
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeMiscRenderer* renderer = gYume->pRenderer;
		Scene* scene = renderer->GetScene();
		YumeCamera* camera = renderer->GetCamera();

		gYume->pInput->AddListener(this);

#ifndef DISABLE_CEF
		optionsMenu_ = new YumeOptionsMenu;
		gYume->pUI->AddUIElement(optionsMenu_);
		optionsMenu_->SetVisible(true);


		overlay_ = new YumeDebugOverlay;
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);
		overlay_->GetBinding("SampleName")->SetValue("Sponza GI Showcase");
#endif

		MaterialPtr diff = YumeAPINew Material;
		diff->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,0,0,1));
		diff->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		diff->SetShaderParameter("Roughness",1.0f);
		diff->SetShaderParameter("ShadingMode",0);
		diff->SetShaderParameter("has_diffuse_tex",false);
		diff->SetShaderParameter("has_alpha_tex",false);
		diff->SetShaderParameter("has_specular_tex",false);
		diff->SetShaderParameter("has_normal_tex",false);
		diff->SetShaderParameter("has_roughness_tex",false);

		StaticModel* model = CreateModel("Models/sponza/sponza.yume");

		RenderPass* dp = renderer->GetDefaultPass();
		dp->Load("RenderCalls/Bloom.xml",true);
		dp->Load("RenderCalls/FXAA.xml",true);
		dp->Load("RenderCalls/LensDistortion.xml",true);
		dp->Load("RenderCalls/ShowGBuffer.xml",true);
		dp->DisableRenderCalls("ShowGBuffer");
		dp->DisableRenderCalls("Bloom");


		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(100,2000,90,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);

	}

	void LightingDemo::CreateLight(Vector3 pos,YumeColor color)
	{


	}


	void LightingDemo::MoveCamera(float timeStep)
	{

	}


	void LightingDemo::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);
	}

	void LightingDemo::Setup()
	{
		engineVariants_["GI"] = SVO;
		engineVariants_["WindowWidth"] = 1024;
		engineVariants_["WindowHeight"] = 768;

		BaseApplication::Setup();
	}
}
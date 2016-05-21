//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"
#include "Shadows.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"
#include "Renderer/YumeLPVCamera.h"
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
#include "Renderer/RenderPass.h"

YUME_DEFINE_ENTRY_POINT(YumeEngine::ShadowsDemo);

namespace YumeEngine
{



	ShadowsDemo::ShadowsDemo():
		angle_(0)
	{

	}

	ShadowsDemo::~ShadowsDemo()
	{

	}

	void ShadowsDemo::Start()
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
#endif

		RenderPass* dp = renderer->GetDefaultPass();
		dp->Load("RenderCalls/Bloom.xml",true);
		dp->Load("RenderCalls/FXAA.xml",true);
		dp->Load("RenderCalls/LensDistortion.xml",true);


		MaterialPtr diff = YumeAPINew Material;
		diff->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		diff->SetShaderParameter("Roughness",1.0f);
		diff->SetShaderParameter("ShadingMode",2.0f);
		diff->SetShaderParameter("has_diffuse_tex",false);
		diff->SetShaderParameter("has_alpha_tex",false);
		diff->SetShaderParameter("has_specular_tex",false);
		diff->SetShaderParameter("has_normal_tex",false);
		diff->SetShaderParameter("has_roughness_tex",false);


		dragon_ = CreateModel("Models/dragon/dragon.yume");
		dragon_->SetScale(18,18,18);
		dragon_->SetPosition(DirectX::XMVectorSet(0,10,0,1));

		StaticModel* mitsubaLeft = CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(-5,0,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(2.3f,2.3f,2.3f));
		StaticModel* mitsubaRight = CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(5,0,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(2.3f,2.3f,2.3f));

		mitsubaLeft->SetMaterial(diff);
		mitsubaRight->SetMaterial(diff);

		StaticModel* jeyjeyModel = CreateModel("Models/cornell/cornell-empty.yume");


		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,27,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);
	}

	void ShadowsDemo::CreateLight(Vector3 pos,YumeColor color)
	{


	}


	void ShadowsDemo::MoveCamera(float timeStep)
	{


	}


	void ShadowsDemo::HandleUpdate(float timeStep)
	{
		const float RotationSpeed = 0.35f;
		angle_ += M_PI * RotationSpeed * timeStep;

		dragon_->SetRotation(DirectX::XMVectorSet(0,angle_,0,1));
	}

	void ShadowsDemo::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
		engineVariants_["GI"] = LPV;
	}
}

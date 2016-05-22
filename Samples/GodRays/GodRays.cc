//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------


#include "Core/YumeHeaders.h"
#include "GodRays.h"
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

YUME_DEFINE_ENTRY_POINT(YumeEngine::GodRays);

#define TURNOFF 1
//#define NO_MODEL
//#define OBJECTS_CAST_SHADOW
#define NO_SKYBOX
#define NO_PLANE

namespace YumeEngine
{



	GodRays::GodRays()
		: angle1_(0),
		updown1_(0),
		leftRight1_(0)
	{
		REGISTER_ENGINE_LISTENER;
	}

	GodRays::~GodRays()
	{

	}

	void GodRays::Start()
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
		overlay_->GetBinding("SampleName")->SetValue("Cornell Showcase 2");
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);

#endif

		MaterialPtr emissiveBlue = YumeAPINew Material;
		emissiveBlue->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0,0,1,1));
		emissiveBlue->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveBlue->SetShaderParameter("Roughness",0.001f);
		emissiveBlue->SetShaderParameter("ShadingMode",0);
		emissiveBlue->SetShaderParameter("has_diffuse_tex",false);
		emissiveBlue->SetShaderParameter("has_alpha_tex",false);
		emissiveBlue->SetShaderParameter("has_specular_tex",false);
		emissiveBlue->SetShaderParameter("has_normal_tex",false);
		emissiveBlue->SetShaderParameter("has_roughness_tex",false);

		MaterialPtr emissiveRed = YumeAPINew Material;
		emissiveRed->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,0,0,1));
		emissiveRed->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveRed->SetShaderParameter("Roughness",0.001f);
		emissiveRed->SetShaderParameter("ShadingMode",0);
		emissiveRed->SetShaderParameter("has_diffuse_tex",false);
		emissiveRed->SetShaderParameter("has_alpha_tex",false);
		emissiveRed->SetShaderParameter("has_specular_tex",false);
		emissiveRed->SetShaderParameter("has_normal_tex",false);
		emissiveRed->SetShaderParameter("has_roughness_tex",false);

		MaterialPtr emissivePink = YumeAPINew Material;
		emissivePink->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,0.0784314f,0.576471f,1));
		emissivePink->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		emissivePink->SetShaderParameter("Roughness",0.001f);
		emissivePink->SetShaderParameter("ShadingMode",0);
		emissivePink->SetShaderParameter("has_diffuse_tex",false);
		emissivePink->SetShaderParameter("has_alpha_tex",false);
		emissivePink->SetShaderParameter("has_specular_tex",false);
		emissivePink->SetShaderParameter("has_normal_tex",false);
		emissivePink->SetShaderParameter("has_roughness_tex",false);




		float boxScale = 0.15f;

		boxBlue = CreateModel("Models/Primitives/box.yume",DirectX::XMFLOAT3(2,5,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(1,1,1));
		boxBlue->SetMaterial(emissiveBlue);
		boxBlue->SetScale(boxScale,boxScale,boxScale);

		boxRed = CreateModel("Models/Primitives/box.yume",DirectX::XMFLOAT3(2,16.2f,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(1,1,1));
		boxRed->SetMaterial(emissiveRed);
		boxRed->SetScale(boxScale,boxScale,boxScale);

		boxPink = CreateModel("Models/Primitives/box.yume",DirectX::XMFLOAT3(-7,8,5),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(1,1,1));
		boxPink->SetMaterial(emissivePink);
		boxPink->SetScale(boxScale,boxScale,boxScale);

		StaticModel* jeyjeyModel = CreateModel("Models/cornell/cornell-empty.yume");

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
		dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);
	}


	void GodRays::MoveCamera(float timeStep)
	{

	}


	void GodRays::HandleUpdate(float timeStep)
	{
		const float YOrbitRadius = 5.f;
		const float ZOrbitRadius = 5.f;
		const float XOrbitRadius = 8;
		angle1_ += M_PI * 0.1f * timeStep;
		updown1_ += M_PI * 0.8f * timeStep;
		leftRight1_ += M_PI * 0.34f * timeStep;

		if(updown1_ > M_PI * 2)
			updown1_ = 0.0f;

		DirectX::XMVECTOR blueRot = DirectX::XMVectorSet(-angle1_,angle1_,0,0);
		DirectX::XMVECTOR bluePos = DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&boxBlue->GetInitialPosition()),DirectX::XMVectorSet(cosf(leftRight1_) * XOrbitRadius,0,sinf(leftRight1_) * ZOrbitRadius,0));

		boxBlue->SetRotation(blueRot);
		boxBlue->SetPosition(bluePos);

		DirectX::XMVECTOR redRot = DirectX::XMVectorSet(-angle1_,angle1_,0,0);
		DirectX::XMVECTOR redPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&boxRed->GetInitialPosition()),DirectX::XMVectorSet(cosf(updown1_) * YOrbitRadius,0,0,0));

		boxRed->SetRotation(redRot);
		boxRed->SetPosition(redPos);

		DirectX::XMVECTOR pinkRot = DirectX::XMVectorSet(-angle1_,angle1_,0,0);
		DirectX::XMVECTOR pinkPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&boxPink->GetInitialPosition()),DirectX::XMVectorSet(0,sinf(leftRight1_) * YOrbitRadius,0,0));

		boxPink->SetRotation(pinkRot);
		boxPink->SetPosition(pinkPos);
	}
	void GodRays::HandleRenderUpdate(float timeStep)
	{

	}

	void GodRays::Setup()
	{
		engineVariants_["GI"] = Dyn_Lpv;
		engineVariants_["WindowWidth"] = 1024;
		engineVariants_["WindowHeight"] = 768;

		BaseApplication::Setup();
	}
}

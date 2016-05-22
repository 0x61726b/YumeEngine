//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------


#include "Core/YumeHeaders.h"
#include "SponzaNormal.h"
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
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);
		overlay_->GetBinding("SampleName")->SetValue("Sponza");
#endif

		RenderPass* dp = renderer->GetDefaultPass();
		dp->Load("RenderCalls/Bloom.xml",true);
		dp->Load("RenderCalls/FXAA.xml",true);
		dp->Load("RenderCalls/LensDistortion.xml",true);
		dp->Load("RenderCalls/ShowGBuffer.xml",true);
		dp->DisableRenderCalls("ShowGBuffer");
		dp->DisableRenderCalls("Bloom");

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


		StaticModel* jeyjeyModel = CreateModel("Models/sponza/sponza.yume");

		jeyjeyModel->SetFloorRoughness(0);

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		Light* pointLight = new Light;
		pointLight->SetName("PointLight");
		pointLight->SetType(LT_POINT);
		pointLight->SetPosition(DirectX::XMVectorSet(46,550,80,0),true);
		pointLight->SetColor(YumeColor(1,1,1,0));
		pointLight->SetRange(500);

		Light* pointLight2 = new Light;
		pointLight2->SetName("PointLight");
		pointLight2->SetType(LT_POINT);
		pointLight2->SetPosition(DirectX::XMVectorSet(-700,550,80,0),true);
		pointLight2->SetColor(YumeColor(1,1,1,0));
		pointLight2->SetRange(500);

		Light* pointLight3 = new Light;
		pointLight3->SetName("PointLight");
		pointLight3->SetType(LT_POINT);
		pointLight3->SetPosition(DirectX::XMVectorSet(1140,218,65,0),true);
		pointLight3->SetColor(YumeColor(1,1,1,0));
		pointLight3->SetRange(500);

		Light* pointLight4 = new Light;
		pointLight4->SetName("PointLight");
		pointLight4->SetType(LT_POINT);
		pointLight4->SetPosition(DirectX::XMVectorSet(-800,50,465,0),true);
		pointLight4->SetColor(YumeColor(1,0,0,0));
		pointLight4->SetRange(500);

		Light* pointLight5 = new Light;
		pointLight5->SetName("PointLight");
		pointLight5->SetType(LT_POINT);
		pointLight5->SetPosition(DirectX::XMVectorSet(-350,50,465,0),true);
		pointLight5->SetColor(YumeColor(0,0,1,0));
		pointLight5->SetRange(500);

		Light* pointLight6 = new Light;
		pointLight6->SetName("PointLight");
		pointLight6->SetType(LT_POINT);
		pointLight6->SetPosition(DirectX::XMVectorSet(50,50,465,0),true);
		pointLight6->SetColor(YumeColor(0,1,0,0));
		pointLight6->SetRange(500);

		Light* pointLight7 = new Light;
		pointLight7->SetName("PointLight");
		pointLight7->SetType(LT_POINT);
		pointLight7->SetPosition(DirectX::XMVectorSet(500,50,465,0),true);
		pointLight7->SetColor(YumeColor(1,0,0,0));
		pointLight7->SetRange(500);

		Light* pointLight8 = new Light;
		pointLight8->SetName("PointLight");
		pointLight8->SetType(LT_POINT);
		pointLight8->SetPosition(DirectX::XMVectorSet(950,50,465,0),true);
		pointLight8->SetColor(YumeColor(0,0,1,0));
		pointLight8->SetRange(500);

		scene->AddNode(pointLight);
		scene->AddNode(pointLight2);
		scene->AddNode(pointLight3);
		scene->AddNode(pointLight4);
		scene->AddNode(pointLight5);
		scene->AddNode(pointLight6);
		scene->AddNode(pointLight7);
		scene->AddNode(pointLight8);




		scene->AddNode(dirLight);
	}


	void GodRays::MoveCamera(float timeStep)
	{

	}


	void GodRays::HandleUpdate(float timeStep)
	{

	}
	void GodRays::HandleRenderUpdate(float timeStep)
	{

	}

	void GodRays::Setup()
	{
		engineVariants_["GI"] = NoGI;
		engineVariants_["WindowWidth"] = 1024;
		engineVariants_["WindowHeight"] = 768;
		BaseApplication::Setup();
	}
}

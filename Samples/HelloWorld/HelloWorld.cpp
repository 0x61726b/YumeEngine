//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"
#include "HelloWorld.h"
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

YUME_DEFINE_ENTRY_POINT(YumeEngine::HelloWorld);

namespace YumeEngine
{



	HelloWorld::HelloWorld()
	{
		REGISTER_ENGINE_LISTENER;
	}

	HelloWorld::~HelloWorld()
	{

	}

	void HelloWorld::Start()
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeMiscRenderer* renderer = gYume->pRenderer;
		Scene* scene = renderer->GetScene();
		YumeCamera* camera = renderer->GetCamera();

		gYume->pInput->AddListener(this);

#ifndef DISABLE_CEF
		/*overlay_ = new YumeDebugOverlay;
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);*/

		optionsMenu_ = new YumeOptionsMenu;
		gYume->pUI->AddUIElement(optionsMenu_);
		optionsMenu_->SetVisible(true);
#endif

		StaticModel* dragon= CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(-40,150,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
		StaticModel* dragon2= CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(0,150,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
		StaticModel* dragon3= CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(40,150,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
		StaticModel* dragon4= CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(80,150,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
		StaticModel* dragon5= CreateModel("Models/Mitsuba/mitsuba-sphere.yume",DirectX::XMFLOAT3(120,150,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));

		StaticModel* sponza = CreateModel("Models/sponza/sponza.yume");


		MaterialPtr diff = YumeAPINew Material;
		diff->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		diff->SetShaderParameter("FloorRoughness",0.05f);
		diff->SetShaderParameter("Roughness",0.05f);
		diff->SetShaderParameter("has_diffuse_tex",false);
		diff->SetShaderParameter("has_alpha_tex",false);
		diff->SetShaderParameter("has_specular_tex",false);
		diff->SetShaderParameter("has_normal_tex",false);
		diff->SetShaderParameter("has_roughness_tex",false);
		dragon->SetMaterial(diff);

		MaterialPtr diff2 = YumeAPINew Material;
		diff2->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff2->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(0.8f,0.8f,0.8f,1));
		diff2->SetShaderParameter("FloorRoughness",0.05f);
		diff2->SetShaderParameter("Roughness",0.05f);
		diff2->SetShaderParameter("has_diffuse_tex",false);
		diff2->SetShaderParameter("has_alpha_tex",false);
		diff2->SetShaderParameter("has_specular_tex",false);
		diff2->SetShaderParameter("has_normal_tex",false);
		diff2->SetShaderParameter("has_roughness_tex",false);
		dragon2->SetMaterial(diff2);


		MaterialPtr diff3 = YumeAPINew Material;
		diff3->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff3->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(0.8f,0.8f,0.8f,1));
		diff3->SetShaderParameter("FloorRoughness",0.129f);
		diff3->SetShaderParameter("Roughness",0.129f);
		diff3->SetShaderParameter("has_diffuse_tex",false);
		diff3->SetShaderParameter("has_alpha_tex",false);
		diff3->SetShaderParameter("has_specular_tex",false);
		diff3->SetShaderParameter("has_normal_tex",false);
		diff3->SetShaderParameter("has_roughness_tex",false);
		dragon3->SetMaterial(diff3);

		MaterialPtr diff4 = YumeAPINew Material;
		diff4->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff4->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(0.8f,0.9f,0.8f,1));
		diff4->SetShaderParameter("FloorRoughness",0.3f);
		diff4->SetShaderParameter("Roughness",0.3f);
		diff4->SetShaderParameter("has_diffuse_tex",false);
		diff4->SetShaderParameter("has_alpha_tex",false);
		diff4->SetShaderParameter("has_specular_tex",false);
		diff4->SetShaderParameter("has_normal_tex",false);
		diff4->SetShaderParameter("has_roughness_tex",false);
		dragon4->SetMaterial(diff4);

		MaterialPtr diff5 = YumeAPINew Material;
		diff5->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0.4f,0.4f,0.4f,1));
		diff5->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(0.8,0.8,0.8,1));
		diff5->SetShaderParameter("FloorRoughness",1);
		diff5->SetShaderParameter("Roughness",1);
		diff5->SetShaderParameter("has_diffuse_tex",false);
		diff5->SetShaderParameter("has_alpha_tex",false);
		diff5->SetShaderParameter("has_specular_tex",false);
		diff5->SetShaderParameter("has_normal_tex",false);
		diff5->SetShaderParameter("has_roughness_tex",false);
		dragon5->SetMaterial(diff5);

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(100,2000,200,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);
	}

	void HelloWorld::MoveCamera(float timeStep)
	{
		
	}


	void HelloWorld::HandleUpdate(float timeStep)
	{

	}

	void HelloWorld::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}
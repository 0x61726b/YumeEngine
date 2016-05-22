//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------


#include "Core/YumeHeaders.h"
#include "ModelViewer.h"
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
		overlay_->GetBinding("SampleName")->SetValue("Frustum Culling");
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);


		//SharedPtr<RenderPass> newPass = SharedPtr<RenderPass>(new RenderPass);
		//newPass->Load("RenderCalls/DeferredNoSkybox.xml");
		//newPass->Load("RenderCalls/Bloom.xml",true);
		//newPass->Load("RenderCalls/FXAA.xml",true);
		//newPass->Load("RenderCalls/LensDistortion.xml",true);
		//newPass->Load("RenderCalls/ShowGBuffer.xml",true);
		//newPass->DisableRenderCalls("ShowGBuffer");
		//newPass->DisableRenderCalls("Bloom");
		//renderer->SetDefaultPass(newPass);
#endif

		MaterialPtr emissiveBlue = YumeAPINew Material;
		emissiveBlue->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveBlue->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveBlue->SetShaderParameter("Roughness",1);
		emissiveBlue->SetShaderParameter("ShadingMode",0);
		emissiveBlue->SetShaderParameter("has_diffuse_tex",false);
		emissiveBlue->SetShaderParameter("has_alpha_tex",false);
		emissiveBlue->SetShaderParameter("has_specular_tex",false);
		emissiveBlue->SetShaderParameter("has_normal_tex",false);
		emissiveBlue->SetShaderParameter("has_roughness_tex",false);

		renderer->SetFrustumCulling(true);

		float boxScale = 0.15f;



		YumeVector<YumeString>::type args = GetArguments();


		YumeString modelName = args[args.size() - 3];
		YumeString modelScale = args[args.size() - 2];
		YumeString modelMaterial = args[args.size() - 1];

		if(modelName.length() > 0 && modelName.Contains("Models",true))
		{
			float scale = atof(modelScale.c_str());

			StaticModel* sm_ = new StaticModel(modelName);
			sm_->Initialize();
			sm_->SetName("Cornell");
			sm_->SetScale(scale,scale,scale);
			gYume->pRenderer->UpdateMeshBb(sm_,sm_->GetTransformation());

			DirectX::XMFLOAT3 bbMax = sm_->GetBbMax();
			DirectX::XMFLOAT3 bbMin = sm_->GetBbMin();

			DirectX::XMVECTOR delta = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&bbMax),DirectX::XMLoadFloat3(&bbMin));
			DirectX::XMFLOAT4 d;
			DirectX::XMStoreFloat4(&d,delta);


			DirectX::XMFLOAT3 Pos(0,d.y * 0.1f,0);
			DirectX::XMFLOAT4 rot(0,0,0,0);

			sm_->SetPosition(DirectX::XMLoadFloat3(&Pos),true);
			sm_->SetRotation(DirectX::XMLoadFloat4(&rot));



			gYume->pRenderer->GetScene()->AddNode(sm_);

			if(modelMaterial == "default")
			{
				MaterialPtr defaultMaterial = YumeAPINew Material;
				defaultMaterial->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(0,1,1,1));
				defaultMaterial->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
				defaultMaterial->SetShaderParameter("Roughness",1);
				defaultMaterial->SetShaderParameter("ShadingMode",0);
				defaultMaterial->SetShaderParameter("has_diffuse_tex",false);
				defaultMaterial->SetShaderParameter("has_alpha_tex",false);
				defaultMaterial->SetShaderParameter("has_specular_tex",false);
				defaultMaterial->SetShaderParameter("has_normal_tex",false);
				defaultMaterial->SetShaderParameter("has_roughness_tex",false);

				sm_->SetMaterial(defaultMaterial);
			}
		}

		StaticModel* plane = CreateModel("Models/primitives/highPlane.yume",DirectX::XMFLOAT3(0,0,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(1,1,1));
		plane->SetMaterial(emissiveBlue);

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,1));

		//Light* pointLight4 = new Light;
		//pointLight4->SetName("PointLight");
		//pointLight4->SetType(LT_POINT);
		//pointLight4->SetPosition(DirectX::XMVectorSet(0,10,0,0),true);
		//pointLight4->SetColor(YumeColor(0.7f,0,0.4f,0));
		//pointLight4->SetRange(50);

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
		engineVariants_["GI"] = LPV;
		engineVariants_["WindowWidth"] = 1024;
		engineVariants_["WindowHeight"] = 768;

		BaseApplication::Setup();
	}
}

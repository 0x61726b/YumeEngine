//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"
#include "Playground.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"
#include <boost/shared_ptr.hpp>

#include "Input/YumeInput.h"

#include "Renderer/YumeTexture2D.h"

#include "Renderer/YumeResourceManager.h"

#include "Renderer/YumeRHI.h"



#include "UI/YumeDebugOverlay.h"

#include "Engine/YumeEngine.h"

#include "Core/SharedPtr.h"

#include "UI/YumeUI.h"

#include "Renderer/StaticModel.h"

#include "Renderer/Scene.h"
#include "Renderer/Light.h"




YUME_DEFINE_ENTRY_POINT(YumeEngine::PlaygroundDemo);

#define TURNOFF 1
#define NO_MODEL
#define OBJECTS_CAST_SHADOW
#define NO_SKYBOX
#define NO_PLANE
#define CORNELL
#define DISABLE_EVERYTHING

namespace YumeEngine
{



	PlaygroundDemo::PlaygroundDemo()
		: rot_(Quaternion::IDENTITY)
	{
		REGISTER_ENGINE_LISTENER;
	}

	PlaygroundDemo::~PlaygroundDemo()
	{

	}



	void PlaygroundDemo::Start()
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;

		gYume->pInput->AddListener(this);

#ifndef DISABLE_CEF
		overlay_ = new YumeDebugOverlay;
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);
#endif


		/*StaticModel* sm_ = new StaticModel("Models/Trees/001_Birch_Middle_Age_or_Young.obj");*/
		/*StaticModel* sm_ = new StaticModel("Models/SM/san-miguel.obj");*/
		StaticModel* sm_ = new StaticModel("Models/Primitives/plane.obj");
		sm_->Initialize();
		sm_->SetName("Cornell");

		//StaticModel* sm2_ = new StaticModel("Models/head.obj");
		//sm2_->Initialize();
		//sm2_->SetPosition(DirectX::XMVectorSet(9,10,0,0));
		//sm2_->SetScale(10,10,10);
		//sm2_->SetName("Cornell");
		//gYume->pRenderer->GetScene()->AddNode(sm2_);


		gYume->pRenderer->GetScene()->AddNode(sm_);


		gYume->pRenderer->UpdateMeshBb(*sm_->GetGeometry());

		DirectX::XMFLOAT3 min = gYume->pRenderer->GetMinBb();
		DirectX::XMFLOAT3 max = gYume->pRenderer->GetMaxBb();

		DirectX::XMFLOAT3 delta = DirectX::XMFLOAT3(max.x - min.x,max.y - min.y,max.z - min.z);


		float x = 0;
		float y = 5;

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		/*dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));*/
		dirLight->SetPosition(DirectX::XMVectorSet(0,100,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,1));

		/*gYume->pRenderer->GetScene()->AddNode(pointLight);*/

		gYume->pRenderer->GetScene()->AddNode(dirLight);

	}

	void PlaygroundDemo::CreateLight(DirectX::XMFLOAT3 pos,YumeColor color,float range)
	{
		Light* pointLight = new Light;
		pointLight->SetName("PointLight");
		pointLight->SetType(LT_POINT);
		pointLight->SetPosition(DirectX::XMVectorSet(pos.x,pos.y,pos.z,0));
		pointLight->SetColor(color);
		pointLight->SetRange(range);

		gYume->pRenderer->GetScene()->AddNode(pointLight);
	}

	void PlaygroundDemo::SSAOOffsetVectors()
	{

	}

	void PlaygroundDemo::CreateCube(Vector3 Pos,Quaternion Rot,Vector3 size,YumeColor color)
	{

	}

	void PlaygroundDemo::CreateModel(Vector3 Pos,Quaternion Rot)
	{

	}

	void PlaygroundDemo::CreateCube(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{

	}
	void PlaygroundDemo::CreateSphere(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{

	}
	void PlaygroundDemo::CreateCylinder(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{

	}
	void PlaygroundDemo::CreatePyramid(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{

	}
	void PlaygroundDemo::CreateCone(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
	}
	void PlaygroundDemo::MoveCamera(float timeStep)
	{
	}


	void PlaygroundDemo::HandleUpdate(float timeStep)
	{

	}
	void PlaygroundDemo::HandleRenderUpdate(float timeStep)
	{

	}

	void PlaygroundDemo::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}

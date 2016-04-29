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


		StaticModel* sm_ = new StaticModel("Models/Cornell/cornellbox.obj");
		sm_->Initialize();
		sm_->SetName("Cornell");

		gYume->pRenderer->GetScene()->AddNode(sm_);

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));
		/*dirLight->SetPosition(DirectX::XMVectorSet(60,2200,90,0));*/
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(0.1,0.1,0.1,0.1));


		gYume->pRenderer->GetScene()->AddNode(dirLight);
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

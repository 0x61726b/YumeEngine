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
#include "Scene/YumeOctree.h"
#include "Renderer/YumeCamera.h"
#include <boost/shared_ptr.hpp>

#include "Input/YumeInput.h"
#include "Renderer/YumeViewport.h"

#include "Renderer/YumeRenderer.h"
#include "Renderer/YumeDrawable.h"
#include "Renderer/YumeRenderView.h"
#include "Renderer/YumeMaterial.h"
#include "Renderer/YumeAuxRenderer.h"
#include "Renderer/YumeSkybox.h"
#include "Renderer/YumeRendererEnv.h"
#include "Renderer/YumeTexture2D.h"
#include "Renderer/YumeRenderPass.h"

#include "Renderer/YumeResourceManager.h"

#include "Renderer/YumeRHI.h"

#include "Renderer/YumeModel.h"
#include "Renderer/YumeStaticModel.h"

#include "UI/YumeDebugOverlay.h"

#include "Engine/YumeEngine.h"

#include "Core/SharedPtr.h"

#include "UI/YumeUI.h"

YUME_DEFINE_ENTRY_POINT(YumeEngine::HelloWorld);

namespace YumeEngine
{



	HelloWorld::HelloWorld()
		: rot_(Quaternion::IDENTITY)
	{
		REGISTER_ENGINE_LISTENER;
	}

	HelloWorld::~HelloWorld()
	{

	}

	void HelloWorld::Start()
	{
		BaseApplication::Start();

		overlay_->GetBinding("SampleName")->SetValue("HelloWorld");
		gYume->pUI->SetUIEnabled(false);

		YumeResourceManager* rm_ = gYume->pResourceManager;

		scene_ = SharedPtr<YumeScene>(new YumeScene);
		scene_->SetName("Scene");

		scene_->CreateComponent<Octree>();

		YumeSceneNode* lightNode = scene_->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(0.6f,-1.0f,0.8f)); // The direction vector does not need to be normalized
		YumeLight* light = lightNode->CreateComponent<YumeLight>();
		light->SetLightType(LIGHT_DIRECTIONAL);


		cubeNode_ = scene_->CreateChild("Cube");
		cubeNode_->SetPosition(Vector3(0,0,0));
		cubeNode_->SetRotation(Quaternion::IDENTITY);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		drawable->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Yume.xml"));
		drawable->SetCastShadows(true);

		cameraNode_ = scene_->CreateChild("Camera");
		YumeCamera* camera = cameraNode_->CreateComponent<YumeCamera>();
		cameraNode_->SetPosition(Vector3(2,1,-1));
		
		Quaternion q;
		q.FromLookRotation( (cameraNode_->GetWorldPosition() * -1 ).Normalized() );
		cameraNode_->SetRotation(q);
		camera->SetFarClip(300.0f);



		SharedPtr<YumeViewport> viewport(new YumeViewport(scene_,cameraNode_->GetComponent<YumeCamera>()));

		YumeRenderer* renderer = gYume->pRenderer;
		renderer->SetViewport(0,viewport);
	}

	void HelloWorld::MoveCamera(float timeStep)
	{
		float MOVE_SPEED = 5.0f;
		const float MOUSE_SENSITIVITY = 0.1f;

		YumeInput* input = gYume->pInput;

		if(!input->HasFocus())
			return;

		//IntVector2 mouseMove = input->GetMouseMove();
		//yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		//pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		//pitch_ = Clamp(pitch_,-90.0f,90.0f);


		//cameraNode_->SetRotation(Quaternion(pitch_,yaw_,0.0f));

		//if(input->GetKeyDown(KEY_SHIFT))
		//	MOVE_SPEED = 35.0f;
		//if(input->GetKeyDown('W'))
		//	cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		//if(input->GetKeyDown('S'))
		//	cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		//if(input->GetKeyDown('A'))
		//	cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		//if(input->GetKeyDown('D'))
		//	cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
	}


	void HelloWorld::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);

		Quaternion thisFrame(45*timeStep,Vector3(0,1,0));

		cubeNode_->Rotate(thisFrame);
	}

	void HelloWorld::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}
//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"
#include "BasicRenderWindow.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"
#include "Scene/YumeOctree.h"
#include "Renderer/YumeCamera.h"
#include <boost/shared_ptr.hpp>

#include "Input/YumeInput.h"
#include "Renderer/YumeViewport.h"

#include "Renderer/YumeRenderer.h"

#include "Engine/YumeEngine.h"
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
		BaseApplication::Start();

		scene_ = SharedPtr<YumeScene>(new YumeScene);
		scene_->SetName("Scene");

		Octree* octree_ = scene_->CreateComponent<Octree>();


		cameraNode_ = scene_->CreateChild("Camera");
		cameraNode_->CreateComponent<YumeCamera>();
		cameraNode_->SetPosition(Vector3(0,0,-5));

		SharedPtr<YumeViewport> viewport = SharedPtr<YumeViewport>(new YumeViewport(scene_.get(),cameraNode_->GetComponent<YumeCamera>()));

		YumeRenderer* renderer = YumeEngine3D::Get()->GetRenderLogic();
		renderer->SetViewport(0,viewport);

	}


	void HelloWorld::MoveCamera(float timeStep)
	{
		float MOVE_SPEED = 5.0f;
		const float MOUSE_SENSITIVITY = 0.1f;

		SharedPtr<YumeInput> input = YumeEngine3D::Get()->GetInput();

		IntVector2 mouseMove = input->GetMouseMove();
		yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		pitch_ = Clamp(pitch_,-90.0f,90.0f);

		
		cameraNode_->SetRotation(Quaternion(pitch_,yaw_,0.0f));

		if(input->GetKeyDown(KEY_SHIFT))
			MOVE_SPEED = 15.0f;
		if(input->GetKeyDown('W'))
			cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('S'))
			cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('A'))
			cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('D'))
			cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

		
	}


	void HelloWorld::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);
	}

	void HelloWorld::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
	}
}
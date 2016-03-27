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

#include "Renderer/YumeResourceManager.h"

#include "Renderer/YumeModel.h"
#include "Renderer/YumeStaticModel.h"

#include "Engine/YumeEngine.h"

#include "UI/YumeDebugOverlay.h"


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
		BaseApplication::Start();

		overlay_->GetBinding("SampleName")->SetValue("Lighting");
		gYume->pUI->SetUIEnabled(false);

		YumeResourceManager* rm_ = gYume->pResourceManager;

		scene_ = SharedPtr<YumeScene>(new YumeScene);
		scene_->SetName("Scene");

		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<YumeDebugRenderer>();


		CreateLight(Vector3(-3,2.8f,0),YumeColor(1,0,0));
		CreateLight(Vector3(0,2.8f,0),YumeColor(0,1,0));
		CreateLight(Vector3(3,2.8f,0),YumeColor(0,0,1));



		YumeSceneNode* plane = scene_->CreateChild("Plane");
		plane->SetPosition(Vector3(0,0,0));
		plane->SetRotation(Quaternion::IDENTITY);
		plane->SetScale(Vector3(100,1,100));

		YumeStaticModel* planeModel = plane->CreateComponent<YumeStaticModel>();
		planeModel->SetModel(rm_->PrepareResource<YumeModel>("Models/Plane.mdl"));
		planeModel->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/StoneTiled.xml"));


		YumeSceneNode* test = scene_->CreateChild("Test");
		test->SetPosition(Vector3(1.5f,0.5f,0));
		test->SetRotation(Quaternion::IDENTITY);
		YumeStaticModel* drawable = test->CreateComponent<YumeStaticModel>();
		drawable->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		drawable->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Yume.xml"));
		drawable->SetCastShadows(true);

		YumeSceneNode* test2 = scene_->CreateChild("Test");
		test2->SetPosition(Vector3(-2.0f,0.5f,0));
		test2->SetRotation(Quaternion(45,Vector3(0,1,0)));
		YumeStaticModel* drawable2 = test2->CreateComponent<YumeStaticModel>();
		drawable2->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		drawable2->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Yume.xml"));
		drawable2->SetCastShadows(true);


		YumeSceneNode* test3 = scene_->CreateChild("Test");
		test3->SetPosition(Vector3(0,5,7));
		test3->SetRotation(Quaternion::IDENTITY);
		test3->SetScale(10);
		YumeStaticModel* large = test3->CreateComponent<YumeStaticModel>();
		large->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		large->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Yume.xml"));
		large->SetCastShadows(true);
		large->SetOccluder(true);

		//YumeSceneNode* skyNode = scene_->CreateChild("Sky");
		//skyNode->SetScale(500.0f); // The scale actually does not matter
		//YumeSkybox* skybox = skyNode->CreateComponent<YumeSkybox>();
		//skybox->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		//skybox->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Skybox.xml"));

		cameraNode_ = scene_->CreateChild("Camera");
		YumeCamera* camera = cameraNode_->CreateComponent<YumeCamera>();
		cameraNode_->SetPosition(Vector3(0.0f,1.0f,-5.0f));
		camera->SetFarClip(300.0f);



		SharedPtr<YumeViewport> viewport(new YumeViewport(scene_,cameraNode_->GetComponent<YumeCamera>()));

		YumeRenderer* renderer = gYume->pRenderer;
		renderer->SetViewport(0,viewport);


	}

	void LightingDemo::CreateLight(Vector3 pos,YumeColor color)
	{
		YumeSceneNode* pnode = scene_->CreateChild("plight");
		pnode->SetPosition(pos);
		YumeLight* plight = pnode->CreateComponent<YumeLight>();
		plight->SetLightType(LIGHT_POINT);
		plight->SetRange(5);
		plight->SetFov(60);
		plight->SetColor(color);
		plight->SetCastShadows(true);

	}


	void LightingDemo::MoveCamera(float timeStep)
	{
		float MOVE_SPEED = 5.0f;
		const float MOUSE_SENSITIVITY = 0.1f;

		YumeInput* input = gYume->pInput;

		if(!input->HasFocus())
			return;

		IntVector2 mouseMove = input->GetMouseMove();
		yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		pitch_ = Clamp(pitch_,-90.0f,90.0f);


		cameraNode_->SetRotation(Quaternion(pitch_,yaw_,0.0f));

		if(input->GetKeyDown(KEY_SHIFT))
			MOVE_SPEED = 35.0f;
		if(input->GetKeyDown('W'))
			cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('S'))
			cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('A'))
			cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		if(input->GetKeyDown('D'))
			cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

	}


	void LightingDemo::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);
	}

	void LightingDemo::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}
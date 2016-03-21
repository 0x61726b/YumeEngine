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

#include "Engine/YumeEngine.h"

#include "Core/SharedPtr.h"
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

		YumeResourceManager* rm_ = gYume->pResourceManager;

		scene_ = SharedPtr<YumeScene>(new YumeScene);
		scene_->SetName("Scene");

		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<YumeDebugRenderer>();

		YumeSceneNode* zoneNode = scene_->CreateChild("Zone");
		YumeRendererEnvironment* zone = zoneNode->CreateComponent<YumeRendererEnvironment>();
		zone->SetBoundingBox(BoundingBox(-1000.0f,1000.0f));
		zone->SetAmbientColor(YumeColor(0.15f,0.15f,0.15f));
		zone->SetFogColor(YumeColor(0,0,0));
		zone->SetFogStart(100.0f);
		zone->SetFogEnd(300.0f);

		YumeSceneNode* lightNode = scene_->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(0.6f,-1.0f,0.8f)); // The direction vector does not need to be normalized
		YumeLight* light = lightNode->CreateComponent<YumeLight>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetCastShadows(true);
		light->SetShadowBias(BiasParameters(0.00025f,0.5f));
		light->SetShadowCascade(CascadeParameters(10.0f,50.0f,200.0f,0.0f,0.8f));


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
		drawable->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Stone.xml"));
		drawable->SetCastShadows(true);

		//YumeSceneNode* test2 = scene_->CreateChild("Test");
		//test2->SetPosition(Vector3(-2.0f,0.5f,0));
		//test2->SetRotation(Quaternion(45,Vector3(0,1,0)));
		//YumeStaticModel* drawable2 = test2->CreateComponent<YumeStaticModel>();
		//drawable2->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		//drawable2->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Yume.xml"));
		//drawable2->SetCastShadows(true);


	/*	YumeSceneNode* test3 = scene_->CreateChild("Test");
		test3->SetPosition(Vector3(0,5,7));
		test3->SetRotation(Quaternion::IDENTITY);
		test3->SetScale(10);
		YumeStaticModel* large = test3->CreateComponent<YumeStaticModel>();
		large->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		large->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Stone.xml"));
		large->SetCastShadows(true);
		large->SetOccluder(true);*/

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
		/*renderer->SetNumViewports(2);*/
		renderer->SetViewport(0,viewport);

		//SharedPtr<YumeViewport> rearViewport(new YumeViewport(scene_,cameraNode_->GetComponent<YumeCamera>(),
		//	IntRect(gYume->pRHI->GetWidth() * 2 / 3,32,gYume->pRHI->GetWidth() - 32,gYume->pRHI->GetHeight() / 3)));
		//renderer->SetViewport(1,rearViewport);

		//camera->SetViewOverrideFlags(VO_DISABLE_OCCLUSION);
	}

	void HelloWorld::MoveCamera(float timeStep)
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


	void HelloWorld::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);
	}

	void HelloWorld::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}
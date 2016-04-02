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

#include "Renderer/YumeRenderPipeline.h"

YUME_DEFINE_ENTRY_POINT(YumeEngine::PlaygroundDemo);

#define TURNOFF 1
#define OBJECTS_CAST_SHADOW
//#define NO_SKYBOX
//#define NO_PLANE

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

		scene_ = SharedPtr<YumeScene>(new YumeScene);
		scene_->SetName("Scene");

		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<YumeDebugRenderer>();

		YumeSceneNode* lightNode = scene_->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(0.6f,-1.0f,0.8f)); // The direction vector does not need to be normalized
		YumeLight* light = lightNode->CreateComponent<YumeLight>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetShadowBias(BiasParameters(0.00025f,0.5f));
		light->SetCastShadows(true);


#ifndef NO_PLANE
		YumeSceneNode* plane = scene_->CreateChild("Cube");
		plane->SetPosition(Vector3(0,0,0));
		plane->SetRotation(Quaternion::IDENTITY);
		plane->SetScale(Vector3(50,1,50));
		YumeStaticModel* drawable = plane->CreateComponent<YumeStaticModel>();
		drawable->SetModel(rm_->PrepareResource<YumeModel>("Models/Plane.mdl"));
		YumeMaterial* planeMat = rm_->PrepareResource<YumeMaterial>("Materials/StoneTiled.xml");
		drawable->SetMaterial(planeMat);
#endif

		/*CreateModel(Vector3(0,1.5f,0),Quaternion::IDENTITY);*/

#ifndef NO_SKYBOX
		YumeSceneNode* skyNode = scene_->CreateChild("Sky");
		skyNode->SetScale(500.0f); // The scale actually does not matter
		YumeSkybox* skybox = skyNode->CreateComponent<YumeSkybox>();
		skybox->SetModel(rm_->PrepareResource<YumeModel>("Models/Sphere.mdl"));
		skybox->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Skydome.xml"));
#endif

#if TURNOFF == 1
		CreateCube(Vector3(-3,1,0),
			Quaternion(0,Vector3(0,1,0)),2.5f,YumeColor(1,0,0,0));

		CreateCube(Vector3(0,0.3f,0),
			Quaternion::IDENTITY,2,YumeColor(0,1,0,0));
		CreateCube(Vector3(2,0.5f,0),
			Quaternion::IDENTITY,1,YumeColor(0,0,1,0));

		CreateSphere(Vector3(-8,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(1,0,0,0));

		CreateSphere(Vector3(-6,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(0,1,0,0));

		CreateSphere(Vector3(-4,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(0,0,1,0));

		CreatePyramid(Vector3(4,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(1,0,0,0));

		CreatePyramid(Vector3(6,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(0,1,0,0));

		CreatePyramid(Vector3(8,0.5f,9),
			Quaternion::IDENTITY,1,YumeColor(0,0,1,0));

		CreateCylinder(Vector3(4,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(1,0,0,0));

		CreateCylinder(Vector3(6,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(0,1,0,0));

		CreateCylinder(Vector3(8,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(0,0,1,0));

		CreateCone(Vector3(-8,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(1,0,0,0));

		CreateCone(Vector3(-6,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(0,1,0,0));

		CreateCone(Vector3(-4,0.5f,6),
			Quaternion::IDENTITY,1,YumeColor(0,0,1,0));
#endif

		cameraNode_ = scene_->CreateChild("Camera");
		YumeCamera* camera = cameraNode_->CreateComponent<YumeCamera>();
		cameraNode_->SetPosition(Vector3(0,5,-10));

		Quaternion q;
		q.FromLookRotation((cameraNode_->GetWorldPosition() * -1).Normalized());
		cameraNode_->SetRotation(q);
		camera->SetFarClip(1000.0f);
		camera->SetFov(45);

		BaseApplication::Start();

		overlay_->GetBinding("SampleName")->SetValue("Playground");
		gYume->pUI->SetUIEnabled(false);
	}

	void PlaygroundDemo::SSAOOffsetVectors()
	{

	}

	void PlaygroundDemo::CreateModel(Vector3 Pos,Quaternion Rot)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Cube");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(0.1f);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/cryteksponza.yume"));
		drawable->ApplyMaterialList("Models/cryteksponza.txt");
#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}

	void PlaygroundDemo::CreateCube(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Cube");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(size);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Box.mdl"));
		SharedPtr<YumeMaterial> mat = gYume->pResourceManager->PrepareResource<YumeMaterial>("Materials/DefaultGrey.xml")->Clone();
		mat->SetShaderParameter("MatDiffColor",color);
		drawable->SetMaterial(mat);
#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}
	void PlaygroundDemo::CreateSphere(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Sphere");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(size);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Sphere.mdl"));
		SharedPtr<YumeMaterial> mat = gYume->pResourceManager->PrepareResource<YumeMaterial>("Materials/DefaultGrey.xml")->Clone();
		mat->SetShaderParameter("MatDiffColor",color);
		drawable->SetMaterial(mat);
#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}
	void PlaygroundDemo::CreateCylinder(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Sphere");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(size);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Cylinder.mdl"));
		SharedPtr<YumeMaterial> mat = gYume->pResourceManager->PrepareResource<YumeMaterial>("Materials/DefaultGrey.xml")->Clone();
		mat->SetShaderParameter("MatDiffColor",color);
		drawable->SetMaterial(mat);
#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}
	void PlaygroundDemo::CreatePyramid(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Sphere");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(size);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Pyramid.mdl"));
		SharedPtr<YumeMaterial> mat = gYume->pResourceManager->PrepareResource<YumeMaterial>("Materials/DefaultGrey.xml")->Clone();
		mat->SetShaderParameter("MatDiffColor",color);
		drawable->SetMaterial(mat);
#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}
	void PlaygroundDemo::CreateCone(Vector3 Pos,Quaternion Rot,float size,YumeColor color)
	{
		YumeSceneNode* cubeNode_ = scene_->CreateChild("Sphere");
		cubeNode_->SetPosition(Pos);
		cubeNode_->SetRotation(Rot);
		cubeNode_->SetScale(size);
		YumeStaticModel* drawable = cubeNode_->CreateComponent<YumeStaticModel>();
		drawable->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Cone.mdl"));
		SharedPtr<YumeMaterial> mat = gYume->pResourceManager->PrepareResource<YumeMaterial>("Materials/DefaultGrey.xml")->Clone();
		mat->SetShaderParameter("MatDiffColor",color);
		drawable->SetMaterial(mat);

#ifdef OBJECTS_CAST_SHADOW
		drawable->SetCastShadows(true);
#endif
	}
	void PlaygroundDemo::MoveCamera(float timeStep)
	{
		float MOVE_SPEED = 5.0f;
		const float MOUSE_SENSITIVITY = 0.1f;

		YumeInput* input = gYume->pInput;

		if(!input->HasFocus())
			return;

		if(input->GetMouseButtonDown(MOUSEB_RIGHT))
		{
			IntVector2 mouseMove = input->GetMouseMove();
			yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
			pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
			pitch_ = Clamp(pitch_,-90.0f,90.0f);


			cameraNode_->SetRotation(Quaternion(pitch_,yaw_,0.0f));
		}

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


	void PlaygroundDemo::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);
	}
	void PlaygroundDemo::HandleRenderUpdate(float timeStep)
	{
		YumeViewport* viewport = gYume->pRenderer->GetViewport(0);
		YumeRenderPipeline* fx = viewport->GetRenderPath();
		YumeCamera* cam = viewport->GetCamera();

	}

	void PlaygroundDemo::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;
	}
}

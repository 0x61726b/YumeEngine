//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "Core/YumeHeaders.h"
#include "CefTest.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"
#include "Scene/YumeOctree.h"
#include "Renderer/YumeCamera.h"

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

#include "UI/YumeUI.h"



YUME_DEFINE_ENTRY_POINT(YumeEngine::CefTest);

int LogicalToDevice(int value,float device_scale_factor) {
	float scaled_val = static_cast<float>(value)* device_scale_factor;
	return static_cast<int>(std::floor(scaled_val));
}

namespace YumeEngine
{
	CefTest::CefTest()
	{
		REGISTER_ENGINE_LISTENER;
	}

	CefTest::~CefTest()
	{

	}

	void CefTest::HandleBeginFrame(int frameNumber)
	{

	}

	void CefTest::Start()
	{
		BaseApplication::Start();

		gYume->pTimer->AddTimeEventListener(this);

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
		lightNode->SetDirection(Vector3(0.6f,-1.0f,0.8f));
		YumeLight* light = lightNode->CreateComponent<YumeLight>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetCastShadows(true);
		light->SetShadowBias(BiasParameters(0.00025f,0.5f));
		light->SetShadowCascade(CascadeParameters(10.0f,50.0f,200.0f,0.0f,0.8f));


		cameraNode_ = scene_->CreateChild("Camera");
		cameraNode_->SetRotation(Quaternion(90.0f,0.0f,0.0f));

		YumeCamera* camera = cameraNode_->CreateComponent<YumeCamera>();
		cameraNode_->SetPosition(Vector3(0.0f,5.0f,-10.0f));
		camera->SetFarClip(300.0f);


		//Create browsers
		
		CefUI::CefRect browser1(0,0,1024,768);
		int browser1Index = gYume->pUI->CreateBrowser(browser1,"http://www.google.co.uk");
		

		CefUI::CefRect browser2 = CefUI::CefRect(0,0,1024,768);
		int browser2Index = gYume->pUI->CreateBrowser(browser2,"http://www.reddit.com/r/dota2");


		CefUI::CefRect browser3 = CefUI::CefRect(0,0,1024,768);
		int browser3Index = gYume->pUI->CreateBrowser(browser2,"http://www.chiika.moe");

		//
		YumeSceneNode* screenNode = scene_->CreateChild("Screen");
		screenNode->SetPosition(Vector3(0.0f,10.0f,-0.27f));
		screenNode->SetRotation(Quaternion(-90.0f,0.0f,0.0f));
		screenNode->SetScale(Vector3(20.0f,0.0f,15.0f));
		YumeStaticModel* screenObject = screenNode->CreateComponent<YumeStaticModel>();
		screenObject->SetModel(rm_->PrepareResource<YumeModel>("Models/Plane.mdl"));


		SharedPtr<YumeMaterial> renderMaterial(new YumeMaterial);
		renderMaterial->SetTechnique(0,rm_->PrepareResource<YumeRenderTechnique>("Techniques/DiffUnlit.xml"));
		renderMaterial->SetTexture(TU_DIFFUSE,gYume->pUI->GetBrowserImage(browser1Index,false).second);
		screenObject->SetMaterial(renderMaterial);


		//
		YumeSceneNode* screenNodeRight = scene_->CreateChild("Screen");
		screenNodeRight->SetPosition(Vector3(17.0f,10.0f,-7.0f));
		screenNodeRight->SetRotation(Quaternion(-90.0f,0.0f,45.0f));
		screenNodeRight->SetScale(Vector3(20.0f,0.0f,15.0f));
		YumeStaticModel* screenObjectRight = screenNodeRight->CreateComponent<YumeStaticModel>();
		screenObjectRight->SetModel(rm_->PrepareResource<YumeModel>("Models/Plane.mdl"));

		SharedPtr<YumeMaterial> renderMaterialRight(new YumeMaterial);
		renderMaterialRight->SetTechnique(0,rm_->PrepareResource<YumeRenderTechnique>("Techniques/DiffUnlit.xml"));
		renderMaterialRight->SetTexture(TU_DIFFUSE,gYume->pUI->GetBrowserImage(browser2Index,false).second);
		screenObjectRight->SetMaterial(renderMaterialRight);
		//

		//
		YumeSceneNode* screenNodeLeft = scene_->CreateChild("Screen");
		screenNodeLeft->SetPosition(Vector3(-17.0f,10.0f,-7.0f));
		screenNodeLeft->SetRotation(Quaternion(-90.0f,0.0f,-45.0f));
		screenNodeLeft->SetScale(Vector3(20.0f,0.0f,15.0f));
		YumeStaticModel* screenObjectLeft = screenNodeLeft->CreateComponent<YumeStaticModel>();
		screenObjectLeft->SetModel(rm_->PrepareResource<YumeModel>("Models/Plane.mdl"));

		SharedPtr<YumeMaterial> renderMaterialLeft(new YumeMaterial);
		renderMaterialLeft->SetTechnique(0,rm_->PrepareResource<YumeRenderTechnique>("Techniques/DiffUnlit.xml"));
		renderMaterialLeft->SetTexture(TU_DIFFUSE,gYume->pUI->GetBrowserImage(browser3Index,false).second);
		screenObjectLeft->SetMaterial(renderMaterialLeft);
		//


		//YumeSceneNode* skyNode = scene_->CreateChild("Sky");
		//skyNode->SetScale(500.0f); // The scale actually does not matter
		//YumeSkybox* skybox = skyNode->CreateComponent<YumeSkybox>();
		//skybox->SetModel(rm_->PrepareResource<YumeModel>("Models/Box.mdl"));
		//skybox->SetMaterial(rm_->PrepareResource<YumeMaterial>("Materials/Skybox.xml"));


		SharedPtr<YumeViewport> viewport(new YumeViewport(scene_,cameraNode_->GetComponent<YumeCamera>()));

		YumeRenderer* renderer = gYume->pRenderer;
		renderer->SetViewport(0,viewport);


	}

	void CefTest::OnBrowserReady(unsigned int browserIndex)
	{

	}

	void CefTest::MoveCamera(float timeStep)
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


	void CefTest::HandleUpdate(float timeStep)
	{
		MoveCamera(timeStep);


	}



	void CefTest::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
		engineVariants_["WindowHeight"] = 900;

	}
}

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



YUME_DEFINE_ENTRY_POINT(YumeEngine::CefTest);


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
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);
#endif

		MaterialPtr leftBrowserMat = YumeAPINew Material;
		leftBrowserMat->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,1,1,1));
		leftBrowserMat->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		leftBrowserMat->SetShaderParameter("Roughness",1.0f);
		leftBrowserMat->SetShaderParameter("ShadingMode",4.0f);
		leftBrowserMat->SetShaderParameter("has_diffuse_tex",true);
		leftBrowserMat->SetShaderParameter("has_alpha_tex",false);
		leftBrowserMat->SetShaderParameter("has_specular_tex",false);
		leftBrowserMat->SetShaderParameter("has_normal_tex",false);
		leftBrowserMat->SetShaderParameter("has_roughness_tex",false);

		MaterialPtr rightBrowserMat = YumeAPINew Material;
		rightBrowserMat->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,1,1,1));
		rightBrowserMat->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		rightBrowserMat->SetShaderParameter("Roughness",1.0f);
		rightBrowserMat->SetShaderParameter("ShadingMode",4.0f);
		rightBrowserMat->SetShaderParameter("has_diffuse_tex",true);
		rightBrowserMat->SetShaderParameter("has_alpha_tex",false);
		rightBrowserMat->SetShaderParameter("has_specular_tex",false);
		rightBrowserMat->SetShaderParameter("has_normal_tex",false);
		rightBrowserMat->SetShaderParameter("has_roughness_tex",false);

		MaterialPtr middleBrowserMat = YumeAPINew Material;
		middleBrowserMat->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,0.0784314f,0.576471f,1));
		middleBrowserMat->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		middleBrowserMat->SetShaderParameter("Roughness",1.0f);
		middleBrowserMat->SetShaderParameter("ShadingMode",4.0f);
		middleBrowserMat->SetShaderParameter("has_diffuse_tex",true);
		middleBrowserMat->SetShaderParameter("has_alpha_tex",false);
		middleBrowserMat->SetShaderParameter("has_specular_tex",false);
		middleBrowserMat->SetShaderParameter("has_normal_tex",false);
		middleBrowserMat->SetShaderParameter("has_roughness_tex",false);

		
		float planeScale = 0.3f;

		StaticModel* browserRightModel = CreateModel("Models/Primitives/cefplane.yume");
		browserRightModel->SetPosition(DirectX::XMVectorSet(11,7,-2,1));
		browserRightModel->SetRotation(DirectX::XMVectorSet(0,M_DEGTORAD * 30,-M_DEGTORAD * 90,0));
		browserRightModel->SetScale(planeScale,planeScale,planeScale);
		browserRightModel->SetMaterial(rightBrowserMat);

		StaticModel* browserMiddleModel = CreateModel("Models/Primitives/cefplane.yume");
		browserMiddleModel->SetPosition(DirectX::XMVectorSet(0,7,1,1));
		browserMiddleModel->SetRotation(DirectX::XMVectorSet(0,0,-M_DEGTORAD * 90,0));
		browserMiddleModel->SetScale(planeScale,planeScale,planeScale);
		browserMiddleModel->SetMaterial(middleBrowserMat);

		StaticModel* browserLeftModel = CreateModel("Models/Primitives/cefplane.yume");
		browserLeftModel->SetPosition(DirectX::XMVectorSet(-11,7,-2,1));
		browserLeftModel->SetRotation(DirectX::XMVectorSet(0,-M_DEGTORAD * 30,-M_DEGTORAD * 90,0));
		browserLeftModel->SetScale(planeScale,planeScale,planeScale);
		browserLeftModel->SetMaterial(leftBrowserMat);

		YumeUI* pUI = gYume->pUI;

		YumeUIElement* rightBrowser = new YumeUIElement(1280,720,Element,"RightBrowser");
		rightBrowser->SetPosition(0,0);
		rightBrowser->SetWidth(1280);
		rightBrowser->SetHeight(900);
		rightBrowser->SetURL("http://arkenthera.github.io/");
		pUI->AddUIElement(rightBrowser);

		YumeUIElement* leftBrowser = new YumeUIElement(1280,720,Element,"LeftBrowser");
		leftBrowser->SetPosition(0,0);
		leftBrowser->SetWidth(1280);
		leftBrowser->SetHeight(720);
		leftBrowser->SetURL("https://youtu.be/bbrnPtybNd4?t=10m20s");
		pUI->AddUIElement(leftBrowser);

		YumeUIElement* middleBrowser = new YumeUIElement(1280,720,Element,"MiddleBrowser");
		middleBrowser->SetPosition(0,0);
		middleBrowser->SetWidth(1280);
		middleBrowser->SetHeight(720);
		middleBrowser->SetURL("https://www.taleworlds.com/");
		pUI->AddUIElement(middleBrowser);

		rightBrowserMat->SetTexture(MT_DIFFUSE,rightBrowser->GetTexture());
		leftBrowserMat->SetTexture(MT_DIFFUSE,leftBrowser->GetTexture());
		middleBrowserMat->SetTexture(MT_DIFFUSE,middleBrowser->GetTexture());
		
		StaticModel* plane = CreateModel("Models/Primitives/HighPlane.yume");
		plane->SetFloorRoughness(0);
		

		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,35,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);

	}

	void CefTest::OnBrowserReady(unsigned int browserIndex)
	{

	}

	void CefTest::MoveCamera(float timeStep)
	{
		
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

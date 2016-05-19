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
#include "YumeHeaders.h"
#include "YumeOptionsMenu.h"

#include "Core/YumeDefaults.h"
#include "Renderer/RenderPass.h"
#include "Renderer/YumeRHI.h"

#include "Renderer/Light.h"
#include "Renderer/Scene.h"

namespace YumeEngine
{
	YumeOptionsMenu::YumeOptionsMenu()
		: YumeUIElement(1600,900,"OptionsMenu"),
		lightWarmth(0),
		lightFlux(0)
	{


		//width = rt - (L + R)
		//800 = 1600 - (L+R)
		//800 = L+R
		SetPosition(0,0);
		SetWidth(1600);
		SetHeight(900);

		Initialize();
	}

	YumeOptionsMenu::~YumeOptionsMenu()
	{
	}

	bool YumeOptionsMenu::Initialize()
	{
		FsPath binaryRoot = gYume->pIO->GetBinaryRoot();
		std::string path("file:///");
		path.append(binaryRoot.generic_string());
		path.append("/Engine/Assets/UI/Options/Options.html");

		//path = "http://i.imgur.com/NWBYoP6.gif"; //color test

		SetURL(path.c_str());

		gYume->pUI->AddDOMListener(this);


		return true;

	}

	void YumeOptionsMenu::Update()
	{

	}

	void YumeOptionsMenu::OnContextReady()
	{
		RenderPass* dp = gYume->pRenderer->GetDefaultPass();

		float lpvflux = dp->GetShaderParameter("lpv_flux_amplifier").Get<float>();
		float giScale = dp->GetShaderParameter("gi_scale").Get<float>();


		gYume->pUI->SetCppProperty(0,"lpvFluxValue",std::to_string(lpvflux).c_str());
		gYume->pUI->SetCppProperty(0,"lpvScaleValue",std::to_string(giScale).c_str());
		gYume->pUI->SetCppProperty(0,"vctScaleValue",std::to_string(giScale).c_str());


		float exposure_speed = dp->GetShaderParameter("exposure_speed").Get<float>();
		float exposure_key = dp->GetShaderParameter("exposure_key").Get<float>();
		float bloom_treshold = dp->GetShaderParameter("bloom_treshold").Get<float>();
		float bloom_sigma = dp->GetShaderParameter("bloom_sigma").Get<float>();

		float godrays_tau = dp->GetShaderParameter("godrays_tau").Get<float>();
		float MistSpeed = dp->GetShaderParameter("MistSpeed").Get<float>();
		float NumSamples = dp->GetShaderParameter("NumSamples").Get<float>();
		float Dithering = dp->GetShaderParameter("Dithering").Get<float>();

		gYume->pUI->SetCppProperty(0,"GodraysTauValue",std::to_string(godrays_tau).c_str());
		gYume->pUI->SetCppProperty(0,"GodraysMistSpeedValue",std::to_string(MistSpeed).c_str());
		gYume->pUI->SetCppProperty(0,"GodraysNumSamplesValue",std::to_string(NumSamples).c_str());
		gYume->pUI->SetCppProperty(0,"GodraysDitheringValue",std::to_string(Dithering).c_str());

		gYume->pUI->SetCppProperty(0,"exposureSpeedValue",std::to_string(exposure_speed).c_str());
		gYume->pUI->SetCppProperty(0,"exposureKeyValue",std::to_string(exposure_key).c_str());
		gYume->pUI->SetCppProperty(0,"bloomTresholdValue",std::to_string(bloom_treshold).c_str());
		gYume->pUI->SetCppProperty(0,"bloomSigmaValue",std::to_string(bloom_sigma).c_str());

		bool bloomEnabled = true;

		gYume->pUI->SetCppProperty(0,"bloomEnabled",std::to_string(bloomEnabled).c_str());


		float g_R = dp->GetShaderParameter("g_R").Get<float>();




		float g_NumSteps = dp->GetShaderParameter("g_NumSteps").Get<float>();
		float g_NumDir = dp->GetShaderParameter("g_NumDir").Get<float>();
		float g_AngleBias = dp->GetShaderParameter("g_AngleBias").Get<float>();
		float g_Attenuation = dp->GetShaderParameter("g_Attenuation").Get<float>();
		float g_Contrast = dp->GetShaderParameter("g_Contrast").Get<float>();
		float g_BlurRadius = dp->GetShaderParameter("g_BlurRadius").Get<float>();
		float g_EdgeThreshold = dp->GetShaderParameter("g_EdgeThreshold").Get<float>();
		float g_Sharpness = dp->GetShaderParameter("g_Sharpness").Get<float>();


		g_AngleBias *= M_RADTODEG;
		gYume->pUI->SetCppProperty(0,"HBAORadiusValue",std::to_string(g_R).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOAngleBiasValue",std::to_string(g_AngleBias).c_str());
		gYume->pUI->SetCppProperty(0,"HBAONumDirsValue",std::to_string(g_NumDir).c_str());
		gYume->pUI->SetCppProperty(0,"HBAONumStepsValue",std::to_string(g_NumSteps).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOContrastValue",std::to_string(g_Contrast).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOAttenValue",std::to_string(g_Attenuation).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOBlurRadiusValue",std::to_string(g_BlurRadius).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOBlurSharpnessValue",std::to_string(g_Sharpness).c_str());
		gYume->pUI->SetCppProperty(0,"HBAOBlurEdgeThresholdValue",std::to_string(g_EdgeThreshold).c_str());



		gYume->pUI->SetCppProperty(0,"VCTRoughnessValue","1");

		gYume->pUI->SetCppProperty(0,"lightWarmthValue","0");
		gYume->pUI->SetCppProperty(0,"lightFluxValue","0");


	}

	void YumeOptionsMenu::OnDomEvent(const YumeString& element,DOMEvents event,const YumeString& data)
	{
		switch(event)
		{
		case DOMEvents::InputChecked:
		{
			if(element == ("switchBloomEnabled"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("Bloom");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("Bloom");

			}

			if(element == ("switchHBAO"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("SSAO");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("SSAO");

			}

			if(element == ("switchGodrays"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("Godrays");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("Godrays");

			}

			if(element == ("switchFXAA"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("FXAA");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("FXAA");

			}

			if(element == ("switchLensDistortion"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("LensDistortion");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("LensDistortion");

			}

			if(element == ("switchDebugGI"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->SetShaderParameter("debug_gi",true);
				else
					gYume->pRenderer->GetDefaultPass()->SetShaderParameter("debug_gi",false);

			}

			if(element == ("switchDOF"))
			{
				if(data == ("true"))
					gYume->pRenderer->GetDefaultPass()->EnableRenderCalls("DoF");
				else
					gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("DoF");

			}
		}
		break;
		case DOMEvents::InputValueChanged:
		{
			if(element == "sliderlightFluxValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				lightFlux = value / 100.0f;

				Light* light = static_cast<Light*>(gYume->pRenderer->GetScene()->GetDirectionalLight());

				DirectX::XMVECTORF32 flux ={1,1.0f,1.0f - (lightWarmth / 2.f),1.0f};

				DirectX::XMFLOAT4 finalColor;
				XMStoreFloat4(&finalColor,flux * lightFlux);

				light->SetColor(YumeColor(finalColor.x,finalColor.y,finalColor.z,finalColor.w));
			}
			if(element == "sliderlightWarmthValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				float w = value / 100.f;

				DirectX::XMVECTORF32 flux ={1.0f,1.0f,1.0f - (w / 2.f),1.0f};

				Light* light = static_cast<Light*>(gYume->pRenderer->GetScene()->GetDirectionalLight());

				const DirectX::XMFLOAT4& pos = light->GetPosition();
				const DirectX::XMFLOAT4& dir = light->GetDirection();
				const YumeColor& color = light->GetColor();

				DirectX::XMFLOAT4 finalColor;
				XMStoreFloat4(&finalColor,flux * lightFlux);

				light->SetColor(YumeColor(finalColor.x,finalColor.y,finalColor.z,finalColor.w));

			}
			if(element == "sliderlpvFluxValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				dp->SetShaderParameter("lpv_flux_amplifier",value);

				gYume->pRenderer->UpdateGI();
			}

			if(element == "slidergiScaleValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				value /= 50;

				dp->SetShaderParameter("gi_scale",value);

				gYume->pRenderer->UpdateGI();
			}

			if(element == "sliderVCTRoughnessValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				gYume->pRenderer->SetFloorRoughness(value);
			}

			if(element == "sliderexposureSpeedValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				dp->SetShaderParameter("exposure_speed",value);
			}

			if(element == "sliderexposureKeyValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				dp->SetShaderParameter("exposure_key",value);
			}

			if(element == "sliderbloomSigmaValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				dp->SetShaderParameter("bloom_sigma",value);
			}

			if(element == "sliderbloomTresholdValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float value = atof(data.c_str());

				dp->SetShaderParameter("bloom_treshold",value);

			}

			if(element == "sliderHBAORadiusValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());

				dp->SetShaderParameter("g_R",R);
				dp->SetShaderParameter("g_inv_R",1.0f / R);
				dp->SetShaderParameter("g_sqr_R",R * R);
			}


			if(element == "sliderHBAOContrastValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float g_AngleBias = dp->GetShaderParameter("g_AngleBias").Get<float>();
				float c = atof(data.c_str());
				float contrast = c / (1.0f - sin(g_AngleBias * M_PI / 180));


				dp->SetShaderParameter("g_Contrast",contrast);
			}

			if(element == "sliderHBAONumStepsValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());

				dp->SetShaderParameter("g_NumSteps",R);
			}

			if(element == "sliderHBAONumDirsValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());

				dp->SetShaderParameter("g_NumDirs",R);
			}

			if(element == "sliderHBAOAngleBiasValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float angle = atof(data.c_str()) * M_PI/ 180;
				dp->SetShaderParameter("g_AngleBias",angle);
				dp->SetShaderParameter("g_TanAngleBias",tan(angle));

				float m_Contrast = dp->GetShaderParameter("g_Contrast").Get<float>();

				float contrast = m_Contrast / (1.0f - sin(angle * M_PI / 180));

				dp->SetShaderParameter("g_Contrast",contrast);
			}

			if(element == "sliderVCTRoughnessValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());


				dp->SetShaderParameter("glossiness",R);
			}

			if(element == "sliderGodraysTauValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());


				dp->SetShaderParameter("godrays_tau",R);
			}


			if(element == "sliderGodraysMistSpeedValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());


				dp->SetShaderParameter("MistSpeed",R);
			}

			if(element == "sliderGodraysNumSamplesValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());


				dp->SetShaderParameter("NumSamples",R);
			}


			if(element == "sliderGodraysDitheringValue")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				float R = atof(data.c_str());


				dp->SetShaderParameter("Dithering",R);
			}

			if(element == "godraysFogColor")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				YumeVector<YumeString>::type rgb = ParseFlags(data.c_str());

				int R = atof(rgb[0].c_str());
				int G = atof(rgb[1].c_str());
				int B = atof(rgb[2].c_str());

				DirectX::XMFLOAT4 rgbv4(R / 255.0f,G / 255.0f,B / 255.0f,1.0f);

				dp->SetShaderParameter("FogColor",rgbv4);
			}

			if(element == "lightColor")
			{
				RenderPass* dp = gYume->pRenderer->GetDefaultPass();

				YumeVector<YumeString>::type rgb = ParseFlags(data.c_str());

				int R = atof(rgb[0].c_str());
				int G = atof(rgb[1].c_str());
				int B = atof(rgb[2].c_str());

				DirectX::XMFLOAT4 rgbv4(R / 255.0f,G / 255.0f,B / 255.0f,1.0f);

				DirectX::XMVECTORF32 flux ={rgbv4.x,rgbv4.y,rgbv4.z - (lightWarmth / 2.f),1.0f};

				Light* light = static_cast<Light*>(gYume->pRenderer->GetScene()->GetDirectionalLight());

				const DirectX::XMFLOAT4& pos = light->GetPosition();
				const DirectX::XMFLOAT4& dir = light->GetDirection();
				const YumeColor& color = light->GetColor();

				DirectX::XMFLOAT4 finalColor;
				XMStoreFloat4(&finalColor,flux * lightFlux);

				light->SetColor(YumeColor(finalColor.x,finalColor.y,finalColor.z,finalColor.w));
			}
		}
		break;
		default:
			break;
		}
	}
}

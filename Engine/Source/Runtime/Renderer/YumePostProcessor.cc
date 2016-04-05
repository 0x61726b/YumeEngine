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
#include "YumePostProcessor.h"

#include "YumeRHI.h"
#include "YumeRenderPipeline.h"

namespace YumeEngine
{
	static const YumeString LightScatterFx = "VolumetricLightScatter";

	YumePostProcessor::YumePostProcessor()
		: lightScattering_(false)
	{
	}

	YumePostProcessor::~YumePostProcessor()
	{
	}

	void YumePostProcessor::SetDefaultParameters()
	{
		SetLightScatteringParameter(LS_SAMPLECOUNT,100);
		SetLightScatteringParameter(LS_DECAY,0.95f);
		SetLightScatteringParameter(LS_EXPOSURE,0.4f);
		SetLightScatteringParameter(LS_WEIGHT,0.08f);
	}

	void YumePostProcessor::ToggleLightScattering()
	{
		YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

		fx->ToggleEnabled(LightScatterFx);

		lightScattering_ = !lightScattering_;
	}

	void YumePostProcessor::SetEnableLightScattering(bool enabled)
	{
		YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();
		fx->SetEnabled(LightScatterFx,enabled);
		lightScattering_  = enabled;
	}



	void YumePostProcessor::SetLightScatteringParameter(LightScatteringParams param,const Variant& var)
	{
		YumeRenderPipeline* fx = gYume->pRenderer->GetViewport(0)->GetRenderPath();

		switch(param)
		{
		case LS_SAMPLECOUNT:
			fx->SetShaderParameter("LSSampleCount",var);
			break;
		case LS_DECAY:
			fx->SetShaderParameter("LSDecay",var);
			break;
		case LS_EXPOSURE:
			fx->SetShaderParameter("LSExposure",var);
			break;
		case LS_WEIGHT:
			fx->SetShaderParameter("LSWeight",var);
			break;

		default:
			break;
		}
	}

}

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
#ifndef __YumePostProcessor_h__
#define __YumePostProcessor_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum LightScatteringParams
	{
		LS_SAMPLECOUNT,
		LS_WEIGHT,
		LS_DECAY,
		LS_EXPOSURE
	};
	class YumeAPIExport YumePostProcessor : public YumeBase
	{
	public:
		YumePostProcessor();
		virtual ~YumePostProcessor();

		void SetDefaultParameters();
		//Light scatter
		void SetLightScatteringParameter(LightScatteringParams param,const Variant& var);

		void ToggleLightScattering();
		void SetEnableLightScattering(bool enabled);

		bool GetLightScatteringEnabled() const { return lightScattering_; }

		bool lightScattering_;
	};
}


//----------------------------------------------------------------------------
#endif

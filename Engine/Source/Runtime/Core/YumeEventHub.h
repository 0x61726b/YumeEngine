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
#ifndef __YumeEventHub_h__
#define __YumeEventHub_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum YumeEngineEvents
	{
		E_UPDATE,
		E_POSTUPDATE,
		R_UPDATE,
		R_POSTUPDATE,
		S_UPDATE,
		S_POSTUPDATE
	};

	class YumeScene;

	class YumeAPIExport EngineEventListener
	{
	public:
		virtual void HandleUpdate(float timeStep) { };
		virtual void HandlePostUpdate(float timeStep) { };
		virtual void HandleRenderUpdate(float timeStep) { };
		virtual void HandlePostRenderUpdate(float timeStep) { };
	};

	class YumeAPIExport RHIEventListener
	{
	public:
		virtual void HandleGraphicsMode() { };
	};

	class YumeAPIExport InputEventListener
	{
	}
	;
	class YumeAPIExport SceneEventListener
	{
	public:
		virtual void HandleSceneUpdate(YumeScene* scene,float timeStep) { };
		virtual void HandleScenePostUpdate(YumeScene* scene,float timeStep) { };
	};

}


//----------------------------------------------------------------------------
#endif

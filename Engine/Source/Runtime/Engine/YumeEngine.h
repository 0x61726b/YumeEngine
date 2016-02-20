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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeEngine_h__
#define __YumeEngine_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGraphics;

	class YumeAPIExport YumeEngine3D
	{
	public:
		YumeEngine3D();

		bool Initialize();

		void Run();

		void Exit();

		void Update();
		void Render();

		bool IsExiting() const { return exiting_; }

	private:
		boost::shared_ptr<YumeGraphics> graphics_;
	private:
		bool initialized_;
		bool exiting_;
	};
}


//----------------------------------------------------------------------------
#endif

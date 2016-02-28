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
#ifndef __YumeBase_h__
#define __YumeBase_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeBase
	{
	public:
		YumeBase(){};
		virtual ~YumeBase() {};
	};
	class YumeAPIExport YumeObjectFactory
	{
	public:
		YumeObjectFactory();
		virtual ~YumeObjectFactory();

		SharedPtr<YumeBase> Create(YumeHash type);
		void RegisterFactoryFunction(YumeHash type,std::function<YumeBase*(void)> classFactoryFunction);

		typedef YumeMap<YumeHash,std::function<YumeBase*(void)> >::type ObjRegistry;

		ObjRegistry functionRegistry;

		static YumeObjectFactory* Get();
	};


	template<class T>
	class YumeAPIExport YumeObjectRegistrar 
	{
	public:
		YumeObjectRegistrar(YumeHash type)
		{
			YumeObjectFactory::Get()->RegisterFactoryFunction(type,[](void) -> YumeBase * { return new T();});
		}

	};

#define REGISTER_CLASS(c,t) static YumeObjectRegistrar<c> registrar(t)
}


//----------------------------------------------------------------------------
#endif

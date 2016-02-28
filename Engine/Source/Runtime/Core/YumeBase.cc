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
#include "YumeBase.h"

#include "YumeDefaults.h"

namespace YumeEngine
{
	YumeObjectFactory* GlobalFactoryObject = 0;

	YumeObjectFactory::YumeObjectFactory()
	{
		GlobalFactoryObject = this;
	}

	YumeObjectFactory::~YumeObjectFactory()
	{
		
	}

	SharedPtr<YumeBase> YumeObjectFactory::Create(YumeHash type)
	{
		YumeBase* instance = nullptr;

		ObjRegistry::iterator It = functionRegistry.find(type);

		if(It != functionRegistry.end())
			instance = It->second();

		if(instance != nullptr)
			return SharedPtr<YumeBase>(instance);
		else
			return nullptr;
	}

	YumeObjectFactory* YumeObjectFactory::Get()
	{
		return GlobalFactoryObject;
	}

	void YumeObjectFactory::RegisterFactoryFunction(YumeHash type,std::function<YumeBase*(void)> classFactoryFunction)
	{
		functionRegistry.insert(std::make_pair(type,classFactoryFunction));
	}
}

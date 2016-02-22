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
#include "YumeHeaders.h"
#include "YumeEnvironment.h"

#include "YumeDynamicLibrary.h"

#include "Logging/logging.h"

namespace YumeEngine
{
	YumeEnvironment::YumeEnvironment()
	{
		std::string appData;

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		appData = std::string(std::getenv("APPDATA"));
#elif YUME_PLATFORM == YUME_PLATFORM_LINUX
		appData = std::string(std::getenv("APPDATA"));
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
		appData = std::string(std::getenv("APPDATA"));
#endif



#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		appDataPath_ = boost::filesystem::path(appData);
#else
		appDataPath_ = boost::filesystem::path(appData);
#endif


		boost::filesystem::path yumeConfigsPath = appDataPath_ / "YumeEngine";

		YUMELOG_INFO("Initializing environment..."
			<< std::endl << "Engine Config Path: " << yumeConfigsPath.c_str());

		

		CreateDirectory(yumeConfigsPath);

		configFile_ = yumeConfigsPath / "Yume.config";
		logFile_ = yumeConfigsPath / "Yume.log";
	}

	YumeEnvironment::~YumeEnvironment()
	{
		// Unload & delete resources in turn
		for(DynLibMap::iterator it = dynLibMap_.begin(); it != dynLibMap_.end(); ++it)
		{
			it->second->Unload();
			YumeAPIDelete it->second;
		}

		// Empty the list
		dynLibMap_.clear();
	}


	YumeDynamicLibrary* YumeEnvironment::LoadDynLib(const YumeString& name)
	{
		DynLibMap::iterator i = dynLibMap_.find(name);
		if(i != dynLibMap_.end())
		{
			return i->second;
		}
		else
		{
			YumeDynamicLibrary* pLib = YumeAPINew YumeDynamicLibrary(name);
			if(!pLib->Load())
				return NULL;
			dynLibMap_[name] = pLib;
			return pLib;
		}
	}

	void YumeEnvironment::UnloadDynLib(YumeDynamicLibrary* lib)
	{
		DynLibMap::iterator i = dynLibMap_.find(lib->GetName());
		if(i != dynLibMap_.end())
		{
			dynLibMap_.erase(i);
		}
		lib->Unload();
		YumeAPIDelete lib;
	}

	bool YumeEnvironment::CreateDirectory(const boost::filesystem::path& path)
	{
		return boost::filesystem::create_directory(path);
	}

	bool YumeEnvironment::Exists(boost::filesystem::path path)
	{
		return boost::filesystem::exists(path);
	}
}

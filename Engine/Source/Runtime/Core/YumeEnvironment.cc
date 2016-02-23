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

#include "Core/YumeFile.h"
#include "Core/YumeXmlParser.h"
#include "Core/YumeDefaults.h"

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
		appData = std::string(std::getenv("HOME"));
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
		appData = std::string(std::getenv("HOME"));
#endif



#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		appDataPath_ = boost::filesystem::path(appData);
#else
		appDataPath_ = boost::filesystem::path(appData);
#endif

		//On Windows this will be C:/User/AppData/Roaming/YumeEngine
		//On linux this will be /home/user/YumeEngine
		root_ = appDataPath_ / "YumeEngine";



		CreateDirectory(root_);

		configFile_ = root_ / "Yume.config";
		logFile_ = root_ / "Yume.log";

		ReadAndParseConfig();

		//Append command line arguments to engine config
		StringVector commandLine = GetArguments();

		for(int i=0; i < commandLine.size(); ++i)
		{
			engineConfig_.insert( ConfigMap::value_type(commandLine[i],"1"));
		}
			
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
            dynLibMap_.insert( std::make_pair(name,pLib) );
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

	const YumeString& YumeEnvironment::GetParameter(const YumeString& param)
	{
		return engineConfig_[param];
	}

	void YumeEnvironment::ReadAndParseConfig()
	{
		if(!boost::filesystem::exists(configFile_))
		{
			CreateConfigFile(configFile_);
		}
		YumeFile file(configFile_);

		YumeString configContent = file.Read();
		file.Close();

		ConfigMap configMap = Parsers::ParseConfig(configContent);

		ConfigMap::iterator It = configMap.begin();

		for(It;It != configMap.end(); ++It)
		{
			engineConfig_.insert(*It);
		}
		
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

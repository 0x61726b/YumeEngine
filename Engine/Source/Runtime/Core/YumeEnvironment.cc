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


		const size_t cSize = strlen(appData.c_str());
		std::wstring wstr(cSize,L'#');
		mbstowcs(&wstr[0],appData.c_str(),cSize);

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		appDataPath_ = boost::filesystem::path(wstr);
#else
		appDataPath_ = boost::filesystem::path(wstr);
#endif


		boost::filesystem::path ym = appDataPath_ / L"YumeEngine";

		YUMELOG_INFO("Initializing environment..."
			<< std::endl << "Engine Config Path: " << yumeConfigsPath.c_str());

		configFile_ = yumeConfigsPath / L"Yume.config";



		if(boost::filesystem::exists(L"C:\\Users\\alperen\\AppData\\Roaming\\YumeEngine"))
		{
			std::cout << "Exists";
		}
		else
		{
			std::cout << "Does not Exists";
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

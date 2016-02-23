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
#ifndef __YumeEnvironment_h__
#define __YumeEnvironment_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/filesystem.hpp>

//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeDynamicLibrary;
	typedef boost::filesystem::path FsPath;
	typedef YumeMap<YumeString,YumeString>::type ConfigMap;

	class YumeFile;

	class YumeAPIExport YumeEnvironment
	{
	public:
		YumeEnvironment();
		virtual ~YumeEnvironment();

		bool Exists(boost::filesystem::path path);
		bool CreateDirectory(const boost::filesystem::path& path);

		void ReadAndParseConfig();
		

		const FsPath& GetLogFile() { return logFile_; }
		const FsPath& GetRoot() { return root_; }

		const YumeString& GetParameter(const YumeString&);

		YumeDynamicLibrary* LoadDynLib(const YumeString& name);
		void UnloadDynLib(YumeDynamicLibrary*);
	private:
		ConfigMap engineConfig_;
	private:
		boost::filesystem::path		appDataPath_;
		boost::filesystem::path		root_;
		boost::filesystem::path		configFile_;
		boost::filesystem::path		logFile_;

		typedef YumeMap<YumeString,YumeDynamicLibrary*>::type DynLibMap;
		DynLibMap dynLibMap_;
	};
}


//----------------------------------------------------------------------------
#endif

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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeIO.h"


#include <sys/stat.h>

#ifdef _WIN32
#include <cstdio>
#ifndef _MSC_VER
#define _WIN32_IE 0x501
#endif
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/utime.h>
#else
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <utime.h>
#include <sys/wait.h>
#define MAX_PATH 256
#endif

namespace YumeEngine
{
	YumeIO::YumeIO()
	{
	}

	YumeIO::~YumeIO()
	{
	}

	bool YumeIO::IsDirectoryExist(const FsPath& path)
	{
		return boost::filesystem::exists(path);
	}

	FsPath YumeIO::GetBinaryRoot()
	{
		FsPath current = GetCurrentPath();

		//ToDo(arkenthera) fix this by working around Debug/Release that vs uses
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		FsPath root = current / "..";
#else
		FsPath root = current / "..";
#endif
		return boost::filesystem::absolute(root);
	}

	boost::filesystem::path YumeIO::GetCurrentPath() const
	{
		return boost::filesystem::current_path();
	}

	unsigned YumeIO::GetLastModifiedTime(const YumeString& fileName) const
	{
		if(fileName.empty())
			return 0;

#ifdef _WIN32
		struct _stat st;
		if(!_stat(fileName.c_str(),&st))
			return (unsigned)st.st_mtime;
		else
			return 0;
#else
		struct stat st;
		if (!stat(fileName.c_str(), &st))
			return (unsigned)st.st_mtime;
		else
			return 0;
#endif
	}

	bool YumeIO::CreateDir(const FsPath& path)
	{
		return boost::filesystem::create_directory(path);
	}

	void SplitPath(const YumeString& fullPath,YumeString& pathName,YumeString& fileName,YumeString& extension,bool lowercaseExtension)
	{
		//Shaders/HLSL/LitSolid.hlsl
		YumeString copy = fullPath;

		size_t extensionPos = copy.find_last_of(".");
		size_t pathPos = copy.find_last_of("/");

		size_t size = copy.length();

		extension = copy.substr(extensionPos,size - extensionPos);

		copy = copy.substr(0,extensionPos);

		size = copy.length();

		fileName = copy.substr(pathPos+1,size - pathPos +1);

		pathName = copy.substr(0,pathPos+1);
	}

	YumeString GetPath(const YumeString& fullPath)
	{
		YumeString path,file,extension;
		SplitPath(fullPath,path,file,extension);
		return path;
	}

	YumeString GetFileName(const YumeString& fullPath)
	{
		YumeString path,file,extension;
		SplitPath(fullPath,path,file,extension);
		return file;
	}

	YumeString GetExtension(const YumeString& fullPath,bool lowercaseExtension)
	{
		YumeString path,file,extension;
		SplitPath(fullPath,path,file,extension,lowercaseExtension);
		return extension;
	}
}

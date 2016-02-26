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
		FsPath root = current / ".." / ".." / "..";
#else
		FsPath root = current / ".." / "..";
#endif
		return boost::filesystem::absolute(root);
	}

	boost::filesystem::path YumeIO::GetCurrentPath() const
	{
		return boost::filesystem::current_path();
	}
}

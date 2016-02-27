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
#ifndef __YumeDefaults_h__
#define __YumeDefaults_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/filesystem.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	void CreateConfigFile(const boost::filesystem::path& path);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const YumeString&,bool skipFirst = true);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const char* cmdLine);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const std::wstring& cmdLine);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(int argc,char** argv);

	YumeAPIExport const YumeString& WCharToUtf8(const std::wstring&);

	YumeAPIExport YumeHash GenerateHash(const YumeString& key);

	YumeAPIExport void ErrorDialog(const YumeString& title,const YumeString& message);

	YumeAPIExport void ErrorExit(const YumeString& message,int exitCode);

	const YumeVector<YumeString>::type& GetArguments();
}


//----------------------------------------------------------------------------
#endif

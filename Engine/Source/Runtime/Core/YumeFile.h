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
#ifndef __YumeFile_h__
#define __YumeFile_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum FileMode
	{
		FILEMODE_READ,
		FILEMODE_WRITE,
		FILEMODE_READWRITE
	};
	class YumeAPIExport YumeFile : public GeneralObjAlloc
	{
	public:
		YumeFile(const std::string& file,FileMode = FILEMODE_READ);
		YumeFile(const boost::filesystem::path& file,FileMode = FILEMODE_READ);

		virtual ~YumeFile();

		bool Open(const std::string& file,FileMode filemode = FILEMODE_READ);
		unsigned Read(void* dest,int size);

		void Write(const YumeString& str);

		YumeString Read();

		void Close();
		const YumeString& GetName() const { return fileName_; }


	private:
		YumeString fileName_;
		FileMode fileMode_;

		boost::filesystem::fstream fileStream;
	};
}


//----------------------------------------------------------------------------
#endif

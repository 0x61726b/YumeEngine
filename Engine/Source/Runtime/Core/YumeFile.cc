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
#include "YumeFile.h"

#include "Logging/logging.h"




namespace fs = boost::filesystem;

namespace YumeEngine
{
	YumeFile::YumeFile(const YumeString& file,FileMode fileMode)
		: file_(file),fileMode_(fileMode),position_(0),size_(0)
	{
		Open(file_,fileMode_);
	}

	YumeFile::YumeFile(const boost::filesystem::path& file,FileMode filemode) :
		file_(file.generic_string()),fileMode_(filemode),position_(0),size_(0)
	{
		Open(file_,fileMode_);
	}

	YumeFile::~YumeFile()
	{
	}

	bool YumeFile::Open(const std::string& file,FileMode filemode)
	{
		file_ = file;
		fileMode_ = filemode;

		//std::ios_base::openmode fileMode;
		//if(filemode == FILEMODE_READ)
		//	fileMode = fs::fstream::in;
		//else
		//	fileMode = fs::fstream::in || fs::fstream::out;

		fs::path p = fs::path(file);

		if(fs::exists(p))
		{
			fileStream.open(p,std::fstream::in | std::fstream::out | std::fstream::binary);

			size_ = GetSize();

			return fileStream.is_open();
		}
		else
		{
			if(fileMode_ == FILEMODE_WRITE)
			{
				fileStream.open(p,std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

				return fileStream.is_open();
			}
		}
		return fileStream.is_open();
	}

	void YumeFile::Close()
	{
		if(fileStream.is_open())
			fileStream.close();
	}

	void YumeFile::Write(const YumeString& str)
	{
		if(!fileStream.is_open())
			return;

		fileStream << str;
		fileStream.flush();
	}

	unsigned YumeFile::Read(void* dest,int size)
	{
		if(size + position_ > size_)
			size = size_ - position_;
		if(!size)
			return 0;

		char* destPtr = (char*)dest;
		fileStream.read(destPtr,size);

		position_ += size;

		return size;
	}

	YumeString YumeFile::ReadLine()
	{
		YumeString line;
		if(std::getline(fileStream,line))
		{
			return line;
		}
	}

	bool YumeFile::Eof() const
	{
		return fileStream.eof();
	}

	YumeString YumeFile::Read()
	{
		YumeString str;
		YumeStringStream sstr;
		while(fileStream)
		{
			std::getline(fileStream,str);
			sstr << str;
		}
		return sstr.str();
	}
	unsigned YumeFile::Write(const void* str,unsigned size)
	{
		if(fileMode_ == FILEMODE_READ)
		{
			YUMELOG_ERROR("File isn't open for writing!");
			return 0;
		}

		if(!size)
			return 0;

		fileStream.write((char*)str,size);

		return size;
	}

	bool YumeFile::WriteFileID(const YumeString& value)
	{
		bool success = true;
		unsigned length = (unsigned)std::min((int)value.length(),4);

		success &= Write(value.c_str(),length) == length;
		for(unsigned i = value.length(); i < 4; ++i)
			success &= WriteByte(' ');
		return success;
	}

	bool YumeFile::WriteByte(signed char value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}
	bool YumeFile::WriteUInt(unsigned value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}
	bool YumeFile::WriteUByte(signed char value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}
	bool YumeFile::WriteShort(short value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}
	bool YumeFile::WriteUShort(unsigned short value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}


	void YumeFile::Seek(unsigned s)
	{
		fileStream.seekg(s);
		position_ = s;
	}

	void YumeFile::SetName(const YumeString& name)
	{
		fileName_ = name;
	}

	unsigned YumeFile::GetSize()
	{
		return boost::filesystem::file_size(file_);
	}

	YumeString YumeFile::GetFileExtension()
	{
		YumeString ret;
		ret.resize(4);
		Read(&ret[0],4);
		return ret;
	}

}

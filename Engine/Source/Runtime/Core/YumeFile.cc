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

#include "YumeDefaults.h"

#include "Math/YumeMath.h"


namespace fs = boost::filesystem;

namespace YumeEngine
{

	static const char* openMode[] =
	{
		"rb",
		"wb",
		"r+b",
		"w+b"
	};


	YumeFile::YumeFile(const YumeString& file,FileMode fileMode)
		: file_(file),fileMode_(fileMode),position_(0),size_(0),handle_(0)
	{
		Open(file_,fileMode_);
	}

	YumeFile::YumeFile(const boost::filesystem::path& file,FileMode filemode) :
		file_(file.generic_string()),fileMode_(filemode),position_(0),size_(0),handle_(0)
	{
		Open(file_,fileMode_);
	}

	YumeFile::~YumeFile()
	{
		Close();
	}

	bool YumeFile::Open(const std::string& file,FileMode filemode)
	{
		Close();

		file_ = file;
		fileMode_ = filemode;
		position_ = 0;
		offset_ = 0;
		checksum_ = 0;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;

		//std::ios_base::openmode fileMode;
		//if(filemode == FILEMODE_READ)
		//	fileMode = fs::fstream::in;
		//else
		//	fileMode = fs::fstream::in || fs::fstream::out;

		fs::path p = fs::path(file);

		if(!fs::exists(p) && filemode == FILEMODE_READ)
		{
			YUMELOG_ERROR("Can't open non-existent file for reading." << file);
			return false;
		}

		handle_ = fopen(file.c_str(),openMode[filemode]);

		if(filemode == FILEMODE_READWRITE && !handle_)
		{
			handle_ = fopen(file.c_str(),openMode[filemode + 1]);
		}

		if(!handle_)
		{
			YUMELOG_ERROR("Could not open file " << file.c_str());
			return false;
		}


		fseek((FILE*)handle_,0,SEEK_END);
		long size = ftell((FILE*)handle_);
		fseek((FILE*)handle_,0,SEEK_SET);
		if(size > Math::M_MAX_UNSIGNED)
		{
			YUMELOG_ERROR("Could not open file which is larger than 4GB" << file_.c_str());
			Close();
			size_ = 0;
			return false;
		}
		size_ = (unsigned)size;
		return true;
	}

	void YumeFile::Close()
	{
		if(handle_)
		{
			fclose((FILE*)handle_);
			handle_ = 0;
			position_ = 0;
			size_ = 0;
			offset_ = 0;
			checksum_ = 0;
		}
	}

	void YumeFile::Flush()
	{
		if(handle_)
			fflush((FILE*)handle_);
	}

	bool YumeFile::WriteString(const YumeString& str)
	{
		const char* chars = str.c_str();
		// Count length to the first zero, because ReadString() does the same
		unsigned length = CStringLength(chars);
		return Write(chars,length + 1) == length + 1;
	}

	unsigned YumeFile::Read(void* dest,int size)
	{
		if(!handle_)
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		if(fileMode_ == FILEMODE_WRITE)
		{
			YUMELOG_ERROR("File not opened for reading");
			return 0;
		}

		if(size + position_ > size_)
			size = size_ - position_;
		if(!size)
			return 0;

		// Need to reassign the position due to internal buffering when transitioning from writing to reading
		if(readSyncNeeded_)
		{
			fseek((FILE*)handle_,position_ + offset_,SEEK_SET);
			readSyncNeeded_ = false;
		}

		size_t ret = fread(dest,size,1,(FILE*)handle_);
		if(ret != 1)
		{
			// Return to the position where the read began
			fseek((FILE*)handle_,position_ + offset_,SEEK_SET);
			YUMELOG_ERROR("Error while reading from file " + GetName());
			return 0;
		}

		writeSyncNeeded_ = true;
		position_ += size;
		return size;
	}

	YumeString YumeFile::ReadLine()
	{
		YumeString ret;

		while(!Eof())
		{
			char c = ReadByte();
			if(c == 10)
				break;
			if(c == 13)
			{
				// Peek next char to see if it's 10, and skip it too
				if(!Eof())
				{
					char next = ReadByte();
					if(next != 10)
						Seek(position_ - 1);
				}
				break;
			}

			ret += c;
		}

		return ret;
	}

	bool YumeFile::Eof() const
	{
		return position_ >= size_;
	}

	YumeString YumeFile::ReadString()
	{
		YumeString ret;

		while(!Eof())
		{
			char c = ReadByte();
			if(!c)
				break;
			else
				ret += c;
		}

		return ret;
	}


	unsigned YumeFile::Write(const void* data,unsigned size)
	{
		if(!handle_)
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		if(fileMode_ == FILEMODE_READ)
		{
			YUMELOG_ERROR("File not opened for writing");
			return 0;
		}

		if(!size)
			return 0;

		// Need to reassign the position due to internal buffering when transitioning from reading to writing
		if(writeSyncNeeded_)
		{
			fseek((FILE*)handle_,position_ + offset_,SEEK_SET);
			writeSyncNeeded_ = false;
		}

		if(fwrite(data,size,1,(FILE*)handle_) != 1)
		{
			// Return to the position where the write began
			fseek((FILE*)handle_,position_ + offset_,SEEK_SET);
			YUMELOG_ERROR("Error while writing to file " + GetName());
			return 0;
		}

		readSyncNeeded_ = true;
		position_ += size;
		if(position_ > size_)
			size_ = position_;

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
	bool YumeFile::WriteUByte(unsigned char value)
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

	signed char YumeFile::ReadByte()
	{
		signed char ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned YumeFile::ReadUInt()
	{
		unsigned ret = 0;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned char YumeFile::ReadUByte()
	{
		unsigned char ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned short YumeFile::ReadUShort()
	{
		unsigned short ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	short YumeFile::ReadShort()
	{
		short ret;
		Read(&ret,sizeof ret);
		return ret;
	}
	unsigned YumeFile::Seek(unsigned position)
	{
		if(!handle_)
		{
			// Do not log the error further here to prevent spamming the stderr stream
			return 0;
		}

		// Allow sparse seeks if writing
		if(fileMode_ == FILEMODE_READ && position > size_)
			position = size_;

		fseek((FILE*)handle_,position + offset_,SEEK_SET);
		position_ = position;
		readSyncNeeded_ = false;
		writeSyncNeeded_ = false;
		return position_;
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

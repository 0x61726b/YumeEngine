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
#include "YumeVariant.h"

#include "Math/YumeBoundingBox.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum FileMode
	{
		FILEMODE_READ = 0,
		FILEMODE_WRITE,
		FILEMODE_READWRITE
	};
	class YumeAPIExport YumeFile : public YumeBase
	{
	public:
		YumeFile(const YumeString& file,FileMode = FILEMODE_READ);
		YumeFile(const boost::filesystem::path& file,FileMode = FILEMODE_READ);

		virtual ~YumeFile();

		bool Open(const YumeString& file,FileMode filemode = FILEMODE_READ);
		unsigned Read(void* dest,int size);
		YumeString ReadLine();

		static YumeHash GetTypeStatic() { return type_; }
		virtual YumeHash GetType() { return type_; }
		static YumeHash type_;

		bool Eof() const;

		bool WriteString(const YumeString& str);
		unsigned Write(const void* str,unsigned len);
		bool WriteFileID(const YumeString& str);
		bool WriteByte(signed char value);
		bool WriteUInt(unsigned value);
		bool WriteUByte(unsigned char value);
		bool WriteShort(short value);
		bool WriteUShort(unsigned short value);
		bool WriteInt(int);
		bool WriteBool(bool);
		bool WriteFloat(float);
		bool WriteDouble(double);
		bool WriteVector4(DirectX::XMFLOAT4& v);
		bool WriteVector3(DirectX::XMFLOAT3& v);
		bool WriteVariantData(const Variant& v);

		YumeString ReadString();
		signed char ReadByte();
		unsigned ReadUInt();
		int ReadInt();
		bool ReadBool();
		float ReadFloat();
		double ReadDouble();
		unsigned char ReadUByte();
		short ReadShort();
		unsigned short ReadUShort();
		Variant ReadVariant();
		Variant ReadVariant(VariantType type);
		BoundingBox ReadBoundingBox();
		DirectX::XMFLOAT3 ReadVector3();
		DirectX::XMFLOAT4 ReadVector4();
		Quaternion ReadQuaternion();

		unsigned GetSize();

		unsigned Seek(unsigned);

		void Flush();
		void Close();
		const YumeString& GetName() const { return fileName_; }
		void SetName(const YumeString& name);

		YumeString GetFileExtension();
	private:
		YumeString fileName_;
		YumeString file_;
		FileMode fileMode_;

		void* handle_;

		unsigned int position_;
		unsigned int size_;
		unsigned int checksum_;
		unsigned int offset_;
		bool readSyncNeeded_;
		bool writeSyncNeeded_;
	};
}


//----------------------------------------------------------------------------
#endif

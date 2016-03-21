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
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeStreamWriter.h"
#include "YumeDefaults.h"


namespace YumeEngine
{

	static const float q = 32767.0f;

	StreamWriter::~StreamWriter()
	{
	}

	bool StreamWriter::WriteInt(int value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteShort(short value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteByte(signed char value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteUInt(unsigned value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteUShort(unsigned short value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteUByte(unsigned char value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteBool(bool value)
	{
		return WriteUByte((unsigned char)(value ? 1 : 0)) == 1;
	}

	bool StreamWriter::WriteFloat(float value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteDouble(double value)
	{
		return Write(&value,sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteIntRect(const IntRect& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteIntVector2(const IntVector2& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteRect(const Rect& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteVector2(const Vector2& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteVector3(const Vector3& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WritePackedVector3(const Vector3& value,float maxAbsCoord)
	{
		short coords[3];
		float v = 32767.0f / maxAbsCoord;

		coords[0] = (short)(Clamp(value.x_,-maxAbsCoord,maxAbsCoord) * v + 0.5f);
		coords[1] = (short)(Clamp(value.y_,-maxAbsCoord,maxAbsCoord) * v + 0.5f);
		coords[2] = (short)(Clamp(value.z_,-maxAbsCoord,maxAbsCoord) * v + 0.5f);
		return Write(&coords[0],sizeof coords) == sizeof coords;
	}

	bool StreamWriter::WriteVector4(const Vector4& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteQuaternion(const Quaternion& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WritePackedQuaternion(const Quaternion& value)
	{
		short coords[4];
		Quaternion norm = value.Normalized();

		coords[0] = (short)(Clamp(norm.w_,-1.0f,1.0f) * q + 0.5f);
		coords[1] = (short)(Clamp(norm.x_,-1.0f,1.0f) * q + 0.5f);
		coords[2] = (short)(Clamp(norm.y_,-1.0f,1.0f) * q + 0.5f);
		coords[3] = (short)(Clamp(norm.z_,-1.0f,1.0f) * q + 0.5f);
		return Write(&coords[0],sizeof coords) == sizeof coords;
	}

	bool StreamWriter::WriteMatrix3(const Matrix3& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteMatrix3x4(const Matrix3x4& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteMatrix4(const Matrix4& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteColor(const YumeColor& value)
	{
		return Write(value.Data(),sizeof value) == sizeof value;
	}

	bool StreamWriter::WriteBoundingBox(const BoundingBox& value)
	{
		bool success = true;
		success &= WriteVector3(value.min_);
		success &= WriteVector3(value.max_);
		return success;
	}

	bool StreamWriter::WriteString(const YumeString& value)
	{
		const char* chars = value.c_str();
		// Count length to the first zero, because ReadString() does the same
		unsigned length = CStringLength(chars);
		return Write(chars,length + 1) == length + 1;
	}

	bool StreamWriter::WriteFileID(const YumeString& value)
	{
		bool success = true;
		unsigned length = Min((int)value.length(),4U);

		success &= Write(value.c_str(),length) == length;
		for(unsigned i = value.length(); i < 4; ++i)
			success &= WriteByte(' ');
		return success;
	}

	bool StreamWriter::WriteStringHash(const YumeHash& value)
	{
		return WriteUInt(value.Value());
	}

	bool StreamWriter::WriteBuffer(const YumeVector<unsigned char>::type& value)
	{
		bool success = true;
		unsigned size = value.size();

		success &= WriteVLE(size);
		if(size)
			success &= Write(&value[0],size) == size;
		return success;
	}

	bool StreamWriter::WriteResourceRef(const ResourceRef& value)
	{
		bool success = true;
		success &= WriteStringHash(value.type_);
		success &= WriteString(value.name_);
		return success;
	}

	bool StreamWriter::WriteResourceRefList(const ResourceRefList& value)
	{
		bool success = true;

		success &= WriteStringHash(value.type_);
		success &= WriteVLE(value.names_.size());
		for(unsigned i = 0; i < value.names_.size(); ++i)
			success &= WriteString(value.names_[i]);

		return success;
	}

	bool StreamWriter::WriteVariant(const Variant& value)
	{
		bool success = true;
		VariantType type = value.GetType();

		success &= WriteUByte((unsigned char)type);
		success &= WriteVariantData(value);
		return success;
	}

	bool StreamWriter::WriteVariantData(const Variant& value)
	{
		switch(value.GetType())
		{
		case VAR_EMPTY:
			return true;

		case VAR_INT:
			return WriteInt(value.GetInt());

		case VAR_BOOL:
			return WriteBool(value.GetBool());

		case VAR_FLOAT:
			return WriteFloat(value.GetFloat());

		case VAR_VECTOR2:
			return WriteVector2(value.GetVector2());

		case VAR_VECTOR3:
			return WriteVector3(value.GetVector3());

		case VAR_VECTOR4:
			return WriteVector4(value.GetVector4());

		case VAR_QUATERNION:
			return WriteQuaternion(value.GetQuaternion());

		case VAR_COLOR:
			return WriteColor(value.GetColor());

		case VAR_STRING:
			return WriteString(value.GetString());

		case VAR_BUFFER:
			return WriteBuffer(value.GetBuffer());

			// Serializing pointers is not supported. Write null
		case VAR_VOIDPTR:
		case VAR_PTR:
			return WriteUInt(0);

		case VAR_RESOURCEREF:
			return WriteResourceRef(value.GetResourceRef());

		case VAR_RESOURCEREFLIST:
			return WriteResourceRefList(value.GetResourceRefList());

		case VAR_VARIANTVECTOR:
			return WriteVariantVector(value.GetVariantVector());

		case VAR_STRINGVECTOR:
			return WriteStringVector(value.GetStringVector());

		case VAR_VARIANTMAP:
			return WriteVariantMap(value.GetVariantMap());

		case VAR_INTRECT:
			return WriteIntRect(value.GetIntRect());

		case VAR_INTVECTOR2:
			return WriteIntVector2(value.GetIntVector2());

		case VAR_MATRIX3:
			return WriteMatrix3(value.GetMatrix3());

		case VAR_MATRIX3X4:
			return WriteMatrix3x4(value.GetMatrix3x4());

		case VAR_MATRIX4:
			return WriteMatrix4(value.GetMatrix4());

		case VAR_DOUBLE:
			return WriteDouble(value.GetDouble());

		default:
			return false;
		}
	}

	bool StreamWriter::WriteVariantVector(const VariantVector::type& value)
	{
		bool success = true;
		success &= WriteVLE(value.size());
		for(VariantVector::const_iterator i = value.begin(); i != value.end(); ++i)
			success &= WriteVariant(*i);
		return success;
	}

	bool StreamWriter::WriteStringVector(const StringVector::type& value)
	{
		bool success = true;
		success &= WriteVLE(value.size());
		for(StringVector::const_iterator i = value.begin(); i != value.end(); ++i)
			success &= WriteString(*i);
		return success;
	}

	bool StreamWriter::WriteVariantMap(const VariantMap::type& value)
	{
		bool success = true;
		success &= WriteVLE(value.size());
		for(VariantMap::const_iterator i = value.begin(); i != value.end(); ++i)
		{
			WriteStringHash(i->first);
			WriteVariant(i->second);
		}
		return success;
	}

	bool StreamWriter::WriteVLE(unsigned value)
	{
		unsigned char data[4];

		if(value < 0x80)
			return WriteUByte((unsigned char)value);
		else if(value < 0x4000)
		{
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)(value >> 7);
			return Write(data,2) == 2;
		}
		else if(value < 0x200000)
		{
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)((value >> 7) | 0x80);
			data[2] = (unsigned char)(value >> 14);
			return Write(data,3) == 3;
		}
		else
		{
			data[0] = (unsigned char)(value | 0x80);
			data[1] = (unsigned char)((value >> 7) | 0x80);
			data[2] = (unsigned char)((value >> 14) | 0x80);
			data[3] = (unsigned char)(value >> 21);
			return Write(data,4) == 4;
		}
	}

	bool StreamWriter::WriteNetID(unsigned value)
	{
		return Write(&value,3) == 3;
	}

	bool StreamWriter::WriteLine(const YumeString& value)
	{
		bool success = true;
		success &= Write(value.c_str(),value.length()) == value.length();
		success &= WriteUByte(13);
		success &= WriteUByte(10);
		return success;
	}
}

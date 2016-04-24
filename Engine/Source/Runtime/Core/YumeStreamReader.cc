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
#include "YumeStreamReader.h"



namespace YumeEngine
{

	static const float invQ = 1.0f / 32767.0f;

	StreamReader::StreamReader():
		position_(0),
		size_(0)
	{
	}

	StreamReader::StreamReader(unsigned size):
		position_(0),
		size_(size)
	{
	}

	StreamReader::~StreamReader()
	{
	}

	const YumeString& StreamReader::GetName() const
	{
		return EmptyString;
	}

	unsigned StreamReader::GetChecksum()
	{
		return 0;
	}

	int StreamReader::ReadInt()
	{
		int ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	short StreamReader::ReadShort()
	{
		short ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	signed char StreamReader::ReadByte()
	{
		signed char ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned StreamReader::ReadUInt()
	{
		unsigned ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned short StreamReader::ReadUShort()
	{
		unsigned short ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	unsigned char StreamReader::ReadUByte()
	{
		unsigned char ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	bool StreamReader::ReadBool()
	{
		return ReadUByte() != 0;
	}

	float StreamReader::ReadFloat()
	{
		float ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	double StreamReader::ReadDouble()
	{
		double ret;
		Read(&ret,sizeof ret);
		return ret;
	}

	IntRect StreamReader::ReadIntRect()
	{
		int data[4];
		Read(data,sizeof data);
		return IntRect(data);
	}

	IntVector2 StreamReader::ReadIntVector2()
	{
		int data[2];
		Read(data,sizeof data);
		return IntVector2(data);
	}

	Rect StreamReader::ReadRect()
	{
		float data[4];
		Read(data,sizeof data);
		return Rect(data);
	}

	Vector2 StreamReader::ReadVector2()
	{
		float data[2];
		Read(data,sizeof data);
		return Vector2(data);
	}

	Vector3 StreamReader::ReadVector3()
	{
		float data[3];
		Read(data,sizeof data);
		return Vector3(data);
	}

	Vector3 StreamReader::ReadPackedVector3(float maxAbsCoord)
	{
		float invV = maxAbsCoord / 32767.0f;
		short coords[3];
		Read(coords,sizeof coords);
		Vector3 ret(coords[0] * invV,coords[1] * invV,coords[2] * invV);
		return ret;
	}

	Vector4 StreamReader::ReadVector4()
	{
		float data[4];
		Read(data,sizeof data);
		return Vector4(data);
	}

	Quaternion StreamReader::ReadQuaternion()
	{
		float data[4];
		Read(data,sizeof data);
		return Quaternion(data);
	}

	Quaternion StreamReader::ReadPackedQuaternion()
	{
		short coords[4];
		Read(coords,sizeof coords);
		Quaternion ret(coords[0] * invQ,coords[1] * invQ,coords[2] * invQ,coords[3] * invQ);
		ret.Normalize();
		return ret;
	}

	Matrix3 StreamReader::ReadMatrix3()
	{
		float data[9];
		Read(data,sizeof data);
		return Matrix3(data);
	}

	Matrix3x4 StreamReader::ReadMatrix3x4()
	{
		float data[12];
		Read(data,sizeof data);
		return Matrix3x4(data);
	}

	Matrix4 StreamReader::ReadMatrix4()
	{
		float data[16];
		Read(data,sizeof data);
		return Matrix4(data);
	}

	YumeColor StreamReader::ReadColor()
	{
		float data[4];
		Read(data,sizeof data);
		return YumeColor(data);
	}

	BoundingBox StreamReader::ReadBoundingBox()
	{
		float data[6];
		Read(data,sizeof data);
		return BoundingBox(Vector3(&data[0]),Vector3(&data[3]));
	}

	YumeString StreamReader::ReadString()
	{
		YumeString ret;

		while(!IsEof())
		{
			char c = ReadByte();
			if(!c)
				break;
			else
				ret += c;
		}

		return ret;
	}

	YumeString StreamReader::ReadFileID()
	{
		YumeString ret;
		ret.resize(4);
		Read(&ret[0],4);
		return ret;
	}

	YumeHash StreamReader::ReadStringHash()
	{
		return YumeHash(ReadUInt());
	}

	YumeVector<unsigned char>::type StreamReader::ReadBuffer()
	{
		YumeVector<unsigned char>::type ret(ReadVLE());
		if(ret.size())
			Read(&ret[0],ret.size());
		return ret;
	}

	ResourceRef StreamReader::ReadResourceRef()
	{
		ResourceRef ret;
		ret.type_ = ReadStringHash();
		ret.name_ = ReadString();
		return ret;
	}

	ResourceRefList StreamReader::ReadResourceRefList()
	{
		ResourceRefList ret;
		ret.type_ = ReadStringHash();
		ret.names_.resize(ReadVLE());
		for(unsigned i = 0; i < ret.names_.size(); ++i)
			ret.names_[i] = ReadString();
		return ret;
	}

	Variant StreamReader::ReadVariant()
	{
		VariantType type = (VariantType)ReadUByte();
		return ReadVariant(type);
	}

	Variant StreamReader::ReadVariant(VariantType type)
	{
		switch(type)
		{
		case VAR_INT:
			return Variant(ReadInt());

		case VAR_BOOL:
			return Variant(ReadBool());

		case VAR_FLOAT:
			return Variant(ReadFloat());

		case VAR_VECTOR2:
			return Variant(ReadVector2());

		case VAR_QUATERNION:
			return Variant(ReadQuaternion());

		case VAR_COLOR:
			return Variant(ReadColor());

		case VAR_STRING:
			return Variant(ReadString());

		case VAR_BUFFER:
			return Variant(ReadBuffer());

			// Deserializing pointers is not supported. Return null
		case VAR_VOIDPTR:
		case VAR_PTR:
			ReadUInt();
			return Variant((void*)0);

		case VAR_RESOURCEREF:
			return Variant(ReadResourceRef());

		case VAR_RESOURCEREFLIST:
			return Variant(ReadResourceRefList());

		case VAR_VARIANTVECTOR:
			return Variant(ReadVariantVector());

		case VAR_STRINGVECTOR:
			return Variant(ReadStringVector());

		case VAR_VARIANTMAP:
			return Variant(ReadVariantMap());

		case VAR_INTRECT:
			return Variant(ReadIntRect());

		case VAR_INTVECTOR2:
			return Variant(ReadIntVector2());

		case VAR_MATRIX3:
			return Variant(ReadMatrix3());

		case VAR_MATRIX3X4:
			return Variant(ReadMatrix3x4());

		case VAR_MATRIX4:
			return Variant(ReadMatrix4());

		case VAR_DOUBLE:
			return Variant(ReadDouble());

		default:
			return Variant();
		}
	}

	VariantVector::type StreamReader::ReadVariantVector()
	{
		VariantVector::type ret(ReadVLE());
		for(unsigned i = 0; i < ret.size(); ++i)
			ret[i] = ReadVariant();
		return ret;
	}

	StringVector::type StreamReader::ReadStringVector()
	{
		StringVector::type ret(ReadVLE());
		for(unsigned i = 0; i < ret.size(); ++i)
			ret[i] = ReadString();
		return ret;
	}

	VariantMap::type StreamReader::ReadVariantMap()
	{
		VariantMap::type ret;
		unsigned num = ReadVLE();

		for(unsigned i = 0; i < num; ++i)
		{
			YumeHash key = ReadStringHash();
			ret[key] = ReadVariant();
		}

		return ret;
	}

	unsigned StreamReader::ReadVLE()
	{
		unsigned ret;
		unsigned char byte;

		byte = ReadUByte();
		ret = (unsigned)(byte & 0x7f);
		if(byte < 0x80)
			return ret;

		byte = ReadUByte();
		ret |= ((unsigned)(byte & 0x7f)) << 7;
		if(byte < 0x80)
			return ret;

		byte = ReadUByte();
		ret |= ((unsigned)(byte & 0x7f)) << 14;
		if(byte < 0x80)
			return ret;

		byte = ReadUByte();
		ret |= ((unsigned)byte) << 21;
		return ret;
	}

	unsigned StreamReader::ReadNetID()
	{
		unsigned ret = 0;
		Read(&ret,3);
		return ret;
	}

	YumeString StreamReader::ReadLine()
	{
		YumeString ret;

		while(!IsEof())
		{
			char c = ReadByte();
			if(c == 10)
				break;
			if(c == 13)
			{
				// Peek next char to see if it's 10, and skip it too
				if(!IsEof())
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

}

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
#ifndef __YumeStreamWriter_h__
#define __YumeStreamWriter_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVariant.h"
#include "Math/YumeBoundingBox.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeColor;
	class IntRect;
	class IntVector2;
	class Quaternion;
	class Rect;
	class Vector2;
	class Vector3;
	class Vector4;

	class YumeAPIExport StreamWriter
	{
	public:
		/// Destruct.
		virtual ~StreamWriter();

		/// Write bytes to the stream. Return number of bytes actually written.
		virtual unsigned Write(const void* data,unsigned size) = 0;

		/// Write a 32-bit integer.
		bool WriteInt(int value);
		/// Write a 16-bit integer.
		bool WriteShort(short value);
		/// Write an 8-bit integer.
		bool WriteByte(signed char value);
		/// Write a 32-bit unsigned integer.
		bool WriteUInt(unsigned value);
		/// Write a 16-bit unsigned integer.
		bool WriteUShort(unsigned short value);
		/// Write an 8-bit unsigned integer.
		bool WriteUByte(unsigned char value);
		/// Write a bool.
		bool WriteBool(bool value);
		/// Write a float.
		bool WriteFloat(float value);
		/// Write a double.
		bool WriteDouble(double value);
		/// Write an IntRect.
		bool WriteIntRect(const IntRect& value);
		/// Write an IntVector2.
		bool WriteIntVector2(const IntVector2& value);
		/// Write a Rect.
		bool WriteRect(const Rect& value);
		/// Write a Vector2.
		bool WriteVector2(const Vector2& value);
		/// Write a Vector3.
		bool WriteVector3(const Vector3& value);
		/// Write a Vector3 packed into 3 x 16 bits with the specified maximum absolute range.
		bool WritePackedVector3(const Vector3& value,float maxAbsCoord);
		/// Write a Vector4.
		bool WriteVector4(const Vector4& value);
		/// Write a quaternion.
		bool WriteQuaternion(const Quaternion& value);
		/// Write a quaternion with each component packed in 16 bits.
		bool WritePackedQuaternion(const Quaternion& value);
		/// Write a Matrix3.
		bool WriteMatrix3(const Matrix3& value);
		/// Write a Matrix3x4.
		bool WriteMatrix3x4(const Matrix3x4& value);
		/// Write a Matrix4.
		bool WriteMatrix4(const Matrix4& value);
		/// Write a color.
		bool WriteColor(const YumeColor& value);
		/// Write a bounding box.
		bool WriteBoundingBox(const BoundingBox& value);
		/// Write a null-terminated string.
		bool WriteString(const YumeString& value);
		/// Write a four-letter file ID. If the string is not long enough, spaces will be appended.
		bool WriteFileID(const YumeString& value);
		/// Write a 32-bit StringHash.
		bool WriteStringHash(const YumeHash& value);
		/// Write a buffer, with size encoded as VLE.
		bool WriteBuffer(const YumeVector<unsigned char>::type& buffer);
		/// Write a resource reference.
		bool WriteResourceRef(const ResourceRef& value);
		/// Write a resource reference list.
		bool WriteResourceRefList(const ResourceRefList& value);
		/// Write a variant.
		bool WriteVariant(const Variant& value);
		/// Write a variant without the type information.
		bool WriteVariantData(const Variant& value);
		/// Write a variant vector.
		bool WriteVariantVector(const VariantVector& value);
		/// Write a variant vector.
		bool WriteStringVector(const StringVector& value);
		/// Write a variant map.
		bool WriteVariantMap(const VariantMap& value);
		/// Write a variable-length encoded unsigned integer, which can use 29 bits maximum.
		bool WriteVLE(unsigned value);
		/// Write a 24-bit network object ID.
		bool WriteNetID(unsigned value);
		/// Write a text line. Char codes 13 & 10 will be automatically appended.
		bool WriteLine(const YumeString& value);
	};
}


//----------------------------------------------------------------------------
#endif

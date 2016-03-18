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
#ifndef __YumeStreamReader_h__
#define __YumeStreamReader_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVariant.h"
#include "Math/YumeBoundingBox.h"
#include "Math/YumeRect.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport StreamReader
	{
	public:
		/// Construct with zero size.
		StreamReader();
		/// Construct with defined size.
		StreamReader(unsigned size);
		/// Destruct.
		virtual ~StreamReader();

		/// Read bytes from the stream. Return number of bytes actually read.
		virtual unsigned Read(void* dest,unsigned size) = 0;
		/// Set position from the beginning of the stream.
		virtual unsigned Seek(unsigned position) = 0;
		/// Return name of the stream.
		virtual const YumeString& GetName() const;
		/// Return a checksum if applicable.
		virtual unsigned GetChecksum();

		/// Return current position.
		unsigned GetPosition() const { return position_; }

		/// Return size.
		unsigned GetSize() const { return size_; }

		/// Return whether the end of stream has been reached.
		bool IsEof() const { return position_ >= size_; }

		/// Read a 32-bit integer.
		int ReadInt();
		/// Read a 16-bit integer.
		short ReadShort();
		/// Read an 8-bit integer.
		signed char ReadByte();
		/// Read a 32-bit unsigned integer.
		unsigned ReadUInt();
		/// Read a 16-bit unsigned integer.
		unsigned short ReadUShort();
		/// Read an 8-bit unsigned integer.
		unsigned char ReadUByte();
		/// Read a bool.
		bool ReadBool();
		/// Read a float.
		float ReadFloat();
		/// Read a double.
		double ReadDouble();
		/// Read an IntRect.
		IntRect ReadIntRect();
		/// Read an IntVector2.
		IntVector2 ReadIntVector2();
		/// Read a Rect.
		Rect ReadRect();
		/// Read a Vector2.
		Vector2 ReadVector2();
		/// Read a Vector3.
		Vector3 ReadVector3();
		/// Read a Vector3 packed into 3 x 16 bits with the specified maximum absolute range.
		Vector3 ReadPackedVector3(float maxAbsCoord);
		/// Read a Vector4.
		Vector4 ReadVector4();
		/// Read a quaternion.
		Quaternion ReadQuaternion();
		/// Read a quaternion with each component packed in 16 bits.
		Quaternion ReadPackedQuaternion();
		/// Read a Matrix3.
		Matrix3 ReadMatrix3();
		/// Read a Matrix3x4.
		Matrix3x4 ReadMatrix3x4();
		/// Read a Matrix4.
		Matrix4 ReadMatrix4();
		/// Read a color.
		YumeColor ReadColor();
		/// Read a bounding box.
		BoundingBox ReadBoundingBox();
		/// Read a null-terminated string.
		YumeString ReadString();
		/// Read a four-letter file ID.
		YumeString ReadFileID();
		/// Read a 32-bit StringHash.
		YumeHash ReadStringHash();
		/// Read a buffer with size encoded as VLE.
		YumeVector<unsigned char>::type ReadBuffer();
		/// Read a resource reference.
		ResourceRef ReadResourceRef();
		/// Read a resource reference list.
		ResourceRefList ReadResourceRefList();
		/// Read a variant.
		Variant ReadVariant();
		/// Read a variant whose type is already known.
		Variant ReadVariant(VariantType type);
		/// Read a variant vector.
		VariantVector ReadVariantVector();
		/// Read a string vector.
		StringVector ReadStringVector();
		/// Read a variant map.
		VariantMap ReadVariantMap();
		/// Read a variable-length encoded unsigned integer, which can use 29 bits maximum.
		unsigned ReadVLE();
		/// Read a 24-bit network object ID.
		unsigned ReadNetID();
		/// Read a text line.
		YumeString ReadLine();

	protected:
		/// Stream position.
		unsigned position_;
		/// Stream size.
		unsigned size_;
	};
}


//----------------------------------------------------------------------------
#endif

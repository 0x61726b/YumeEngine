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
#ifndef __Variant_h__
#define __Variant_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Math/YumeColor.h"
#include "Math/YumeMatrix3.h"
#include "Math/YumeMatrix3x4.h"
#include "Math/YumeRect.h"
#include "Math/YumeHash.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Variant's supported types.
	enum VariantType
	{
		VAR_EMPTY = 0,
		VAR_INT,
		VAR_BOOL,
		VAR_FLOAT,
		VAR_VECTOR2,
		VAR_VECTOR3,
		VAR_VECTOR4,
		VAR_QUATERNION,
		VAR_COLOR,
		VAR_STRING,
		VAR_BUFFER,
		VAR_VOIDPTR,
		VAR_RESOURCEREF,
		VAR_RESOURCEREFLIST,
		VAR_VARIANTVECTOR,
		VAR_VARIANTMAP,
		VAR_INTRECT,
		VAR_INTVECTOR2,
		VAR_PTR,
		VAR_MATRIX3,
		VAR_MATRIX3X4,
		VAR_MATRIX4,
		VAR_DOUBLE,
		VAR_STRINGVECTOR,
		MAX_VAR_TYPES
	};

	/// Union for the possible variant values. Also stores non-POD objects such as String and math objects (excluding Matrix) which must not exceed 16 bytes in size. Objects exceeding 16 bytes size are stored in the heap pointed by _ptr.
	struct VariantValue
	{
		union
		{
			int int_;
			bool bool_;
			float float_;
			void* ptr_;
		};

		union
		{
			int int2_;
			float float2_;
			void* ptr2_;
		};

		union
		{
			int int3_;
			float float3_;
			void* ptr3_;
		};

		union
		{
			int int4_;
			float float4_;
			void* ptr4_;
		};
	};

	class Variant;
	class VectorBuffer;

	/// Vector of variants.
	typedef YumeVector<Variant>::type VariantVector;


	/// Map of variants.
	typedef YumeMap<YumeHash,Variant>::type VariantMap;

	/// Typed resource reference.
	struct YumeAPIExport ResourceRef
	{
		/// Construct.
		ResourceRef()
		{
		}

		/// Construct with type only and empty id.
		ResourceRef(YumeHash type):
			type_(type)
		{
		}

		/// Construct with type and resource name.
		ResourceRef(YumeHash type,const YumeString& name):
			type_(type),
			name_(name)
		{
		}

		// Construct from another ResourceRef.
		ResourceRef(const ResourceRef& rhs):
			type_(rhs.type_),
			name_(rhs.name_)
		{
		}

		/// Object type.
		YumeHash type_;
		/// Object name.
		YumeString name_;

		/// Test for equality with another reference.
		bool operator ==(const ResourceRef& rhs) const { return type_ == rhs.type_ && name_ == rhs.name_; }

		/// Test for inequality with another reference.
		bool operator !=(const ResourceRef& rhs) const { return type_ != rhs.type_ || name_ != rhs.name_; }
	};

	/// %List of typed resource references.
	struct YumeAPIExport ResourceRefList
	{
		/// Construct.
		ResourceRefList()
		{
		}

		/// Construct with type only.
		ResourceRefList(YumeHash type):
			type_(type)
		{
		}

		/// Construct with type and id list.
		ResourceRefList(YumeHash type,const StringVector& names):
			type_(type),
			names_(names)
		{
		}

		/// Object type.
		YumeHash type_;
		/// List of object names.
		StringVector names_;

		/// Test for equality with another reference list.
		bool operator ==(const ResourceRefList& rhs) const { return type_ == rhs.type_ && names_ == rhs.names_; }

		/// Test for inequality with another reference list.
		bool operator !=(const ResourceRefList& rhs) const { return type_ != rhs.type_ || names_ != rhs.names_; }
	};

	/// Variable that supports a fixed set of types.
	class YumeAPIExport Variant
	{
	public:
		/// Construct empty.
		Variant():
			type_(VAR_EMPTY)
		{
		}

		/// Construct from integer.
		Variant(int value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from unsigned integer.
		Variant(unsigned value):
			type_(VAR_EMPTY)
		{
			*this = (int)value;
		}

		/// Construct from a string hash (convert to integer).
		Variant(const YumeHash& value):
			type_(VAR_EMPTY)
		{
			*this = (int)value;
		}

		/// Construct from a bool.
		Variant(bool value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a float.
		Variant(float value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a double.
		Variant(double value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Vector2.
		Variant(const Vector2& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Vector3.
		Variant(const Vector3& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Vector4.
		Variant(const Vector4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a quaternion.
		Variant(const Quaternion& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a color.
		Variant(const YumeColor& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a string.
		Variant(const YumeString& value):
			type_(VAR_EMPTY)
		{
			stringHack_ = value;
		}

		/// Construct from a C string.
		Variant(const char* value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a buffer.
		Variant(const YumeVector<unsigned char>::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a %VectorBuffer and store as a buffer.
		Variant(const VectorBuffer& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a pointer.
		Variant(void* value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a resource reference.
		Variant(const ResourceRef& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a resource reference list.
		Variant(const ResourceRefList& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		Variant& operator =(const ResourceRefList& rhs)
		{
			SetType(VAR_RESOURCEREFLIST);
			*(reinterpret_cast<ResourceRefList*>(&value_)) = rhs;
			return *this;
		}

		/// Construct from a variant vector.
		Variant(const VariantVector& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a variant map.
		Variant(const VariantMap& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a string vector.
		Variant(const StringVector& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from an integer rect.
		Variant(const IntRect& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from an IntVector2.
		Variant(const IntVector2& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Matrix3.
		Variant(const Matrix3& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Matrix3x4.
		Variant(const Matrix3x4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from a Matrix4.
		Variant(const Matrix4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Construct from type and value.
		Variant(const YumeString& type,const YumeString& value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}

		/// Construct from type and value.
		Variant(VariantType type,const YumeString& value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}

		/// Construct from type and value.
		Variant(const char* type,const char* value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}

		/// Construct from type and value.
		Variant(VariantType type,const char* value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}

		/// Copy-construct from another variant.
		Variant(const Variant& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		/// Destruct.
		~Variant()
		{
			if(type_ != VAR_STRING)
				SetType(VAR_EMPTY);
		}

		/// Reset to empty.
		void Clear()
		{
			SetType(VAR_EMPTY);
		}

		/// Assign from another variant.
		Variant& operator =(const Variant& rhs);

		/// Assign from an integer.
		Variant& operator =(int rhs)
		{
			SetType(VAR_INT);
			value_.int_ = rhs;
			return *this;
		}

		/// Assign from an unsigned integer.
		Variant& operator =(unsigned rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs;
			return *this;
		}

		/// Assign from a YumeHash (convert to integer.)
		Variant& operator =(const YumeHash& rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs;
			return *this;
		}

		/// Assign from a bool.
		Variant& operator =(bool rhs)
		{
			SetType(VAR_BOOL);
			value_.bool_ = rhs;
			return *this;
		}

		Variant& operator =(const ResourceRef& rhs)
		{
			SetType(VAR_RESOURCEREF);
			*(reinterpret_cast<ResourceRef*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a float.
		Variant& operator =(float rhs)
		{
			SetType(VAR_FLOAT);
			value_.float_ = rhs;
			return *this;
		}

		/// Assign from a double.
		Variant& operator = (double rhs)
		{
			SetType(VAR_DOUBLE);
			*(reinterpret_cast<double*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a Vector2.
		Variant& operator =(const Vector2& rhs)
		{
			SetType(VAR_VECTOR2);
			*(reinterpret_cast<Vector2*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a Vector3.
		Variant& operator =(const Vector3& rhs)
		{
			SetType(VAR_VECTOR3);
			*(reinterpret_cast<Vector3*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a Vector4.
		Variant& operator =(const Vector4& rhs)
		{
			SetType(VAR_VECTOR4);
			*(reinterpret_cast<Vector4*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a quaternion.
		Variant& operator =(const Quaternion& rhs)
		{
			SetType(VAR_QUATERNION);
			*(reinterpret_cast<Quaternion*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a color.
		Variant& operator =(const YumeColor& rhs)
		{
			SetType(VAR_COLOR);
			*(reinterpret_cast<YumeColor*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a string.
		Variant& operator =(const YumeString& rhs)
		{
			SetType(VAR_STRING);
			stringHack_ = rhs;
			return *this;
		}

		/// Assign from a C string.
		Variant& operator =(const char* rhs)
		{
			SetType(VAR_STRING);
			stringHack_ = YumeString(rhs);
			return *this;
		}

		/// Assign from a buffer.
		Variant& operator =(const YumeVector<unsigned char>::type& rhs)
		{
			SetType(VAR_BUFFER);
			*(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_)) = rhs;
			return *this;
		}

		Variant& operator =(const VectorBuffer& rhs);

		/// Assign from a void pointer.
		Variant& operator =(void* rhs)
		{
			SetType(VAR_VOIDPTR);
			value_.ptr_ = rhs;
			return *this;
		}

		/// Assign from a variant vector.
		Variant& operator =(const VariantVector& rhs)
		{
			SetType(VAR_VARIANTVECTOR);
			*(reinterpret_cast<VariantVector*>(&value_)) = rhs;
			return *this;
		}

		// Assign from a string vector.
		Variant& operator =(const StringVector& rhs)
		{
			SetType(VAR_STRINGVECTOR);
			*(reinterpret_cast<StringVector*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a variant map.
		Variant& operator =(const VariantMap& rhs)
		{
			SetType(VAR_VARIANTMAP);
			*(reinterpret_cast<VariantMap*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from an integer rect.
		Variant& operator =(const IntRect& rhs)
		{
			SetType(VAR_INTRECT);
			*(reinterpret_cast<IntRect*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from an IntVector2.
		Variant& operator =(const IntVector2& rhs)
		{
			SetType(VAR_INTVECTOR2);
			*(reinterpret_cast<IntVector2*>(&value_)) = rhs;
			return *this;
		}

		/// Assign from a Matrix3.
		Variant& operator =(const Matrix3& rhs)
		{
			SetType(VAR_MATRIX3);
			*(reinterpret_cast<Matrix3*>(value_.ptr_)) = rhs;
			return *this;
		}

		/// Assign from a Matrix3x4.
		Variant& operator =(const Matrix3x4& rhs)
		{
			SetType(VAR_MATRIX3X4);
			*(reinterpret_cast<Matrix3x4*>(value_.ptr_)) = rhs;
			return *this;
		}

		/// Assign from a Matrix4.
		Variant& operator =(const Matrix4& rhs)
		{
			SetType(VAR_MATRIX4);
			*(reinterpret_cast<Matrix4*>(value_.ptr_)) = rhs;
			return *this;
		}

		/// Test for equality with another variant.
		bool operator ==(const Variant& rhs) const;

		/// Test for equality with an integer. To return true, both the type and value must match.
		bool operator ==(int rhs) const { return type_ == VAR_INT ? value_.int_ == rhs : false; }

		/// Test for equality with an unsigned integer. To return true, both the type and value must match.
		bool operator ==(unsigned rhs) const { return type_ == VAR_INT ? value_.int_ == (int)rhs : false; }

		/// Test for equality with a bool. To return true, both the type and value must match.
		bool operator ==(bool rhs) const { return type_ == VAR_BOOL ? value_.bool_ == rhs : false; }

		/// Test for equality with a float. To return true, both the type and value must match.
		bool operator ==(float rhs) const { return type_ == VAR_FLOAT ? value_.float_ == rhs : false; }

		/// Test for equality with a double. To return true, both the type and value must match.
		bool operator ==(double rhs) const { return type_ == VAR_DOUBLE ? *(reinterpret_cast<const double*>(&value_)) == rhs : false; }

		/// Test for equality with a Vector2. To return true, both the type and value must match.
		bool operator ==(const Vector2& rhs) const
		{
			return type_ == VAR_VECTOR2 ? *(reinterpret_cast<const Vector2*>(&value_)) == rhs : false;
		}

		/// Test for equality with a Vector3. To return true, both the type and value must match.
		bool operator ==(const Vector3& rhs) const
		{
			return type_ == VAR_VECTOR3 ? *(reinterpret_cast<const Vector3*>(&value_)) == rhs : false;
		}

		/// Test for equality with a Vector4. To return true, both the type and value must match.
		bool operator ==(const Vector4& rhs) const
		{
			return type_ == VAR_VECTOR4 ? *(reinterpret_cast<const Vector4*>(&value_)) == rhs : false;
		}

		/// Test for equality with a quaternion. To return true, both the type and value must match.
		bool operator ==(const Quaternion& rhs) const
		{
			return type_ == VAR_QUATERNION ? *(reinterpret_cast<const Quaternion*>(&value_)) == rhs : false;
		}

		/// Test for equality with a color. To return true, both the type and value must match.
		bool operator ==(const YumeColor& rhs) const
		{
			return type_ == VAR_COLOR ? *(reinterpret_cast<const YumeColor*>(&value_)) == rhs : false;
		}

		/// Test for equality with a string. To return true, both the type and value must match.
		bool operator ==(const YumeString& rhs) const
		{
			return type_ == VAR_STRING ? *(reinterpret_cast<const YumeString*>(&value_)) == rhs : false;
		}

		/// Test for equality with a buffer. To return true, both the type and value must match.
		bool operator ==(const YumeVector<unsigned char>::type& rhs) const;
		/// Test for equality with a %VectorBuffer. To return true, both the type and value must match.
		bool operator ==(const VectorBuffer& rhs) const;


		/// Test for equality with a resource reference. To return true, both the type and value must match.
		bool operator ==(const ResourceRef& rhs) const
		{
			return type_ == VAR_RESOURCEREF ? *(reinterpret_cast<const ResourceRef*>(&value_)) == rhs : false;
		}

		/// Test for equality with a resource reference list. To return true, both the type and value must match.
		bool operator ==(const ResourceRefList& rhs) const
		{
			return type_ == VAR_RESOURCEREFLIST ? *(reinterpret_cast<const ResourceRefList*>(&value_)) == rhs : false;
		}

		/// Test for equality with a variant vector. To return true, both the type and value must match.
		bool operator ==(const VariantVector& rhs) const
		{
			return type_ == VAR_VARIANTVECTOR ? *(reinterpret_cast<const VariantVector*>(&value_)) == rhs : false;
		}

		/// Test for equality with a string vector. To return true, both the type and value must match.
		bool operator ==(const StringVector& rhs) const
		{
			return type_ == VAR_STRINGVECTOR ? *(reinterpret_cast<const StringVector*>(&value_)) == rhs : false;
		}

		/// Test for equality with a variant map. To return true, both the type and value must match.
		bool operator ==(const VariantMap& rhs) const
		{
			return type_ == VAR_VARIANTMAP ? *(reinterpret_cast<const VariantMap*>(&value_)) == rhs : false;
		}

		/// Test for equality with an integer rect. To return true, both the type and value must match.
		bool operator ==(const IntRect& rhs) const
		{
			return type_ == VAR_INTRECT ? *(reinterpret_cast<const IntRect*>(&value_)) == rhs : false;
		}

		/// Test for equality with an IntVector2. To return true, both the type and value must match.
		bool operator ==(const IntVector2& rhs) const
		{
			return type_ == VAR_INTVECTOR2 ? *(reinterpret_cast<const IntVector2*>(&value_)) == rhs : false;
		}

		/// Test for equality with a YumeHash. To return true, both the type and value must match.
		bool operator ==(const YumeHash& rhs) const { return type_ == VAR_INT ? (unsigned)value_.int_ == rhs : false; }


		/// Test for equality with a Matrix3. To return true, both the type and value must match.
		bool operator ==(const Matrix3& rhs) const
		{
			return type_ == VAR_MATRIX3 ? *(reinterpret_cast<const Matrix3*>(value_.ptr_)) == rhs : false;
		}

		/// Test for equality with a Matrix3x4. To return true, both the type and value must match.
		bool operator ==(const Matrix3x4& rhs) const
		{
			return type_ == VAR_MATRIX3X4 ? *(reinterpret_cast<const Matrix3x4*>(value_.ptr_)) == rhs : false;
		}

		/// Test for equality with a Matrix4. To return true, both the type and value must match.
		bool operator ==(const Matrix4& rhs) const
		{
			return type_ == VAR_MATRIX4 ? *(reinterpret_cast<const Matrix4*>(value_.ptr_)) == rhs : false;
		}

		/// Test for inequality with another variant.
		bool operator !=(const Variant& rhs) const { return !(*this == rhs); }

		/// Test for inequality with an integer.
		bool operator !=(int rhs) const { return !(*this == rhs); }

		/// Test for inequality with an unsigned integer.
		bool operator !=(unsigned rhs) const { return !(*this == rhs); }

		/// Test for inequality with a bool.
		bool operator !=(bool rhs) const { return !(*this == rhs); }

		/// Test for inequality with a float.
		bool operator !=(float rhs) const { return !(*this == rhs); }

		/// Test for inequality with a double.
		bool operator !=(double rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Vector2.
		bool operator !=(const Vector2& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Vector3.
		bool operator !=(const Vector3& rhs) const { return !(*this == rhs); }

		/// Test for inequality with an Vector4.
		bool operator !=(const Vector4& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Quaternion.
		bool operator !=(const Quaternion& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a string.
		bool operator !=(const YumeString& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a buffer.
		bool operator !=(const YumeVector<unsigned char>::type& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a %VectorBuffer.
		bool operator !=(const VectorBuffer& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a pointer.
		bool operator !=(void* rhs) const { return !(*this == rhs); }

		/// Test for inequality with a resource reference.
		bool operator !=(const ResourceRef& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a resource reference list.
		bool operator !=(const ResourceRefList& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a variant vector.
		bool operator !=(const VariantVector& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a string vector.
		bool operator !=(const StringVector& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a variant map.
		bool operator !=(const VariantMap& rhs) const { return !(*this == rhs); }

		/// Test for inequality with an integer rect.
		bool operator !=(const IntRect& rhs) const { return !(*this == rhs); }

		/// Test for inequality with an IntVector2.
		bool operator !=(const IntVector2& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a YumeHash.
		bool operator !=(const YumeHash& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Matrix3.
		bool operator !=(const Matrix3& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Matrix3x4.
		bool operator !=(const Matrix3x4& rhs) const { return !(*this == rhs); }

		/// Test for inequality with a Matrix4.
		bool operator !=(const Matrix4& rhs) const { return !(*this == rhs); }

		/// Set from typename and value strings. Pointers will be set to null, and VariantBuffer or VariantMap types are not supported.
		void FromString(const YumeString& type,const YumeString& value);
		/// Set from typename and value strings. Pointers will be set to null, and VariantBuffer or VariantMap types are not supported.
		void FromString(const char* type,const char* value);
		/// Set from type and value string. Pointers will be set to null, and VariantBuffer or VariantMap types are not supported.
		void FromString(VariantType type,const YumeString& value);
		/// Set from type and value string. Pointers will be set to null, and VariantBuffer or VariantMap types are not supported.
		void FromString(VariantType type,const char* value);
		/// Set buffer type from a memory area.
		void SetBuffer(const void* data,unsigned size);

		/// Return int or zero on type mismatch. Floats and doubles are converted.
		int GetInt() const
		{
			if(type_ == VAR_INT)
				return value_.int_;
			else if(type_ == VAR_FLOAT)
				return (int)value_.float_;
			else if(type_ == VAR_DOUBLE)
				return (int)*reinterpret_cast<const double*>(&value_);
			else
				return 0;
		}

		/// Return unsigned int or zero on type mismatch. Floats and doubles are converted.
		unsigned GetUInt() const
		{
			if(type_ == VAR_INT)
				return value_.int_;
			else if(type_ == VAR_FLOAT)
				return (unsigned)value_.float_;
			else if(type_ == VAR_DOUBLE)
				return (unsigned)*reinterpret_cast<const double*>(&value_);
			else
				return 0;
		}

		/// Return YumeHash or zero on type mismatch.
		YumeHash GetYumeHash() const { return YumeHash(GetUInt()); }

		/// Return bool or false on type mismatch.
		bool GetBool() const { return type_ == VAR_BOOL ? value_.bool_ : false; }

		/// Return float or zero on type mismatch. Ints and doubles are converted.
		float GetFloat() const
		{
			if(type_ == VAR_FLOAT)
				return value_.float_;
			else if(type_ == VAR_DOUBLE)
				return (float)*reinterpret_cast<const double*>(&value_);
			else if(type_ == VAR_INT)
				return (float)value_.int_;
			else
				return 0.0f;
		}

		void* GetVoidPtr() const
		{
			if(type_ == VAR_VOIDPTR)
				return value_.ptr_;
			else
				return 0;
		}


		/// Return double or zero on type mismatch. Ints and floats are converted.
		double GetDouble() const
		{
			if(type_ == VAR_DOUBLE)
				return *reinterpret_cast<const double*>(&value_);
			else if(type_ == VAR_FLOAT)
				return (double)value_.float_;
			else if(type_ == VAR_INT)
				return (double)value_.int_;
			else
				return 0.0;
		}

		/// Return Vector2 or zero on type mismatch.
		const Vector2& GetVector2() const { return type_ == VAR_VECTOR2 ? *reinterpret_cast<const Vector2*>(&value_) : Vector2::ZERO; }

		/// Return Vector3 or zero on type mismatch.
		const Vector3& GetVector3() const { return type_ == VAR_VECTOR3 ? *reinterpret_cast<const Vector3*>(&value_) : Vector3::ZERO; }

		/// Return Vector4 or zero on type mismatch.
		const Vector4& GetVector4() const { return type_ == VAR_VECTOR4 ? *reinterpret_cast<const Vector4*>(&value_) : Vector4::ZERO; }

		/// Return quaternion or identity on type mismatch.
		const Quaternion& GetQuaternion() const
		{
			return type_ == VAR_QUATERNION ? *reinterpret_cast<const Quaternion*>(&value_) : Quaternion::IDENTITY;
		}

		/// Return color or default on type mismatch.
		const YumeColor& GetColor() const { return type_ == VAR_COLOR ? *reinterpret_cast<const YumeColor*>(&value_) : YumeColor::WHITE; }

		/// Return string or empty on type mismatch.
		const YumeString& GetString() const { return type_ == VAR_STRING ? stringHack_ : EmptyString; }

		/// Return buffer or empty on type mismatch.
		const YumeVector<unsigned char>::type& GetBuffer() const
		{
			return type_ == VAR_BUFFER ? *reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_) : emptyBuffer;
		}

		/// Return %VectorBuffer containing the buffer or empty on type mismatch.
		const VectorBuffer GetVectorBuffer() const;

		/// Return a resource reference or empty on type mismatch.
		const ResourceRef& GetResourceRef() const
		{
			return type_ == VAR_RESOURCEREF ? *reinterpret_cast<const ResourceRef*>(&value_) : emptyResourceRef;
		}

		/// Return a resource reference list or empty on type mismatch.
		const ResourceRefList& GetResourceRefList() const
		{
			return type_ == VAR_RESOURCEREFLIST ? *reinterpret_cast<const ResourceRefList*>(&value_) : emptyResourceRefList;
		}

		/// Return a variant vector or empty on type mismatch.
		const VariantVector& GetVariantVector() const
		{
			return type_ == VAR_VARIANTVECTOR ? *reinterpret_cast<const VariantVector*>(&value_) : emptyVariantVector;
		}

		/// Return a string vector or empty on type mismatch.
		const StringVector& GetStringVector() const
		{
			return type_ == VAR_STRINGVECTOR ? *reinterpret_cast<const StringVector*>(&value_) : emptyStringVector;
		}

		/// Return a variant map or empty on type mismatch.
		const VariantMap& GetVariantMap() const
		{
			return type_ == VAR_VARIANTMAP ? *reinterpret_cast<const VariantMap*>(&value_) : emptyVariantMap;
		}

		/// Return an integer rect or empty on type mismatch.
		const IntRect& GetIntRect() const { return type_ == VAR_INTRECT ? *reinterpret_cast<const IntRect*>(&value_) : IntRect::ZERO; }

		/// Return an IntVector2 or empty on type mismatch.
		const IntVector2& GetIntVector2() const
		{
			return type_ == VAR_INTVECTOR2 ? *reinterpret_cast<const IntVector2*>(&value_) : IntVector2::ZERO;
		}
		/// Return a Matrix3 or identity on type mismatch.
		const Matrix3& GetMatrix3() const
		{
			return type_ == VAR_MATRIX3 ? *(reinterpret_cast<const Matrix3*>(value_.ptr_)) : Matrix3::IDENTITY;
		}

		/// Return a Matrix3x4 or identity on type mismatch.
		const Matrix3x4& GetMatrix3x4() const
		{
			return type_ == VAR_MATRIX3X4 ? *(reinterpret_cast<const Matrix3x4*>(value_.ptr_)) : Matrix3x4::IDENTITY;
		}

		/// Return a Matrix4 or identity on type mismatch.
		const Matrix4& GetMatrix4() const
		{
			return type_ == VAR_MATRIX4 ? *(reinterpret_cast<const Matrix4*>(value_.ptr_)) : Matrix4::IDENTITY;
		}

		/// Return value's type.
		VariantType GetType() const { return type_; }

		/// Return value's type name.
		YumeString GetTypeName() const;
		/// Convert value to string. Pointers are returned as null, and VariantBuffer or VariantMap are not supported and return empty.
		YumeString ToString() const;
		/// Return true when the variant value is considered zero according to its actual type.
		bool IsZero() const;

		/// Return true when the variant is empty (i.e. not initialized yet).
		bool IsEmpty() const { return type_ == VAR_EMPTY; }

		/// Return the value, template version.
		template <class T> T Get() const;

		/// Return a pointer to a modifiable buffer or null on type mismatch.
		YumeVector<unsigned char>::type* GetBufferPtr()
		{
			return type_ == VAR_BUFFER ? reinterpret_cast<YumeVector<unsigned char>::type*>(&value_) : 0;
		}

		/// Return a pointer to a modifiable variant vector or null on type mismatch.
		VariantVector* GetVariantVectorPtr() { return type_ == VAR_VARIANTVECTOR ? reinterpret_cast<VariantVector*>(&value_) : 0; }

		/// Return a pointer to a modifiable string vector or null on type mismatch.
		StringVector* GetStringVectorPtr() { return type_ == VAR_STRINGVECTOR ? reinterpret_cast<StringVector*>(&value_) : 0; }

		/// Return a pointer to a modifiable variant map or null on type mismatch.
		VariantMap* GetVariantMapPtr() { return type_ == VAR_VARIANTMAP ? reinterpret_cast<VariantMap*>(&value_) : 0; }

		/// Return name for variant type.
		static YumeString GetTypeName(VariantType type);
		/// Return variant type from type name.
		static VariantType GetTypeFromName(const YumeString& typeName);
		/// Return variant type from type name.
		static VariantType GetTypeFromName(const char* typeName);

		/// Empty variant.
		static const Variant EMPTY;
		/// Empty buffer.
		static const YumeVector<unsigned char>::type emptyBuffer;
		/// Empty resource reference.
		static const ResourceRef emptyResourceRef;
		/// Empty resource reference list.
		static const ResourceRefList emptyResourceRefList;
		/// Empty variant map.
		static const VariantMap emptyVariantMap;
		/// Empty variant vector.
		static const VariantVector emptyVariantVector;
		/// Empty string vector.
		static const StringVector emptyStringVector;

	private:
		/// Set new type and allocate/deallocate memory as necessary.
		void SetType(VariantType newType);

		/// Variant type.
		VariantType type_;
		/// Variant value.
		VariantValue value_;

		YumeString stringHack_;
	};

	/// Return variant type from type.
	template <typename T> VariantType GetVariantType();

	// Return variant type from concrete types
	template <> inline VariantType GetVariantType<int>() { return VAR_INT; }

	template <> inline VariantType GetVariantType<unsigned>() { return VAR_INT; }

	template <> inline VariantType GetVariantType<bool>() { return VAR_BOOL; }

	template <> inline VariantType GetVariantType<float>() { return VAR_FLOAT; }

	template <> inline VariantType GetVariantType<double>() { return VAR_DOUBLE; }

	template <> inline VariantType GetVariantType<Vector2>() { return VAR_VECTOR2; }

	template <> inline VariantType GetVariantType<Vector3>() { return VAR_VECTOR3; }

	template <> inline VariantType GetVariantType<Vector4>() { return VAR_VECTOR4; }

	template <> inline VariantType GetVariantType<Quaternion>() { return VAR_QUATERNION; }

	template <> inline VariantType GetVariantType<YumeColor>() { return VAR_COLOR; }

	template <> inline VariantType GetVariantType<YumeString>() { return VAR_STRING; }

	template <> inline VariantType GetVariantType<YumeHash>() { return VAR_INT; }

	template <> inline VariantType GetVariantType<YumeVector<unsigned char>::type >() { return VAR_BUFFER; }

	template <> inline VariantType GetVariantType<ResourceRef>() { return VAR_RESOURCEREF; }

	template <> inline VariantType GetVariantType<ResourceRefList>() { return VAR_RESOURCEREFLIST; }

	template <> inline VariantType GetVariantType<VariantVector>() { return VAR_VARIANTVECTOR; }

	template <> inline VariantType GetVariantType<StringVector>() { return VAR_STRINGVECTOR; }

	template <> inline VariantType GetVariantType<VariantMap>() { return VAR_VARIANTMAP; }

	template <> inline VariantType GetVariantType<IntRect>() { return VAR_INTRECT; }

	template <> inline VariantType GetVariantType<IntVector2>() { return VAR_INTVECTOR2; }

	template <> inline VariantType GetVariantType<Matrix3>() { return VAR_MATRIX3; }

	template <> inline VariantType GetVariantType<Matrix3x4>() { return VAR_MATRIX3X4; }

	template <> inline VariantType GetVariantType<Matrix4>() { return VAR_MATRIX4; }

	// Specializations of Variant::Get<T>
	template <> YumeAPIExport int Variant::Get<int>() const;

	template <> YumeAPIExport unsigned Variant::Get<unsigned>() const;

	template <> YumeAPIExport YumeHash Variant::Get<YumeHash>() const;

	template <> YumeAPIExport bool Variant::Get<bool>() const;

	template <> YumeAPIExport float Variant::Get<float>() const;

	template <> YumeAPIExport double Variant::Get<double>() const;

	template <> YumeAPIExport const Vector2& Variant::Get<const Vector2&>() const;

	template <> YumeAPIExport const Vector3& Variant::Get<const Vector3&>() const;

	template <> YumeAPIExport const Vector4& Variant::Get<const Vector4&>() const;

	template <> YumeAPIExport const Quaternion& Variant::Get<const Quaternion&>() const;

	template <> YumeAPIExport const YumeColor& Variant::Get<const YumeColor&>() const;

	template <> YumeAPIExport const YumeString& Variant::Get<const YumeString&>() const;

	template <> YumeAPIExport const IntRect& Variant::Get<const IntRect&>() const;

	template <> YumeAPIExport const IntVector2& Variant::Get<const IntVector2&>() const;

	template <> YumeAPIExport const YumeVector<unsigned char>::type& Variant::Get<const YumeVector<unsigned char>::type&>() const;

	template <> YumeAPIExport void* Variant::Get<void*>() const;

	template <> YumeAPIExport const Matrix3& Variant::Get<const Matrix3&>() const;

	template <> YumeAPIExport const Matrix3x4& Variant::Get<const Matrix3x4&>() const;

	template <> YumeAPIExport const Matrix4& Variant::Get<const Matrix4&>() const;

	template <> YumeAPIExport ResourceRef Variant::Get<ResourceRef>() const;

	template <> YumeAPIExport ResourceRefList Variant::Get<ResourceRefList>() const;

	template <> YumeAPIExport VariantVector Variant::Get<VariantVector>() const;

	template <> YumeAPIExport StringVector Variant::Get<StringVector>() const;

	template <> YumeAPIExport VariantMap Variant::Get<VariantMap>() const;

	template <> YumeAPIExport Vector2 Variant::Get<Vector2>() const;

	template <> YumeAPIExport Vector3 Variant::Get<Vector3>() const;

	template <> YumeAPIExport Vector4 Variant::Get<Vector4>() const;

	template <> YumeAPIExport Quaternion Variant::Get<Quaternion>() const;

	template <> YumeAPIExport YumeColor Variant::Get<YumeColor>() const;

	template <> YumeAPIExport YumeString Variant::Get<YumeString>() const;

	template <> YumeAPIExport IntRect Variant::Get<IntRect>() const;

	template <> YumeAPIExport IntVector2 Variant::Get<IntVector2>() const;

	template <> YumeAPIExport YumeVector<unsigned char>::type Variant::Get<YumeVector<unsigned char>::type >() const;

	template <> YumeAPIExport Matrix3 Variant::Get<Matrix3>() const;

	template <> YumeAPIExport Matrix3x4 Variant::Get<Matrix3x4>() const;

	template <> YumeAPIExport Matrix4 Variant::Get<Matrix4>() const;
}


//----------------------------------------------------------------------------
#endif

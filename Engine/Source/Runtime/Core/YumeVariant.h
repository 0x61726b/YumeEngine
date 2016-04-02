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

#ifdef _MSC_VER
#   pragma warning(disable:4805)
#   pragma warning(disable:4389)
#   pragma warning(disable:4800)
#endif
//----------------------------------------------------------------------------
namespace YumeEngine
{


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
		VAR_VECTOR4VECTOR,
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


	typedef YumeVector<Variant> VariantVector;
	typedef YumeVector<Vector4> Vector4Vector;


	typedef YumeMap<YumeHash,Variant> VariantMap;


	struct YumeAPIExport ResourceRef
	{

		ResourceRef()
		{
		}


		ResourceRef(YumeHash type):
			type_(type)
		{
		}


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


		YumeHash type_;

		YumeString name_;


		bool operator ==(const ResourceRef& rhs) const { return type_ == rhs.type_ && name_ == rhs.name_; }


		bool operator !=(const ResourceRef& rhs) const { return type_ != rhs.type_ || name_ != rhs.name_; }
	};


	struct YumeAPIExport ResourceRefList
	{
		ResourceRefList()
		{
		}
		ResourceRefList(YumeHash type):
			type_(type)
		{
		}
		ResourceRefList(YumeHash type,const StringVector::type& names):
			type_(type),
			names_(names)
		{
		}

		YumeHash type_;
		StringVector::type names_;
		bool operator ==(const ResourceRefList& rhs) const { return type_ == rhs.type_; }


		bool operator !=(const ResourceRefList& rhs) const { return type_ != rhs.type_; }
	};


	class YumeAPIExport Variant
	{
	public:

		Variant():
			type_(VAR_EMPTY)
		{
		}


		Variant(int value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(unsigned value):
			type_(VAR_EMPTY)
		{
			*this = (int)value;
		}


		Variant(const YumeHash& value):
			type_(VAR_EMPTY)
		{
			*this = (int)value;
		}

		Variant(const YumeVector<unsigned char>::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(bool value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(float value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(double value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Vector2& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Vector3& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Vector4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Quaternion& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const YumeColor& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const YumeString& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const char* value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const VectorBuffer& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(void* value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const ResourceRef& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


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


		Variant(const VariantVector::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}

		Variant(const Vector4Vector::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const VariantMap::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const StringVector::type& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const IntRect& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const IntVector2& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Matrix3& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Matrix3x4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const Matrix4& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		Variant(const YumeString& type,const YumeString& value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}


		Variant(VariantType type,const YumeString& value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}


		Variant(const char* type,const char* value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}


		Variant(VariantType type,const char* value):
			type_(VAR_EMPTY)
		{
			FromString(type,value);
		}


		Variant(const Variant& value):
			type_(VAR_EMPTY)
		{
			*this = value;
		}


		~Variant()
		{
			SetType(VAR_EMPTY);
		}


		void Clear()
		{
			SetType(VAR_EMPTY);
		}


		Variant& operator =(const Variant& rhs);


		Variant& operator =(int rhs)
		{
			SetType(VAR_INT);
			value_.int_ = rhs;
			return *this;
		}


		Variant& operator =(unsigned rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs;
			return *this;
		}


		Variant& operator =(const YumeHash& rhs)
		{
			SetType(VAR_INT);
			value_.int_ = (int)rhs;
			return *this;
		}


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


		Variant& operator =(float rhs)
		{
			SetType(VAR_FLOAT);
			value_.float_ = rhs;
			return *this;
		}


		Variant& operator = (double rhs)
		{
			SetType(VAR_DOUBLE);
			*(reinterpret_cast<double*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const Vector2& rhs)
		{
			SetType(VAR_VECTOR2);
			*(reinterpret_cast<Vector2*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const Vector3& rhs)
		{
			SetType(VAR_VECTOR3);
			*(reinterpret_cast<Vector3*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const Vector4& rhs)
		{
			SetType(VAR_VECTOR4);
			*(reinterpret_cast<Vector4*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const Quaternion& rhs)
		{
			SetType(VAR_QUATERNION);
			*(reinterpret_cast<Quaternion*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const YumeColor& rhs)
		{
			SetType(VAR_COLOR);
			*(reinterpret_cast<YumeColor*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const YumeString& rhs)
		{
			SetType(VAR_STRING);
			*(reinterpret_cast<YumeString*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const char* rhs)
		{
			SetType(VAR_STRING);
			*(reinterpret_cast<YumeString*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const YumeVector<unsigned char>::type& rhs)
		{
			SetType(VAR_BUFFER);
			*(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_)) = rhs;
			return *this;
		}

		Variant& operator =(const VectorBuffer& rhs);


		Variant& operator =(void* rhs)
		{
			SetType(VAR_VOIDPTR);
			value_.ptr_ = rhs;
			return *this;
		}


		Variant& operator =(const VariantVector::type& rhs)
		{
			SetType(VAR_VARIANTVECTOR);
			*(reinterpret_cast<VariantVector::type*>(&value_)) = rhs;
			return *this;
		}

		Variant& operator =(const Vector4Vector::type& rhs)
		{
			SetType(VAR_VECTOR4VECTOR);
			*(reinterpret_cast<Vector4Vector::type*>(&value_)) = rhs;
			return *this;
		}

		// Assign from a string vector.
		Variant& operator =(const StringVector& rhs)
		{
			SetType(VAR_STRINGVECTOR);
			*(reinterpret_cast<StringVector*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const VariantMap& rhs)
		{
			SetType(VAR_VARIANTMAP);
			*(reinterpret_cast<VariantMap*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const IntRect& rhs)
		{
			SetType(VAR_INTRECT);
			*(reinterpret_cast<IntRect*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const IntVector2& rhs)
		{
			SetType(VAR_INTVECTOR2);
			*(reinterpret_cast<IntVector2*>(&value_)) = rhs;
			return *this;
		}


		Variant& operator =(const Matrix3& rhs)
		{
			SetType(VAR_MATRIX3);
			*(reinterpret_cast<Matrix3*>(value_.ptr_)) = rhs;
			return *this;
		}


		Variant& operator =(const Matrix3x4& rhs)
		{
			SetType(VAR_MATRIX3X4);
			*(reinterpret_cast<Matrix3x4*>(value_.ptr_)) = rhs;
			return *this;
		}


		Variant& operator =(const Matrix4& rhs)
		{
			SetType(VAR_MATRIX4);
			*(reinterpret_cast<Matrix4*>(value_.ptr_)) = rhs;
			return *this;
		}


		bool operator ==(const Variant& rhs) const;


		bool operator ==(int rhs) const { return type_ == VAR_INT ? value_.int_ == rhs : false; }


		bool operator ==(unsigned rhs) const { return type_ == VAR_INT ? value_.int_ == (int)rhs : false; }


		bool operator ==(bool rhs) const { return type_ == VAR_BOOL ? value_.bool_ == rhs : false; }


		bool operator ==(float rhs) const { return type_ == VAR_FLOAT ? value_.float_ == rhs : false; }


		bool operator ==(double rhs) const { return type_ == VAR_DOUBLE ? *(reinterpret_cast<const double*>(&value_)) == rhs : false; }


		bool operator ==(const Vector2& rhs) const
		{
			return type_ == VAR_VECTOR2 ? *(reinterpret_cast<const Vector2*>(&value_)) == rhs : false;
		}


		bool operator ==(const Vector3& rhs) const
		{
			return type_ == VAR_VECTOR3 ? *(reinterpret_cast<const Vector3*>(&value_)) == rhs : false;
		}


		bool operator ==(const Vector4& rhs) const
		{
			return type_ == VAR_VECTOR4 ? *(reinterpret_cast<const Vector4*>(&value_)) == rhs : false;
		}


		bool operator ==(const Quaternion& rhs) const
		{
			return type_ == VAR_QUATERNION ? *(reinterpret_cast<const Quaternion*>(&value_)) == rhs : false;
		}


		bool operator ==(const YumeColor& rhs) const
		{
			return type_ == VAR_COLOR ? *(reinterpret_cast<const YumeColor*>(&value_)) == rhs : false;
		}


		bool operator ==(const YumeString& rhs) const
		{
			return type_ == VAR_STRING ? *(reinterpret_cast<const YumeString*>(&value_)) == rhs : false;
		}


		bool operator ==(const YumeVector<unsigned char>::type& rhs) const;

		bool operator ==(const VectorBuffer& rhs) const;



		bool operator ==(const ResourceRef& rhs) const
		{
			return type_ == VAR_RESOURCEREF ? *(reinterpret_cast<const ResourceRef*>(&value_)) == rhs : false;
		}


		bool operator ==(const ResourceRefList& rhs) const
		{
			return type_ == VAR_RESOURCEREFLIST ? *(reinterpret_cast<const ResourceRefList*>(&value_)) == rhs : false;
		}


		bool operator ==(const VariantVector::type& rhs) const
		{
			return type_ == VAR_VARIANTVECTOR ? *(reinterpret_cast<const VariantVector::type*>(&value_)) == rhs : false;
		}

		bool operator ==(const Vector4Vector::type& rhs) const
		{
			return type_ == VAR_VECTOR4VECTOR ? *(reinterpret_cast<const Vector4Vector::type*>(&value_)) == rhs : false;
		}


		bool operator ==(const StringVector& rhs) const
		{
			/*return type_ == VAR_STRINGVECTOR ? *(reinterpret_cast<const StringVector*>(&value_)) == rhs : false;*/
			return false;
		}


		bool operator ==(const VariantMap::type& rhs) const
		{
			return type_ == VAR_VARIANTMAP ? *(reinterpret_cast<const VariantMap::type*>(&value_)) == rhs : false;
		}


		bool operator ==(const IntRect& rhs) const
		{
			return type_ == VAR_INTRECT ? *(reinterpret_cast<const IntRect*>(&value_)) == rhs : false;
		}


		bool operator ==(const IntVector2& rhs) const
		{
			return type_ == VAR_INTVECTOR2 ? *(reinterpret_cast<const IntVector2*>(&value_)) == rhs : false;
		}


		bool operator ==(const YumeHash& rhs) const { return type_ == VAR_INT ? (unsigned)value_.int_ == rhs : false; }



		bool operator ==(const Matrix3& rhs) const
		{
			return type_ == VAR_MATRIX3 ? *(reinterpret_cast<const Matrix3*>(value_.ptr_)) == rhs : false;
		}


		bool operator ==(const Matrix3x4& rhs) const
		{
			return type_ == VAR_MATRIX3X4 ? *(reinterpret_cast<const Matrix3x4*>(value_.ptr_)) == rhs : false;
		}


		bool operator ==(const Matrix4& rhs) const
		{
			return type_ == VAR_MATRIX4 ? *(reinterpret_cast<const Matrix4*>(value_.ptr_)) == rhs : false;
		}


		bool operator !=(const Variant& rhs) const { return !(*this == rhs); }


		bool operator !=(int rhs) const { return !(*this == rhs); }


		bool operator !=(unsigned rhs) const { return !(*this == rhs); }


		bool operator !=(bool rhs) const { return !(*this == rhs); }


		bool operator !=(float rhs) const { return !(*this == rhs); }


		bool operator !=(double rhs) const { return !(*this == rhs); }


		bool operator !=(const Vector2& rhs) const { return !(*this == rhs); }


		bool operator !=(const Vector3& rhs) const { return !(*this == rhs); }


		bool operator !=(const Vector4& rhs) const { return !(*this == rhs); }


		bool operator !=(const Quaternion& rhs) const { return !(*this == rhs); }


		bool operator !=(const YumeString& rhs) const { return !(*this == rhs); }


		bool operator !=(const YumeVector<unsigned char>::type& rhs) const { return !(*this == rhs); }


		bool operator !=(const VectorBuffer& rhs) const { return !(*this == rhs); }


		bool operator !=(void* rhs) const { return !(*this == rhs); }


		bool operator !=(const ResourceRef& rhs) const { return !(*this == rhs); }


		bool operator !=(const ResourceRefList& rhs) const { return !(*this == rhs); }


		bool operator !=(const VariantVector::type& rhs) const { return !(*this == rhs); }


		bool operator !=(const StringVector::type& rhs) const { return !(*this == rhs); }


		bool operator !=(const VariantMap::type& rhs) const { return !(*this == rhs); }


		bool operator !=(const IntRect& rhs) const { return !(*this == rhs); }


		bool operator !=(const IntVector2& rhs) const { return !(*this == rhs); }


		bool operator !=(const YumeHash& rhs) const { return !(*this == rhs); }


		bool operator !=(const Matrix3& rhs) const { return !(*this == rhs); }


		bool operator !=(const Matrix3x4& rhs) const { return !(*this == rhs); }


		bool operator !=(const Matrix4& rhs) const { return !(*this == rhs); }


		void FromString(const YumeString& type,const YumeString& value);

		void FromString(const char* type,const char* value);

		void FromString(VariantType type,const YumeString& value);

		void FromString(VariantType type,const char* value);

		void SetBuffer(const void* data,unsigned size);


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


		YumeHash GetYumeHash() const { return YumeHash(GetUInt()); }


		bool GetBool() const { return type_ == VAR_BOOL ? value_.bool_ : false; }


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


		const Vector2& GetVector2() const { return type_ == VAR_VECTOR2 ? *reinterpret_cast<const Vector2*>(&value_) : Vector2::ZERO; }


		const Vector3& GetVector3() const { return type_ == VAR_VECTOR3 ? *reinterpret_cast<const Vector3*>(&value_) : Vector3::ZERO; }


		const Vector4& GetVector4() const { return type_ == VAR_VECTOR4 ? *reinterpret_cast<const Vector4*>(&value_) : Vector4::ZERO; }


		const Quaternion& GetQuaternion() const
		{
			return type_ == VAR_QUATERNION ? *reinterpret_cast<const Quaternion*>(&value_) : Quaternion::IDENTITY;
		}


		const YumeColor& GetColor() const { return type_ == VAR_COLOR ? *reinterpret_cast<const YumeColor*>(&value_) : YumeColor::WHITE; }


		const String& GetString() const { return type_ == VAR_STRING ? *reinterpret_cast<const String*>(&value_) : String::EMPTY; }


		const YumeVector<unsigned char>::type& GetBuffer() const
		{
			return type_ == VAR_BUFFER ? *reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_) : emptyBuffer;
		}


		VectorBuffer GetVectorBuffer() const;


		const ResourceRef& GetResourceRef() const
		{
			return type_ == VAR_RESOURCEREF ? *reinterpret_cast<const ResourceRef*>(&value_) : emptyResourceRef;
		}


		const ResourceRefList& GetResourceRefList() const
		{
			return type_ == VAR_RESOURCEREFLIST ? *reinterpret_cast<const ResourceRefList*>(&value_) : emptyResourceRefList;
		}


		const VariantVector::type& GetVariantVector() const
		{
			return type_ == VAR_VARIANTVECTOR ? *reinterpret_cast<const VariantVector::type*>(&value_) : emptyVariantVector;
		}

		const Vector4Vector::type& GetVector4Array() const
		{
			return type_ == VAR_VECTOR4VECTOR ? *reinterpret_cast<const Vector4Vector::type*>(&value_) : emptyVector4Array;
		}


		const StringVector::type& GetStringVector() const
		{
			return type_ == VAR_STRINGVECTOR ? *reinterpret_cast<const StringVector::type*>(&value_) : emptyStringVector;
		}


		const VariantMap::type& GetVariantMap() const
		{
			return type_ == VAR_VARIANTMAP ? *reinterpret_cast<const VariantMap::type*>(&value_) : emptyVariantMap;
		}


		const IntRect& GetIntRect() const { return type_ == VAR_INTRECT ? *reinterpret_cast<const IntRect*>(&value_) : IntRect::ZERO; }


		const IntVector2& GetIntVector2() const
		{
			return type_ == VAR_INTVECTOR2 ? *reinterpret_cast<const IntVector2*>(&value_) : IntVector2::ZERO;
		}

		const Matrix3& GetMatrix3() const
		{
			return type_ == VAR_MATRIX3 ? *(reinterpret_cast<const Matrix3*>(value_.ptr_)) : Matrix3::IDENTITY;
		}


		const Matrix3x4& GetMatrix3x4() const
		{
			return type_ == VAR_MATRIX3X4 ? *(reinterpret_cast<const Matrix3x4*>(value_.ptr_)) : Matrix3x4::IDENTITY;
		}


		const Matrix4& GetMatrix4() const
		{
			return type_ == VAR_MATRIX4 ? *(reinterpret_cast<const Matrix4*>(value_.ptr_)) : Matrix4::IDENTITY;
		}


		VariantType GetType() const { return type_; }


		YumeString GetTypeName() const;

		YumeString ToString() const;

		bool IsZero() const;


		bool IsEmpty() const { return type_ == VAR_EMPTY; }


		template <class T> T Get() const;


		YumeVector<unsigned char>::type* GetBufferPtr()
		{
			return type_ == VAR_BUFFER ? reinterpret_cast<YumeVector<unsigned char>::type*>(&value_) : 0;
		}


		VariantVector::type* GetVariantVectorPtr() { return type_ == VAR_VARIANTVECTOR ? reinterpret_cast<VariantVector::type*>(&value_) : 0; }


		StringVector* GetStringVectorPtr() { return type_ == VAR_STRINGVECTOR ? reinterpret_cast<StringVector*>(&value_) : 0; }


		VariantMap* GetVariantMapPtr() { return type_ == VAR_VARIANTMAP ? reinterpret_cast<VariantMap*>(&value_) : 0; }

		static YumeString GetTypeName(VariantType type);

		static VariantType GetTypeFromName(const YumeString& typeName);

		static VariantType GetTypeFromName(const char* typeName);



		static const Variant EMPTY;
		static const YumeVector<unsigned char>::type emptyBuffer;
		static const ResourceRef emptyResourceRef;
		static const ResourceRefList emptyResourceRefList;
		static const VariantMap::type emptyVariantMap;
		static const VariantVector::type emptyVariantVector;
		static const Vector4Vector::type emptyVector4Array;
		static const StringVector::type emptyStringVector;

	private:

		void SetType(VariantType newType);

		VariantType type_;
		VariantValue value_;
	};


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

	template <> inline VariantType GetVariantType<VariantVector::type>() { return VAR_VARIANTVECTOR; }

	template <> inline VariantType GetVariantType<Vector4Vector::type>() { return VAR_VECTOR4VECTOR; }

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

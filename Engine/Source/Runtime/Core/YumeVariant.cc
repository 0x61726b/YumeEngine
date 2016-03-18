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
#include "YumeVariant.h"
#include "YumeDefaults.h"
#include "YumeVectorBuffer.h"

#include <boost/algorithm/string.hpp>

namespace YumeEngine
{

	const Variant Variant::EMPTY;
	const YumeVector<unsigned char>::type Variant::emptyBuffer;
	const ResourceRef Variant::emptyResourceRef;
	const ResourceRefList Variant::emptyResourceRefList;
	const VariantMap Variant::emptyVariantMap;
	const VariantVector Variant::emptyVariantVector;
	const StringVector Variant::emptyStringVector;

	static const char* typeNames[] =
	{
		"None",
		"Int",
		"Bool",
		"Float",
		"Vector2",
		"Vector3",
		"Vector4",
		"Quaternion",
		"Color",
		"String",
		"Buffer",
		"VoidPtr",
		"ResourceRef",
		"ResourceRefList",
		"VariantVector",
		"VariantMap",
		"IntRect",
		"IntVector2",
		"Ptr",
		"Matrix3",
		"Matrix3x4",
		"Matrix4",
		"Double",
		"StringVector",
		0
	};

	Variant& Variant::operator =(const Variant& rhs)
	{
		SetType(rhs.GetType());

		switch(type_)
		{
		case VAR_STRING:
			stringHack_ = rhs.stringHack_;
			break;

		case VAR_BUFFER:
			*(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_)) = *(reinterpret_cast<const YumeVector<unsigned char>::type*>(&rhs.value_));
			break;

		case VAR_RESOURCEREF:
			*(reinterpret_cast<ResourceRef*>(&value_)) = *(reinterpret_cast<const ResourceRef*>(&rhs.value_));
			break;

		case VAR_RESOURCEREFLIST:
			*(reinterpret_cast<ResourceRefList*>(&value_)) = *(reinterpret_cast<const ResourceRefList*>(&rhs.value_));
			break;

		case VAR_VARIANTVECTOR:
			*(reinterpret_cast<VariantVector*>(&value_)) = *(reinterpret_cast<const VariantVector*>(&rhs.value_));
			break;

		case VAR_STRINGVECTOR:
			*(reinterpret_cast<StringVector*>(&value_)) = *(reinterpret_cast<const StringVector*>(&rhs.value_));
			break;

		case VAR_VARIANTMAP:
			*(reinterpret_cast<VariantMap*>(&value_)) = *(reinterpret_cast<const VariantMap*>(&rhs.value_));
			break;

		case VAR_MATRIX3:
			*(reinterpret_cast<Matrix3*>(value_.ptr_)) = *(reinterpret_cast<const Matrix3*>(rhs.value_.ptr_));
			break;

		case VAR_MATRIX3X4:
			*(reinterpret_cast<Matrix3x4*>(value_.ptr_)) = *(reinterpret_cast<const Matrix3x4*>(rhs.value_.ptr_));
			break;

		case VAR_MATRIX4:
			*(reinterpret_cast<Matrix4*>(value_.ptr_)) = *(reinterpret_cast<const Matrix4*>(rhs.value_.ptr_));
			break;

		default:
			value_ = rhs.value_;
			break;
		}

		return *this;
	}

	Variant& Variant::operator =(const VectorBuffer& rhs)
	{
		SetType(VAR_BUFFER);
		*(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_)) = rhs.GetBuffer();
		return *this;
	}


	bool Variant::operator ==(const Variant& rhs) const
	{
		if(type_ != rhs.type_)
			return false;

		switch(type_)
		{
		case VAR_INT:
			return value_.int_ == rhs.value_.int_;

		case VAR_BOOL:
			return value_.bool_ == rhs.value_.bool_;

		case VAR_FLOAT:
			return value_.float_ == rhs.value_.float_;

		case VAR_VECTOR2:
			return *(reinterpret_cast<const Vector2*>(&value_)) == *(reinterpret_cast<const Vector2*>(&rhs.value_));

		case VAR_VECTOR3:
			return *(reinterpret_cast<const Vector3*>(&value_)) == *(reinterpret_cast<const Vector3*>(&rhs.value_));

		case VAR_VECTOR4:
		case VAR_QUATERNION:
		case VAR_COLOR:
			// Hack: use the Vector4 compare for all these classes, as they have the same memory structure
			return *(reinterpret_cast<const Vector4*>(&value_)) == *(reinterpret_cast<const Vector4*>(&rhs.value_));

		case VAR_STRING:
			return *(reinterpret_cast<const YumeString*>(&value_)) == *(reinterpret_cast<const YumeString*>(&rhs.value_));

		case VAR_BUFFER:
			return *(reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_)) ==
				*(reinterpret_cast<const YumeVector<unsigned char>::type*>(&rhs.value_));

		case VAR_RESOURCEREF:
			return *(reinterpret_cast<const ResourceRef*>(&value_)) == *(reinterpret_cast<const ResourceRef*>(&rhs.value_));

		case VAR_RESOURCEREFLIST:
			return *(reinterpret_cast<const ResourceRefList*>(&value_)) == *(reinterpret_cast<const ResourceRefList*>(&rhs.value_));

		case VAR_VARIANTVECTOR:
			return *(reinterpret_cast<const VariantVector*>(&value_)) == *(reinterpret_cast<const VariantVector*>(&rhs.value_));

		case VAR_STRINGVECTOR:
		{
			/*return *(reinterpret_cast<const StringVector*>(&value_)) == *(reinterpret_cast<const StringVector*>(&rhs.value_));*/
			return 0;
		}
		case VAR_VARIANTMAP:
			return *(reinterpret_cast<const VariantMap*>(&value_)) == *(reinterpret_cast<const VariantMap*>(&rhs.value_));

		case VAR_INTRECT:
			return *(reinterpret_cast<const IntRect*>(&value_)) == *(reinterpret_cast<const IntRect*>(&rhs.value_));

		case VAR_INTVECTOR2:
			return *(reinterpret_cast<const IntVector2*>(&value_)) == *(reinterpret_cast<const IntVector2*>(&rhs.value_));

		case VAR_MATRIX3:
			return *(reinterpret_cast<const Matrix3*>(value_.ptr_)) == *(reinterpret_cast<const Matrix3*>(rhs.value_.ptr_));

		case VAR_MATRIX3X4:
			return *(reinterpret_cast<const Matrix3x4*>(value_.ptr_)) == *(reinterpret_cast<const Matrix3x4*>(rhs.value_.ptr_));

		case VAR_MATRIX4:
			return *(reinterpret_cast<const Matrix4*>(value_.ptr_)) == *(reinterpret_cast<const Matrix4*>(rhs.value_.ptr_));

		case VAR_DOUBLE:
			return *(reinterpret_cast<const double*>(&value_)) == *(reinterpret_cast<const double*>(&rhs.value_));

		default:
			return true;
		}
	}

	bool Variant::operator ==(const YumeVector<unsigned char>::type& rhs) const
	{
		// Use strncmp() instead of YumeVector<unsigned char>::operator ==()
		const YumeVector<unsigned char>::type& buffer = *(reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_));
		return type_ == VAR_BUFFER && buffer.size() == rhs.size() ?
			strncmp(reinterpret_cast<const char*>(&buffer[0]),reinterpret_cast<const char*>(&rhs[0]),buffer.size()) == 0 :
			false;
	}

	bool Variant::operator ==(const VectorBuffer& rhs) const
	{
		const YumeVector<unsigned char>::type& buffer = *(reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_));
		return type_ == VAR_BUFFER && buffer.size() == rhs.GetSize() ?
			strncmp(reinterpret_cast<const char*>(&buffer[0]),reinterpret_cast<const char*>(rhs.GetData()),buffer.size()) == 0 :
			false;
	}

	void Variant::FromString(const YumeString& type,const YumeString& value)
	{
		return FromString(GetTypeFromName(type),value.c_str());
	}

	void Variant::FromString(const char* type,const char* value)
	{
		return FromString(GetTypeFromName(type),value);
	}

	void Variant::FromString(VariantType type,const YumeString& value)
	{
		return FromString(type,value.c_str());
	}

	void Variant::FromString(VariantType type,const char* value)
	{
		switch(type)
		{
		case VAR_INT:
			*this = ToInt(value);
			break;

		case VAR_BOOL:
			*this = ToBool(value);
			break;

		case VAR_FLOAT:
			*this = ToFloat(value);
			break;

		case VAR_VECTOR2:
			*this = ToVector2(value);
			break;

		case VAR_VECTOR3:
			*this = ToVector3(value);
			break;

		case VAR_VECTOR4:
			*this = ToVector4(value);
			break;

		case VAR_QUATERNION:
			*this = ToQuaternion(value);
			break;

		case VAR_COLOR:
			*this = ToColor(value);
			break;

		case VAR_STRING:
			*this = value;
			break;

		case VAR_BUFFER:
		{
			SetType(VAR_BUFFER);
			YumeVector<unsigned char>::type& buffer = *(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_));
			StringToBuffer(buffer,value);
		}
		break;

		case VAR_VOIDPTR:
			// From string to void pointer not supported, set to null
			*this = (void*)0;
			break;

		case VAR_RESOURCEREF:
		{
			std::vector<YumeString> values;
			boost::split(values,value,boost::is_any_of(";"));
			if(values.size() == 2)
			{
				SetType(VAR_RESOURCEREF);
				ResourceRef& ref = *(reinterpret_cast<ResourceRef*>(&value_));
				ref.type_ = (values[0]);
				ref.name_ = (values[1]);
			}
		}
		break;

		case VAR_RESOURCEREFLIST:
		{
			std::vector<YumeString> values;
			boost::split(values,value,boost::is_any_of(";"));
			if(values.size() >= 1)
			{
				SetType(VAR_RESOURCEREFLIST);
				ResourceRefList& refList = *(reinterpret_cast<ResourceRefList*>(&value_));
				refList.type_ = (values[0]);
				refList.names_.resize(values.size() - 1);
				for(unsigned i = 1; i < values.size(); ++i)
					refList.names_[i - 1] = values[i];
			}
		}
		break;

		case VAR_INTRECT:
			*this = ToIntRect(value);
			break;

		case VAR_INTVECTOR2:
			*this = ToIntVector2(value);
			break;
		case VAR_MATRIX3:
			*this = ToMatrix3(value);
			break;

		case VAR_MATRIX3X4:
			*this = ToMatrix3x4(value);
			break;

		case VAR_MATRIX4:
			*this = ToMatrix4(value);
			break;

		case VAR_DOUBLE:
			*this = ToDouble(value);
			break;

		default:
			SetType(VAR_EMPTY);
		}
	}

	void Variant::SetBuffer(const void* data,unsigned size)
	{
		if(size && !data)
			size = 0;

		SetType(VAR_BUFFER);
		YumeVector<unsigned char>::type& buffer = *(reinterpret_cast<YumeVector<unsigned char>::type*>(&value_));
		buffer.resize(size);
		if(size)
			memcpy(&buffer[0],data,size);
	}

	VectorBuffer Variant::GetVectorBuffer() const
	{
		return VectorBuffer(type_ == VAR_BUFFER ? *reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_) : emptyBuffer);
	}


	YumeString Variant::GetTypeName() const
	{
		return typeNames[type_];
	}

	YumeString Variant::ToString() const
	{
		switch(type_)
		{
		case VAR_INT:
			return std::to_string(value_.int_);

		case VAR_BOOL:
			return std::to_string(value_.bool_);

		case VAR_FLOAT:
			return std::to_string(value_.float_);

		case VAR_VECTOR2:
			return (reinterpret_cast<const Vector2*>(&value_))->ToString();

		case VAR_VECTOR3:
			return (reinterpret_cast<const Vector3*>(&value_))->ToString();

		case VAR_VECTOR4:
			return (reinterpret_cast<const Vector4*>(&value_))->ToString();

		case VAR_QUATERNION:
			return (reinterpret_cast<const Quaternion*>(&value_))->ToString();

		case VAR_COLOR:
			return (reinterpret_cast<const YumeColor*>(&value_))->ToString();

		case VAR_STRING:
			return *(reinterpret_cast<const YumeString*>(&value_));

		case VAR_BUFFER:
		{
			const YumeVector<unsigned char>::type& buffer = *(reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_));
			YumeString ret;
			BufferToString(ret,buffer.begin(),buffer.size());
			return ret;
		}

		case VAR_VOIDPTR:
		case VAR_PTR:
			// Pointer serialization not supported (convert to null)
			return YumeString(0);

		case VAR_INTRECT:
			return (reinterpret_cast<const IntRect*>(&value_))->ToString();

		case VAR_INTVECTOR2:
			return (reinterpret_cast<const IntVector2*>(&value_))->ToString();

		case VAR_MATRIX3:
			return (reinterpret_cast<const Matrix3*>(value_.ptr_))->ToString();

		case VAR_MATRIX3X4:
			return (reinterpret_cast<const Matrix3x4*>(value_.ptr_))->ToString();

		case VAR_MATRIX4:
			return (reinterpret_cast<const Matrix4*>(value_.ptr_))->ToString();

		case VAR_DOUBLE:
			return std::to_string(*reinterpret_cast<const double*>(&value_));

		default:
			// VAR_RESOURCEREF, VAR_RESOURCEREFLIST, VAR_VARIANTVECTOR, VAR_STRINGVECTOR, VAR_VARIANTMAP
			// Reference string serialization requires typehash-to-name mapping from the context. Can not support here
			// Also variant map or vector string serialization is not supported. XML or binary save should be used instead
			return YumeString();
		}
	}

	bool Variant::IsZero() const
	{
		switch(type_)
		{
		case VAR_INT:
			return value_.int_ == 0;

		case VAR_BOOL:
			return value_.bool_ == false;

		case VAR_FLOAT:
			return value_.float_ == 0.0f;

		case VAR_VECTOR2:
			return *reinterpret_cast<const Vector2*>(&value_) == Vector2::ZERO;

		case VAR_VECTOR3:
			return *reinterpret_cast<const Vector3*>(&value_) == Vector3::ZERO;

		case VAR_VECTOR4:
			return *reinterpret_cast<const Vector4*>(&value_) == Vector4::ZERO;

		case VAR_QUATERNION:
			return *reinterpret_cast<const Quaternion*>(&value_) == Quaternion::IDENTITY;

		case VAR_COLOR:
			// WHITE is considered empty (i.e. default) color in the Color class definition
			return *reinterpret_cast<const YumeColor*>(&value_) == YumeColor::WHITE;

		case VAR_STRING:
			return reinterpret_cast<const YumeString*>(&value_)->empty();

		case VAR_BUFFER:
			return reinterpret_cast<const YumeVector<unsigned char>::type*>(&value_)->empty();

		case VAR_VOIDPTR:
			return value_.ptr_ == 0;

		case VAR_RESOURCEREF:
			return reinterpret_cast<const ResourceRef*>(&value_)->name_.empty();

		case VAR_RESOURCEREFLIST:
		{
			const StringVector& names = reinterpret_cast<const ResourceRefList*>(&value_)->names_;
			for(StringVector::const_iterator i = names.begin(); i != names.end(); ++i)
			{
				if(!i->empty())
					return false;
			}
			return true;
		}

		case VAR_VARIANTVECTOR:
			return reinterpret_cast<const VariantVector*>(&value_)->empty();

		case VAR_STRINGVECTOR:
			return reinterpret_cast<const StringVector*>(&value_)->empty();

		case VAR_VARIANTMAP:
			return reinterpret_cast<const VariantMap*>(&value_)->empty();

		case VAR_INTRECT:
			return *reinterpret_cast<const IntRect*>(&value_) == IntRect::ZERO;

		case VAR_INTVECTOR2:
			return *reinterpret_cast<const IntVector2*>(&value_) == IntVector2::ZERO;

		case VAR_MATRIX3:
			return *reinterpret_cast<const Matrix3*>(value_.ptr_) == Matrix3::IDENTITY;

		case VAR_MATRIX3X4:
			return *reinterpret_cast<const Matrix3x4*>(value_.ptr_) == Matrix3x4::IDENTITY;

		case VAR_MATRIX4:
			return *reinterpret_cast<const Matrix4*>(value_.ptr_) == Matrix4::IDENTITY;

		case VAR_DOUBLE:
			return *reinterpret_cast<const double*>(&value_) == 0.0;

		default:
			return true;
		}
	}

	void Variant::SetType(VariantType newType)
	{
		if(type_ == newType)
			return;

		switch(type_)
		{
		case VAR_STRING:
			(reinterpret_cast<YumeString*>(&value_))->~YumeString();
			break;

		case VAR_BUFFER:
			(reinterpret_cast<YumeVector<unsigned char>*>(&value_))->~YumeVector<unsigned char>();
			break;

		case VAR_RESOURCEREF:
			(reinterpret_cast<ResourceRef*>(&value_))->~ResourceRef();
			break;

		case VAR_RESOURCEREFLIST:
			(reinterpret_cast<ResourceRefList*>(&value_))->~ResourceRefList();
			break;

		case VAR_VARIANTVECTOR:
			(reinterpret_cast<VariantVector*>(&value_))->~VariantVector();
			break;

		case VAR_STRINGVECTOR:
			(reinterpret_cast<StringVector*>(&value_))->~StringVector();
			break;

		case VAR_VARIANTMAP:
			(reinterpret_cast<VariantMap*>(&value_))->~VariantMap();
			break;

		case VAR_MATRIX3:
			delete reinterpret_cast<Matrix3*>(value_.ptr_);
			break;

		case VAR_MATRIX3X4:
			delete reinterpret_cast<Matrix3x4*>(value_.ptr_);
			break;

		case VAR_MATRIX4:
			delete reinterpret_cast<Matrix4*>(value_.ptr_);
			break;

		default:
			break;
		}

		type_ = newType;

		switch(type_)
		{
		case VAR_STRING:
			//new(reinterpret_cast<YumeString*>(&value_)) YumeString();
			break;

		case VAR_BUFFER:
			new(reinterpret_cast<YumeVector<unsigned char>*>(&value_)) YumeVector<unsigned char>();
			break;

		case VAR_RESOURCEREF:
			new(reinterpret_cast<ResourceRef*>(&value_)) ResourceRef();
			break;

		case VAR_RESOURCEREFLIST:
			new(reinterpret_cast<ResourceRefList*>(&value_)) ResourceRefList();
			break;

		case VAR_VARIANTVECTOR:
			new(reinterpret_cast<VariantVector*>(&value_)) VariantVector();
			break;

		case VAR_STRINGVECTOR:
			new(reinterpret_cast<StringVector*>(&value_)) StringVector();
			break;

		case VAR_VARIANTMAP:
			new(reinterpret_cast<VariantMap*>(&value_)) VariantMap();
			break;

		case VAR_MATRIX3:
			value_.ptr_ = new Matrix3();
			break;

		case VAR_MATRIX3X4:
			value_.ptr_ = new Matrix3x4();
			break;

		case VAR_MATRIX4:
			value_.ptr_ = new Matrix4();
			break;

		default:
			break;
		}
	}

	template <> int Variant::Get<int>() const
	{
		return GetInt();
	}

	template <> unsigned Variant::Get<unsigned>() const
	{
		return GetUInt();
	}

	template <> YumeHash Variant::Get<YumeHash>() const
	{
		return GetYumeHash();
	}

	template <> bool Variant::Get<bool>() const
	{
		return GetBool();
	}

	template <> float Variant::Get<float>() const
	{
		return GetFloat();
	}

	template <> double Variant::Get<double>() const
	{
		return GetDouble();
	}

	template <> const Vector2& Variant::Get<const Vector2&>() const
	{
		return GetVector2();
	}

	template <> const Vector3& Variant::Get<const Vector3&>() const
	{
		return GetVector3();
	}

	template <> const Vector4& Variant::Get<const Vector4&>() const
	{
		return GetVector4();
	}

	template <> const Quaternion& Variant::Get<const Quaternion&>() const
	{
		return GetQuaternion();
	}

	template <> const YumeColor& Variant::Get<const YumeColor&>() const
	{
		return GetColor();
	}

	template <> const YumeString& Variant::Get<const YumeString&>() const
	{
		return GetString();
	}

	template <> const IntRect& Variant::Get<const IntRect&>() const
	{
		return GetIntRect();
	}

	template <> const IntVector2& Variant::Get<const IntVector2&>() const
	{
		return GetIntVector2();
	}

	template <> const YumeVector<unsigned char>::type& Variant::Get<const YumeVector<unsigned char>::type&>() const
	{
		return GetBuffer();
	}

	template <> void* Variant::Get<void*>() const
	{
		return GetVoidPtr();
	}


	template <> const Matrix3& Variant::Get<const Matrix3&>() const
	{
		return GetMatrix3();
	}

	template <> const Matrix3x4& Variant::Get<const Matrix3x4&>() const
	{
		return GetMatrix3x4();
	}

	template <> const Matrix4& Variant::Get<const Matrix4&>() const
	{
		return GetMatrix4();
	}

	template <> ResourceRef Variant::Get<ResourceRef>() const
	{
		return GetResourceRef();
	}

	template <> ResourceRefList Variant::Get<ResourceRefList>() const
	{
		return GetResourceRefList();
	}

	template <> VariantVector Variant::Get<VariantVector>() const
	{
		return GetVariantVector();
	}

	template <> StringVector Variant::Get<StringVector >() const
	{
		return GetStringVector();
	}

	template <> VariantMap Variant::Get<VariantMap>() const
	{
		return GetVariantMap();
	}

	template <> Vector2 Variant::Get<Vector2>() const
	{
		return GetVector2();
	}

	template <> Vector3 Variant::Get<Vector3>() const
	{
		return GetVector3();
	}

	template <> Vector4 Variant::Get<Vector4>() const
	{
		return GetVector4();
	}

	template <> Quaternion Variant::Get<Quaternion>() const
	{
		return GetQuaternion();
	}

	template <> YumeColor Variant::Get<YumeColor>() const
	{
		return GetColor();
	}

	template <> YumeString Variant::Get<YumeString>() const
	{
		return GetString();
	}

	template <> IntRect Variant::Get<IntRect>() const
	{
		return GetIntRect();
	}

	template <> IntVector2 Variant::Get<IntVector2>() const
	{
		return GetIntVector2();
	}

	template <> YumeVector<unsigned char>::type Variant::Get<YumeVector<unsigned char>::type >() const
	{
		return GetBuffer();
	}

	template <> Matrix3 Variant::Get<Matrix3>() const
	{
		return GetMatrix3();
	}

	template <> Matrix3x4 Variant::Get<Matrix3x4>() const
	{
		return GetMatrix3x4();
	}

	template <> Matrix4 Variant::Get<Matrix4>() const
	{
		return GetMatrix4();
	}

	YumeString Variant::GetTypeName(VariantType type)
	{
		return typeNames[type];
	}

	VariantType Variant::GetTypeFromName(const YumeString& typeName)
	{
		return GetTypeFromName(typeName.c_str());
	}

	VariantType Variant::GetTypeFromName(const char* typeName)
	{
		return (VariantType)GetStringListIndex(typeName,typeNames,VAR_EMPTY);
	}
}

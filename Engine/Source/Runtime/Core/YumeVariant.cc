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

#include <boost/mpl/contains.hpp>


namespace YumeEngine
{
	template <> int YumeVariant::Get<int>() const
	{
		if(inner_.which() == VAR_INT)
			return boost::get<int>(inner_);
		else
			return 0;
	}

	template <> double YumeVariant::Get<double>() const
	{
		if(inner_.which() == VAR_DOUBLE)
			return boost::get<double>(inner_);
		else
			return 0.0;
	}

	template <> float YumeVariant::Get<float>() const
	{
		if(inner_.which() == VAR_FLOAT)
			return boost::get<float>(inner_);
		else
			return 0.0f;
	}

	template <> bool YumeVariant::Get<bool>() const
	{
		if(inner_.which() == VAR_BOOL)
			return boost::get<bool>(inner_);
		else
			return false;
	}

	template <> YumeString YumeVariant::Get<YumeString>() const
	{
		if(inner_.which() == VAR_STRING)
			return boost::get<YumeString>(inner_);
		else
			return YumeString();
	}


	void YumeVariant::ConvertFromString(VariantType type,const char* value)
	{
		switch(type)
		{
		case VAR_INT:
			inner_= ToInt(value);
			type_ = VAR_INT;
			break;

		case VAR_BOOL:
			inner_= ToBool(value);
			type_ = VAR_BOOL;
			break;

		case VAR_FLOAT:
			inner_= ToFloat(value);
			type_ = VAR_FLOAT;
			break;

			/*case VAR_VECTOR2:
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
				*/
		case VAR_STRING:
			inner_ = value;
			type_ = VAR_STRING;
			break;

			//case VAR_BUFFER:
			//{
			//	SetType(VAR_BUFFER);
			//	PODVector<unsigned char>& buffer = *(reinterpret_cast<PODVector<unsigned char>*>(&value_));
			//	StringToBuffer(buffer,value);
			//}
			//break;

			//case VAR_VOIDPTR:
			//	// From string to void pointer not supported, set to null
			//	*this = (void*)0;
			//	break;

			//case VAR_RESOURCEREF:
			//{
			//	StringVector values = String::Split(value,';');
			//	if(values.Size() == 2)
			//	{
			//		SetType(VAR_RESOURCEREF);
			//		ResourceRef& ref = *(reinterpret_cast<ResourceRef*>(&value_));
			//		ref.type_ = values[0];
			//		ref.name_ = values[1];
			//	}
			//}
			//break;

			//case VAR_RESOURCEREFLIST:
			//{
			//	StringVector values = String::Split(value,';',true);
			//	if(values.Size() >= 1)
			//	{
			//		SetType(VAR_RESOURCEREFLIST);
			//		ResourceRefList& refList = *(reinterpret_cast<ResourceRefList*>(&value_));
			//		refList.type_ = values[0];
			//		refList.names_.Resize(values.Size() - 1);
			//		for(unsigned i = 1; i < values.Size(); ++i)
			//			refList.names_[i - 1] = values[i];
			//	}
			//}
			//break;

			//case VAR_INTRECT:
			//	*this = ToIntRect(value);
			//	break;

			//case VAR_INTVECTOR2:
			//	*this = ToIntVector2(value);
			//	break;

			//case VAR_PTR:
			//	// From string to RefCounted pointer not supported, set to null
			//	*this = (RefCounted*)0;
			//	break;

			//case VAR_MATRIX3:
			//	*this = ToMatrix3(value);
			//	break;

			//case VAR_MATRIX3X4:
			//	*this = ToMatrix3x4(value);
			//	break;

			//case VAR_MATRIX4:
			//	*this = ToMatrix4(value);
			//	break;

		case VAR_DOUBLE:
			inner_ = ToDouble(value);
			type_ = VAR_DOUBLE;
			break;

		default:
			type_ = (VAR_EMPTY);
		}
	}
}

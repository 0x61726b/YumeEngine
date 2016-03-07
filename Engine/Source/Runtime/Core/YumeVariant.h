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
#ifndef __YumeVariant_h__
#define __YumeVariant_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/variant.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/utility/enable_if.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	typedef boost::variant<int,double,float,bool,YumeString> Variant;
	class YumeVariant;
	typedef YumeMap<YumeString,YumeVariant>::type VariantMap;
	typedef YumeMap<YumeHash,YumeVariant>::type HashVariantMap;

	enum VariantType
	{
		VAR_EMPTY = -1,
		VAR_INT = 0,
		VAR_DOUBLE = 1,
		VAR_FLOAT = 2,
		VAR_BOOL = 3,
		VAR_STRING
	};

	class YumeAPIExport YumeVariant
	{
	public:
		explicit YumeVariant()
		{
			inner_ = Variant();
			type_ = VAR_EMPTY;
		}
		YumeVariant(int var)
		{
			inner_ = Variant(var);
			type_ = VAR_INT;
		};

		YumeVariant(double var)
		{
			inner_ = Variant(var);
			type_ = VAR_DOUBLE;
		};

		YumeVariant(float var)
		{
			inner_ = Variant(var);
			type_ = VAR_FLOAT;
		};

		YumeVariant(bool var)
		{
			inner_ = Variant(var);
			type_ = VAR_BOOL;
		};

		YumeVariant(const char* var)
		{
			inner_ = Variant(YumeString(var));
			type_ = VAR_STRING;
		}

		YumeVariant(YumeString var)
		{
			inner_ = Variant(var);
			type_ = VAR_STRING;
		};

		template< class T> T Get() const;

		VariantType GetType() const { return type_; }

		void ConvertFromString(VariantType type,const char* value);
	protected:
		Variant inner_;
		VariantType type_;
	};

	template <> YumeAPIExport int YumeVariant::Get<int>() const;
	template <> YumeAPIExport double YumeVariant::Get<double>() const;
	template <> YumeAPIExport float YumeVariant::Get<float>() const;
	template <> YumeAPIExport bool YumeVariant::Get<bool>() const;
	template <> YumeAPIExport YumeString YumeVariant::Get<YumeString>() const;
}


//----------------------------------------------------------------------------
#endif

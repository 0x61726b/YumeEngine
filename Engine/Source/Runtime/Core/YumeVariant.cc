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
}

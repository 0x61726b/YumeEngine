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
		return boost::get<int>(inner_);
	}

	template <> double YumeVariant::Get<double>() const
	{
		return boost::get<double>(inner_);
	}

	template <> float YumeVariant::Get<float>() const
	{
		return boost::get<float>(inner_);
	}

	template <> bool YumeVariant::Get<bool>() const
	{
		return boost::get<bool>(inner_);
	}

	template <> YumeString YumeVariant::Get<YumeString>() const
	{
		return boost::get<YumeString>(inner_);
	}
}

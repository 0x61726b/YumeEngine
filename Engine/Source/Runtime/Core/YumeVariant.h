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
#include <boost/mpl/contains.hpp>
#include <boost/utility/enable_if.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	typedef boost::variant<int,double,float,bool,YumeString> YumeVariant;
	typedef YumeMap<YumeString,YumeVariant>::type VariantMap;

	template <typename Visitor,typename TypeList>
	struct picky_visitor :
		public boost::static_visitor<void>,
		public Visitor
	{
		template <typename T>
		inline void
			operator () (T v,typename boost::enable_if< typename boost::mpl::contains< TypeList,T >::type >::type *dummy = NULL) const
		{
			
		}

		template <typename T>
		inline void
			operator () (T v,typename boost::disable_if<typename boost::mpl::contains< TypeList,T >::type >::type *dummy = NULL) const
		{
		}
	};
	

	struct example_visitor
	{
		typedef picky_visitor< example_visitor,boost::mpl::vector<int,double,float,bool,char*> > value_type;
	};
}


//----------------------------------------------------------------------------
#endif

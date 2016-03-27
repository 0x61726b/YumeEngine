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
#ifndef __YumeUIBinding_h__
#define __YumeUIBinding_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum BindingType
	{
		BINDING_STATIC = 0,
		BINDING_DYNAMIC
	};

	class YumeUIElement;

	class YumeAPIExport YumeUIBinding : public YumeBase
	{
	public:
		YumeUIBinding(YumeUIElement* parent,const YumeString& name,const Variant& value,BindingType type = BINDING_STATIC );

		void SetValue(const Variant& value);
		void ClearDirty();
		void SetDirty();

		const Variant& GetValue() const { return value_; }
		const YumeString& GetName() const { return name_; }
		const BindingType GetBindingType() const { return bindingType_; }
		bool GetDirty() const {return dirty_;}
	private:
		YumeString name_;
		BindingType bindingType_;
		Variant value_;
		bool dirty_;
		YumeUIElement* parent_;
	};
}


//----------------------------------------------------------------------------
#endif

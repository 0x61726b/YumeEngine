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
#ifndef __YumeConstantBuffer_h__
#define __YumeConstantBuffer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeRenderer;

	class YumeAPIExport YumeConstantBuffer
	{
	public:
		YumeConstantBuffer();

		virtual ~YumeConstantBuffer();

		virtual void Release() = 0;


		
		virtual bool SetSize(unsigned size) = 0;
		
		void SetParameter(unsigned offset,unsigned size,const void* data);
		
		void SetVector3ArrayParameter(unsigned offset,unsigned rows,const void* data);
		
		virtual void Apply() = 0;

		
		unsigned GetSize() const { return size_; }

		
		bool IsDirty() const { return dirty_; }

	protected:
		
		boost::shared_array<unsigned char> shadowData_;
		
		unsigned size_;
		
		bool dirty_;
	};
}


//----------------------------------------------------------------------------
#endif

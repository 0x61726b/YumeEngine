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
#include "YumeConstantBuffer.h"

#include "Logging/logging.h"


namespace YumeEngine
{
	
	YumeConstantBuffer::YumeConstantBuffer()
	{
	}

	YumeConstantBuffer::~YumeConstantBuffer()
	{
		
	}

	void YumeConstantBuffer::SetParameter(unsigned offset,unsigned size,const void* data)
	{
		if(offset + size > size_)
			return; // Would overflow the buffer

		memcpy(&shadowData_[offset],data,size);
		dirty_ = true;
	}

	void YumeConstantBuffer::SetVector3ArrayParameter(unsigned offset,unsigned rows,const void* data)
	{
		if(offset + rows * 4 * sizeof(float) > size_)
			return; // Would overflow the buffer

		float* dest = (float*)&shadowData_[offset];
		const float* src = (const float*)data;

		while(rows--)
		{
			*dest++ = *src++;
			*dest++ = *src++;
			*dest++ = *src++;
			++dest; // Skip over the w coordinate
		}

		dirty_ = true;
	}
}

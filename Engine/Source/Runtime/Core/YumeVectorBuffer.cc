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
#include "YumeVectorBuffer.h"
#include "YumeFile.h"


namespace YumeEngine
{

	VectorBuffer::VectorBuffer()
	{
	}

	VectorBuffer::VectorBuffer(const YumeVector<unsigned char>::type& data)
	{
		SetData(data);
	}

	VectorBuffer::VectorBuffer(const void* data,unsigned size)
	{
		SetData(data,size);
	}

	VectorBuffer::VectorBuffer(StreamReader& source,unsigned size)
	{
		SetData(source,size);
	}

	unsigned VectorBuffer::Read(void* dest,unsigned size)
	{
		if(size + position_ > size_)
			size = size_ - position_;
		if(!size)
			return 0;

		unsigned char* srcPtr = &buffer_[position_];
		unsigned char* destPtr = (unsigned char*)dest;
		position_ += size;

		unsigned copySize = size;
		while(copySize >= sizeof(unsigned))
		{
			*((unsigned*)destPtr) = *((unsigned*)srcPtr);
			srcPtr += sizeof(unsigned);
			destPtr += sizeof(unsigned);
			copySize -= sizeof(unsigned);
		}
		if(copySize & sizeof(unsigned short))
		{
			*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
			srcPtr += sizeof(unsigned short);
			destPtr += sizeof(unsigned short);
		}
		if(copySize & 1)
			*destPtr = *srcPtr;

		return size;
	}

	unsigned VectorBuffer::Seek(unsigned position)
	{
		if(position > size_)
			position = size_;

		position_ = position;
		return position_;
	}

	unsigned VectorBuffer::Write(const void* data,unsigned size)
	{
		if(!size)
			return 0;

		if(size + position_ > size_)
		{
			size_ = size + position_;
			buffer_.resize(size_);
		}

		unsigned char* srcPtr = (unsigned char*)data;
		unsigned char* destPtr = &buffer_[position_];
		position_ += size;

		unsigned copySize = size;
		while(copySize >= sizeof(unsigned))
		{
			*((unsigned*)destPtr) = *((unsigned*)srcPtr);
			srcPtr += sizeof(unsigned);
			destPtr += sizeof(unsigned);
			copySize -= sizeof(unsigned);
		}
		if(copySize & sizeof(unsigned short))
		{
			*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
			srcPtr += sizeof(unsigned short);
			destPtr += sizeof(unsigned short);
		}
		if(copySize & 1)
			*destPtr = *srcPtr;

		return size;
	}

	void VectorBuffer::SetData(const YumeVector<unsigned char>::type& data)
	{
		buffer_ = data;
		position_ = 0;
		size_ = data.size();
	}

	void VectorBuffer::SetData(const void* data,unsigned size)
	{
		if(!data)
			size = 0;

		buffer_.resize(size);
		if(size)
			memcpy(&buffer_[0],data,size);

		position_ = 0;
		size_ = size;
	}

	void VectorBuffer::SetData(StreamReader& source,unsigned size)
	{
		buffer_.resize(size);
		unsigned actualSize = source.Read(&buffer_[0],size);
		if(actualSize != size)
			buffer_.resize(actualSize);

		position_ = 0;
		size_ = actualSize;
	}

	void VectorBuffer::Clear()
	{
		buffer_.clear();
		position_ = 0;
		size_ = 0;
	}

	void VectorBuffer::Resize(unsigned size)
	{
		buffer_.resize(size);
		size_ = size;
		if(position_ > size_)
			position_ = size_;
	}
}

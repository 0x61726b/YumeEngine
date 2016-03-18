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
#include "YumeGLIndexBuffer.h"
#include "YumeGLRendererImpl.h"
#include "YumeGLRenderer.h"

#include "Logging/logging.h"

namespace YumeEngine
{
	YumeGLIndexBuffer::YumeGLIndexBuffer()
	{

	}

	YumeGLIndexBuffer::~YumeGLIndexBuffer()
	{
		Release();
	}

	void YumeGLIndexBuffer::OnDeviceReset()
	{
		if(!object_)
		{
			Create();
			dataLost_ = !UpdateToGPU();
		}
		else if(dataPending_)
			dataLost_ = !UpdateToGPU();

		dataPending_ = false;
	}

	void YumeGLIndexBuffer::Release()
	{
		Unlock();

		if(object_)
		{
			if(!gYume->pRHI)
				return;

			if(!gYume->pRHI->IsDeviceLost())
			{
				if(gYume->pRHI->GetIndexBuffer() == this)
					gYume->pRHI->SetIndexBuffer(0);

				glDeleteBuffers(1,&object_);
			}
			object_ = 0;
		}
	}

	void YumeGLIndexBuffer::SetShadowed(bool enable)
	{
		if(enable != shadowed_)
		{
			if(enable && indexCount_ && indexSize_)
				shadowData_ = boost::shared_array<unsigned char>(new unsigned char[indexCount_ * indexSize_]);
			else
				shadowData_.reset();

			shadowed_ = enable;
		}
	}

	bool YumeGLIndexBuffer::SetSize(unsigned indexCount,bool largeIndices,bool dynamic)
	{
		Unlock();

		dynamic_ = dynamic;
		indexCount_ = indexCount;
		indexSize_ = (unsigned)(largeIndices ? sizeof(unsigned) : sizeof(unsigned short));

		if(shadowed_ && indexCount_ && indexSize_)
			shadowData_ =  boost::shared_array<unsigned char>(new unsigned char[indexCount_ * indexSize_]);
		else
			shadowData_.reset();

		return Create();
	}

	bool YumeGLIndexBuffer::SetData(const void* data)
	{
		if(!data)
		{
			YUMELOG_ERROR("Null pointer for index buffer data");
			return false;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not set index buffer data");
			return false;
		}

		if(shadowData_ && data != shadowData_.get())
			memcpy(shadowData_.get(),data,indexCount_ * indexSize_);

		if(object_)
		{
			if(!gYume->pRHI->IsDeviceLost())
			{
				gYume->pRHI->SetIndexBuffer(this);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexCount_ * indexSize_,data,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			}
			else
			{
				YUMELOG_WARN("Index buffer data assignment while device is lost");
				dataPending_ = true;
			}
		}

		dataLost_ = false;
		return true;
	}

	bool YumeGLIndexBuffer::SetDataRange(const void* data,unsigned start,unsigned count,bool discard)
	{
		if(start == 0 && count == indexCount_)
			return SetData(data);

		if(!data)
		{
			YUMELOG_ERROR("Null pointer for index buffer data");
			return false;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not set index buffer data");
			return false;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal range for setting new index buffer data");
			return false;
		}

		if(!count)
			return true;

		if(shadowData_ && shadowData_.get() + start * indexSize_ != data)
			memcpy(shadowData_.get() + start * indexSize_,data,count * indexSize_);

		if(object_)
		{
			if(!gYume->pRHI->IsDeviceLost())
			{
				gYume->pRHI->SetIndexBuffer(this);
				if(!discard || start != 0)
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,start * indexSize_,count * indexSize_,data);
				else
					glBufferData(GL_ELEMENT_ARRAY_BUFFER,count * indexSize_,data,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			}
			else
			{
				YUMELOG_WARN("Index buffer data assignment while device is lost");
				dataPending_ = true;
			}
		}
		return true;
	}

	void* YumeGLIndexBuffer::Lock(unsigned start,unsigned count,bool discard)
	{
		if(lockState_ != LOCK_NONE)
		{
			YUMELOG_ERROR("Index buffer already locked");
			return 0;
		}

		if(!indexSize_)
		{
			YUMELOG_ERROR("Index size not defined, can not lock index buffer");
			return 0;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal range for locking index buffer");
			return 0;
		}

		if(!count)
			return 0;

		lockStart_ = start;
		lockCount_ = count;

		if(shadowData_)
		{
			lockState_ = LOCK_SHADOW;
			return shadowData_.get() + start * indexSize_;
		}
		else if(gYume->pRHI)
		{
			lockState_ = LOCK_SCRATCH;
			lockScratchData_ = gYume->pRHI->ReserveScratchBuffer(count * indexSize_);
			return lockScratchData_;
		}
		else
			return 0;
	}

	void YumeGLIndexBuffer::Unlock()
	{
		switch(lockState_)
		{
		case LOCK_SHADOW:
			SetDataRange(shadowData_.get() + lockStart_ * indexSize_,lockStart_,lockCount_);
			lockState_ = LOCK_NONE;
			break;

		case LOCK_SCRATCH:
			SetDataRange(lockScratchData_,lockStart_,lockCount_);
			if(gYume->pRHI)
				gYume->pRHI->FreeScratchBuffer(lockScratchData_);
			lockScratchData_ = 0;
			lockState_ = LOCK_NONE;
			break;

		default: break;
		}
	}

	bool YumeGLIndexBuffer::GetUsedVertexRange(unsigned start,unsigned count,unsigned& minVertex,unsigned& vertexCount)
	{
		if(!shadowData_)
		{
			YUMELOG_ERROR("Used vertex range can only be queried from an index buffer with shadow data");
			return false;
		}

		if(start + count > indexCount_)
		{
			YUMELOG_ERROR("Illegal index range for querying used vertices");
			return false;
		}

		minVertex = M_MAX_UNSIGNED;
		unsigned maxVertex = 0;

		if(indexSize_ == sizeof(unsigned))
		{
			unsigned* indices = ((unsigned*)shadowData_.get()) + start;

			for(unsigned i = 0; i < count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}
		else
		{
			unsigned short* indices = ((unsigned short*)shadowData_.get()) + start;

			for(unsigned i = 0; i < count; ++i)
			{
				if(indices[i] < minVertex)
					minVertex = indices[i];
				if(indices[i] > maxVertex)
					maxVertex = indices[i];
			}
		}

		vertexCount = maxVertex - minVertex + 1;
		return true;
	}

	bool YumeGLIndexBuffer::Create()
	{
		if(!indexCount_)
		{
			Release();
			return true;
		}

		if(gYume->pRHI)
		{
			if(gYume->pRHI->IsDeviceLost())
			{
				YUMELOG_WARN("Index buffer creation while device is lost");
				return true;
			}

			if(!object_)
			{
				glGenBuffers(1,(GLuint*)&object_);
			}
			if(!object_)
			{
				YUMELOG_ERROR("Failed to create index buffer");
				return false;
			}

			gYume->pRHI->SetIndexBuffer(this);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexCount_ * indexSize_,0,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}


		return true;
	}

	bool YumeGLIndexBuffer::UpdateToGPU()
	{
		if(object_ && shadowData_)
			return SetData(shadowData_.get());
		else
			return false;
	}
}

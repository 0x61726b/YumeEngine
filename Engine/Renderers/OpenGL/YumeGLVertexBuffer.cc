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
#include "YumeGLVertexBuffer.h"
#include "YumeGLRenderer.h"
#include "YumeGLRendererImpl.h"



#include "Engine/YumeEngine.h"

#include "Logging/logging.h"


namespace YumeEngine
{

	const unsigned YumeGLVertexBuffer::elementSize[] =
	{
		3 * sizeof(float), // Position
		3 * sizeof(float), // Normal
		4 * sizeof(unsigned char), // Color
		2 * sizeof(float), // Texcoord1
		2 * sizeof(float), // Texcoord2
		3 * sizeof(float), // Cubetexcoord1
		3 * sizeof(float), // Cubetexcoord2
		4 * sizeof(float), // Tangent
		4 * sizeof(float), // Blendweights
		4 * sizeof(unsigned char), // Blendindices
		4 * sizeof(float), // Instancematrix1
		4 * sizeof(float), // Instancematrix2
		4 * sizeof(float), // Instancematrix3
		sizeof(int) // Object index
	};

	const unsigned YumeGLVertexBuffer::elementType[] =
	{
		GL_FLOAT, // Position
		GL_FLOAT, // Normal
		GL_UNSIGNED_BYTE, // Color
		GL_FLOAT, // Texcoord1
		GL_FLOAT, // Texcoord2
		GL_FLOAT, // Cubetexcoord1
		GL_FLOAT, // Cubetexcoord2
		GL_FLOAT, // Tangent
		GL_FLOAT, // Blendweights
		GL_UNSIGNED_BYTE, // Blendindices
		GL_FLOAT, // Instancematrix1
		GL_FLOAT, // Instancematrix2
		GL_FLOAT, // Instancematrix3
		GL_INT // Object index
	};

	const unsigned YumeGLVertexBuffer::elementComponents[] =
	{
		3, // Position
		3, // Normal
		4, // Color
		2, // Texcoord1
		2, // Texcoord2
		3, // Cubetexcoord1
		3, // Cubetexcoord2
		4, // Tangent
		4, // Blendweights
		4, // Blendindices
		4, // Instancematrix1
		4, // Instancematrix2
		4, // Instancematrix3
		1 // Object index
	};

	const unsigned YumeGLVertexBuffer::elementNormalize[] =
	{
		GL_FALSE, // Position
		GL_FALSE, // Normal
		GL_TRUE, // Color
		GL_FALSE, // Texcoord1
		GL_FALSE, // Texcoord2
		GL_FALSE, // Cubetexcoord1
		GL_FALSE, // Cubetexcoord2
		GL_FALSE, // Tangent
		GL_FALSE, // Blendweights
		GL_FALSE, // Blendindices
		GL_FALSE, // Instancematrix1
		GL_FALSE, // Instancematrix2
		GL_FALSE, // Instancematrix3
		GL_FALSE // Object index
	};



	YumeGLVertexBuffer::YumeGLVertexBuffer()
	{
		UpdateOffsets();
	}

	YumeGLVertexBuffer::~YumeGLVertexBuffer()
	{
		Release();
	}

	void YumeGLVertexBuffer::OnDeviceReset()
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

	void YumeGLVertexBuffer::Release()
	{
		Unlock();

		if(object_)
		{
			if(!gYume->pRHI)
				return;

			if(!gYume->pRHI->IsDeviceLost())
			{
				for(unsigned i = 0; i < MAX_VERTEX_STREAMS; ++i)
				{
					if(gYume->pRHI->GetVertexBuffer(i) == this)
						gYume->pRHI->SetVertexBuffer(0);
				}

				static_cast<YumeGLRenderer*>(gYume->pRHI)->SetVBO(0);
				glDeleteBuffers(1,(GLuint*)&object_);
			}

			object_ = 0;
		}
	}

	void YumeGLVertexBuffer::SetShadowed(bool enable)
	{
		if(enable != shadowed_)
		{
			if(enable && vertexSize_ && vertexCount_)
				shadowData_ = boost::shared_array<unsigned char>(new unsigned char[vertexCount_ * vertexSize_]);
			else
				shadowData_.reset();

			shadowed_ = enable;
		}
	}

	bool YumeGLVertexBuffer::SetSize(unsigned vertexCount,unsigned elementMask,bool dynamic)
	{
		Unlock();

		dynamic_ = dynamic;
		vertexCount_ = vertexCount;
		elementMask_ = elementMask;

		UpdateOffsets();

		if(shadowed_ && vertexCount_ && vertexSize_)
			shadowData_ = boost::shared_array<unsigned char>(new unsigned char[vertexCount_ * vertexSize_]);
		else
			shadowData_.reset();

		return Create();
	}

	bool YumeGLVertexBuffer::SetData(const void* data)
	{
		if(!data)
		{
			YUMELOG_ERROR("Null pointer for vertex buffer data");
			return false;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if(shadowData_ && data != shadowData_.get())
			memcpy(shadowData_.get(),data,vertexCount_ * vertexSize_);

		if(object_)
		{
			if(!gYume->pRHI->IsDeviceLost())
			{
				static_cast<YumeGLRenderer*>(gYume->pRHI)->SetVBO(object_);
				glBufferData(GL_ARRAY_BUFFER,vertexCount_ * vertexSize_,data,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			}
			else
			{
				YUMELOG_WARN("Vertex buffer data assignment while device is lost");
				dataPending_ = true;
			}
		}

		dataLost_ = false;
		return true;
	}

	bool YumeGLVertexBuffer::SetDataRange(const void* data,unsigned start,unsigned count,bool discard)
	{
		if(start == 0 && count == vertexCount_)
			return SetData(data);

		if(!data)
		{
			YUMELOG_ERROR("Null pointer for vertex buffer data");
			return false;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if(start + count > vertexCount_)
		{
			YUMELOG_ERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if(!count)
			return true;

		if(shadowData_ && shadowData_.get() + start * vertexSize_ != data)
			memcpy(shadowData_.get() + start * vertexSize_,data,count * vertexSize_);

		if(object_)
		{
			if(!gYume->pRHI->IsDeviceLost())
			{
				static_cast<YumeGLRenderer*>(gYume->pRHI)->SetVBO(object_);
				if(!discard || start != 0)
					glBufferSubData(GL_ARRAY_BUFFER,start * vertexSize_,count * vertexSize_,data);
				else
					glBufferData(GL_ARRAY_BUFFER,count * vertexSize_,data,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			}
			else
			{
				YUMELOG_WARN("Vertex buffer data assignment while device is lost");
				dataPending_ = true;
			}
		}

		return true;
	}

	void* YumeGLVertexBuffer::Lock(unsigned start,unsigned count,bool discard)
	{
		if(lockState_ != LOCK_NONE)
		{
			YUMELOG_ERROR("Vertex buffer already locked");
			return 0;
		}

		if(!vertexSize_)
		{
			YUMELOG_ERROR("Vertex elements not defined, can not lock vertex buffer");
			return 0;
		}

		if(start + count > vertexCount_)
		{
			YUMELOG_ERROR("Illegal range for locking vertex buffer");
			return 0;
		}

		if(!count)
			return 0;

		lockStart_ = start;
		lockCount_ = count;

		if(shadowData_)
		{
			lockState_ = LOCK_SHADOW;
			return shadowData_.get() + start * vertexSize_;
		}
		else if(gYume->pRHI)
		{
			lockState_ = LOCK_SCRATCH;
			lockScratchData_ = gYume->pRHI->ReserveScratchBuffer(count * vertexSize_);
			return lockScratchData_;
		}
		else
			return 0;
	}

	void YumeGLVertexBuffer::Unlock()
	{
		switch(lockState_)
		{
		case LOCK_SHADOW:
			SetDataRange(shadowData_.get() + lockStart_ * vertexSize_,lockStart_,lockCount_);
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

	void YumeGLVertexBuffer::UpdateOffsets()
	{
		unsigned elementOffset = 0;
		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask_ & (1 << i))
			{
				elementOffset_[i] = elementOffset;
				elementOffset += elementSize[i];
			}
			else
				elementOffset_[i] = NO_ELEMENT;
		}
		vertexSize_ = elementOffset;
	}

	unsigned YumeGLVertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned vertexSize = 0;

		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask & (1 << i))
				vertexSize += elementSize[i];
		}

		return vertexSize;
	}

	unsigned YumeGLVertexBuffer::GetElementOffset(unsigned elementMask,VertexElement element)
	{
		unsigned offset = 0;

		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(i == element)
				break;

			if(elementMask & (1 << i))
				offset += elementSize[i];
		}

		return offset;
	}

	bool YumeGLVertexBuffer::Create()
	{
		if(!vertexCount_ || !elementMask_)
		{
			Release();
			return true;
		}

		if(gYume->pRHI)
		{
			if(gYume->pRHI->IsDeviceLost())
			{
				YUMELOG_WARN("Vertex buffer creation while device is lost");
				return true;
			}

			if(!object_)
				glGenBuffers(1,(GLuint*)&object_);
			if(!object_)
			{
				YUMELOG_ERROR("Failed to create vertex buffer");
				return false;
			}

			static_cast<YumeGLRenderer*>(gYume->pRHI)->SetVBO(object_);
			glBufferData(GL_ARRAY_BUFFER,vertexCount_ * vertexSize_,0,dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}

		return true;
	}

	bool YumeGLVertexBuffer::UpdateToGPU()
	{
		if(object_ && shadowData_)
			return SetData(shadowData_.get());
		else
			return false;
	}
}

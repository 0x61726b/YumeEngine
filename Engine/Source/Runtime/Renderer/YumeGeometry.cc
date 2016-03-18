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
#include "YumeGeometry.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "Math/YumeRay.h"
#include "Engine/YumeEngine.h"

#include "YumeRHI.h"

#include "Logging/logging.h"



namespace YumeEngine
{
	YumeHash YumeGeometry::type_ = "Geometry";
	YumeGeometry::YumeGeometry():
		primitiveType_(TRIANGLE_LIST),
		indexStart_(0),
		indexCount_(0),
		vertexStart_(0),
		vertexCount_(0),
		positionBufferIndex_(M_MAX_UNSIGNED),
		rawVertexSize_(0),
		rawElementMask_(0),
		rawIndexSize_(0),
		lodDistance_(0.0f)
	{
		SetNumVertexBuffers(1);
	}

	YumeGeometry::~YumeGeometry()
	{
	}

	bool YumeGeometry::SetNumVertexBuffers(unsigned num)
	{
		if(num >= MAX_VERTEX_STREAMS)
		{
			YUMELOG_ERROR("Too many vertex streams");
			return false;
		}

		unsigned oldSize = vertexBuffers_.size();
		vertexBuffers_.resize(num);
		elementMasks_.resize(num);

		for(unsigned i = oldSize; i < num; ++i)
			elementMasks_[i] = MASK_NONE;

		GetPositionBufferIndex();
		return true;
	}

	bool YumeGeometry::SetVertexBuffer(unsigned index,SharedPtr<YumeVertexBuffer> buffer,unsigned elementMask)
	{
		if(index >= vertexBuffers_.size())
		{
			YUMELOG_ERROR("Stream index out of bounds");
			return false;
		}

		YumeRHI* rhi_ = gYume->pRHI;
		vertexBuffers_[index] = buffer;

		if(buffer)
		{
			if(elementMask == MASK_DEFAULT)
				elementMasks_[index] = buffer->GetElementMask();
			else
				elementMasks_[index] = elementMask;
		}

		GetPositionBufferIndex();
		return true;
	}

	void YumeGeometry::SetIndexBuffer(SharedPtr<YumeIndexBuffer> buffer)
	{
		indexBuffer_ = buffer;
	}

	bool YumeGeometry::SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,bool getUsedVertexRange)
	{
		if(!indexBuffer_ && !rawIndexData_)
		{
			YUMELOG_ERROR("Null index buffer and no raw index data, can not define indexed draw range");
			return false;
		}
		if(indexBuffer_ && indexStart + indexCount > indexBuffer_->GetIndexCount())
		{
			YUMELOG_ERROR("Illegal draw range " + std::to_string(indexStart) + " to " + std::to_string(indexStart + indexCount - 1) + ", index buffer has " +
				std::to_string(indexBuffer_->GetIndexCount()) + " indices");
			return false;
		}

		primitiveType_ = type;
		indexStart_ = indexStart;
		indexCount_ = indexCount;

		// Get min.vertex index and num of vertices from index buffer. If it fails, use full range as fallback
		if(indexCount)
		{
			vertexStart_ = 0;
			vertexCount_ = vertexBuffers_[0] ? vertexBuffers_[0]->GetVertexCount() : 0;

			if(getUsedVertexRange && indexBuffer_)
				indexBuffer_->GetUsedVertexRange(indexStart_,indexCount_,vertexStart_,vertexCount_);
		}
		else
		{
			vertexStart_ = 0;
			vertexCount_ = 0;
		}

		return true;
	}

	bool YumeGeometry::SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned minVertex,unsigned vertexCount,
		bool checkIllegal)
	{
		if(indexBuffer_)
		{
			// We can allow setting an illegal draw range now if the caller guarantees to resize / fill the buffer later
			if(checkIllegal && indexStart + indexCount > indexBuffer_->GetIndexCount())
			{
				YUMELOG_ERROR("Illegal draw range " + std::to_string(indexStart) + " to " + std::to_string(indexStart + indexCount - 1) +
					", index buffer has " + std::to_string(indexBuffer_->GetIndexCount()) + " indices");
				return false;
			}
		}
		else if(!rawIndexData_)
		{
			indexStart = 0;
			indexCount = 0;
		}

		primitiveType_ = type;
		indexStart_ = indexStart;
		indexCount_ = indexCount;
		vertexStart_ = minVertex;
		vertexCount_ = vertexCount;

		return true;
	}

	void YumeGeometry::SetLodDistance(float distance)
	{
		if(distance < 0.0f)
			distance = 0.0f;

		lodDistance_ = distance;
	}

	void YumeGeometry::SetRawVertexData(boost::shared_array<unsigned char> data,unsigned vertexSize,unsigned elementMask)
	{
		rawVertexData_ = data;
		rawVertexSize_ = vertexSize;
		rawElementMask_ = elementMask;
	}

	void YumeGeometry::SetRawIndexData(boost::shared_array<unsigned char> data,unsigned indexSize)
	{
		rawIndexData_ = data;
		rawIndexSize_ = indexSize;
	}

	void YumeGeometry::Draw(YumeRHI* graphics)
	{
		if(indexBuffer_ && indexCount_ > 0)
		{
			graphics->SetIndexBuffer(indexBuffer_);
			graphics->SetVertexBuffers(vertexBuffers_,elementMasks_);
			graphics->Draw(primitiveType_,indexStart_,indexCount_,vertexStart_,vertexCount_);
		}
		else if(vertexCount_ > 0)
		{
			graphics->SetVertexBuffers(vertexBuffers_,elementMasks_);
			graphics->Draw(primitiveType_,vertexStart_,vertexCount_);
		}
	}

	YumeVertexBuffer* YumeGeometry::GetVertexBuffer(unsigned index) const
	{
		return index < vertexBuffers_.size() ? vertexBuffers_[index] : (YumeVertexBuffer*)0;
	}

	unsigned YumeGeometry::GetVertexElementMask(unsigned index) const
	{
		return index < elementMasks_.size() ? elementMasks_[index] : 0;
	}

	unsigned short YumeGeometry::GetBufferHash() const
	{
		unsigned short hash = 0;

		for(unsigned i = 0; i < vertexBuffers_.size(); ++i)
		{
			YumeVertexBuffer* vBuf = vertexBuffers_[i];
			hash += *((unsigned short*)&vBuf);
		}

		YumeIndexBuffer* iBuf = indexBuffer_;
		hash += *((unsigned short*)&iBuf);

		return hash;
	}

	void YumeGeometry::GetRawData(const unsigned char*& vertexData,unsigned& vertexSize,const unsigned char*& indexData,
		unsigned& indexSize,unsigned& elementMask) const
	{
		if(rawVertexData_)
		{
			vertexData = rawVertexData_.get();
			vertexSize = rawVertexSize_;
			elementMask = rawElementMask_;
		}
		else
		{
			if(positionBufferIndex_ < vertexBuffers_.size() && vertexBuffers_[positionBufferIndex_])
			{
				vertexData = vertexBuffers_[positionBufferIndex_]->GetShadowData();
				if(vertexData)
				{
					vertexSize = vertexBuffers_[positionBufferIndex_]->GetVertexSize();
					elementMask = vertexBuffers_[positionBufferIndex_]->GetElementMask();
				}
				else
				{
					vertexSize = 0;
					elementMask = 0;
				}
			}
			else
			{
				vertexData = 0;
				vertexSize = 0;
				elementMask = 0;
			}
		}

		if(rawIndexData_)
		{
			indexData = rawIndexData_.get();
			indexSize = rawIndexSize_;
		}
		else
		{
			if(indexBuffer_)
			{
				indexData = indexBuffer_->GetShadowData();
				if(indexData)
					indexSize = indexBuffer_->GetIndexSize();
				else
					indexSize = 0;
			}
			else
			{
				indexData = 0;
				indexSize = 0;
			}
		}
	}

	void YumeGeometry::GetRawDataShared(boost::shared_array<unsigned char>& vertexData,unsigned& vertexSize,
		boost::shared_array<unsigned char>& indexData,unsigned& indexSize,unsigned& elementMask) const
	{
		if(rawVertexData_)
		{
			vertexData = rawVertexData_;
			vertexSize = rawVertexSize_;
			elementMask = rawElementMask_;
		}
		else
		{
			if(positionBufferIndex_ < vertexBuffers_.size() && vertexBuffers_[positionBufferIndex_])
			{
				vertexData = vertexBuffers_[positionBufferIndex_]->GetShadowDataShared();
				if(vertexData)
				{
					vertexSize = vertexBuffers_[positionBufferIndex_]->GetVertexSize();
					elementMask = vertexBuffers_[positionBufferIndex_]->GetElementMask();
				}
				else
				{
					vertexSize = 0;
					elementMask = 0;
				}
			}
			else
			{
				vertexData = 0;
				vertexSize = 0;
				elementMask = 0;
			}
		}

		if(rawIndexData_)
		{
			indexData = rawIndexData_;
			indexSize = rawIndexSize_;
		}
		else
		{
			if(indexBuffer_)
			{
				indexData = indexBuffer_->GetShadowDataShared();
				if(indexData)
					indexSize = indexBuffer_->GetIndexSize();
				else
					indexSize = 0;
			}
			else
			{
				indexData = 0;
				indexSize = 0;
			}
		}
	}

	float YumeGeometry::GetHitDistance(const Ray& ray,Vector3* outNormal,Vector2* outUV) const
	{
		const unsigned char* vertexData;
		const unsigned char* indexData;
		unsigned vertexSize;
		unsigned indexSize;
		unsigned elementMask;
		unsigned uvOffset = 0;

		GetRawData(vertexData,vertexSize,indexData,indexSize,elementMask);

		if(vertexData)
		{
			if(outUV)
			{
				if((elementMask & MASK_TEXCOORD1) == 0)
				{
					// requested UV output, but no texture data in vertex buffer
					YUMELOG_WARN("Illegal GetHitDistance call: UV return requested on vertex buffer without UV coords");
					*outUV = Vector2::ZERO;
					outUV = 0;
				}
				else
					uvOffset = YumeVertexBuffer::GetElementOffset(elementMask,ELEMENT_TEXCOORD1);
			}

			return indexData ? ray.HitDistance(vertexData,vertexSize,indexData,indexSize,indexStart_,indexCount_,outNormal,outUV,
				uvOffset) :
				ray.HitDistance(vertexData,vertexSize,vertexStart_,vertexCount_,outNormal,outUV,uvOffset);
		}

		return M_INFINITY;
	}

	bool YumeGeometry::IsInside(const Ray& ray) const
	{
		const unsigned char* vertexData;
		const unsigned char* indexData;
		unsigned vertexSize;
		unsigned indexSize;
		unsigned elementMask;

		GetRawData(vertexData,vertexSize,indexData,indexSize,elementMask);

		return vertexData ? (indexData ? ray.InsideGeometry(vertexData,vertexSize,indexData,indexSize,indexStart_,indexCount_) :
			ray.InsideGeometry(vertexData,vertexSize,vertexStart_,vertexCount_)) : false;
	}

	void YumeGeometry::GetPositionBufferIndex()
	{
		for(unsigned i = 0; i < vertexBuffers_.size(); ++i)
		{
			if(vertexBuffers_[i] && vertexBuffers_[i]->GetElementMask() & MASK_POSITION)
			{
				positionBufferIndex_ = i;
				return;
			}
		}

		// No vertex buffer with positions
		positionBufferIndex_ = M_MAX_UNSIGNED;
	}
}

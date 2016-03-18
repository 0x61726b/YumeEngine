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
#ifndef __YumeGeometry_h__
#define __YumeGeometry_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeRendererDefs.h"
#include "Core/YumeBase.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeIndexBuffer;
	class Ray;
	class YumeRHI;
	class YumeVertexBuffer;

	
	class YumeAPIExport YumeGeometry : public YumeBase
	{
	public:		
		YumeGeometry();
		virtual ~YumeGeometry();

		static YumeHash type_;
		static YumeHash GetTypeStatic() { return type_; }
		virtual YumeHash GetType() { return type_; }


	
		bool SetNumVertexBuffers(unsigned num);
		bool SetVertexBuffer(unsigned index,SharedPtr<YumeVertexBuffer> buffer,unsigned elementMask = MASK_DEFAULT);
		void SetIndexBuffer(SharedPtr<YumeIndexBuffer> buffer);
		bool SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,bool getUsedVertexRange = true);
		bool SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned vertexStart,unsigned vertexCount,
			bool checkIllegal = true);
		void SetLodDistance(float distance);
		void SetRawVertexData(boost::shared_array<unsigned char> data,unsigned vertexSize,unsigned elementMask);
		void SetRawIndexData(boost::shared_array<unsigned char> data,unsigned indexSize);
		
		void Draw(YumeRHI* graphics);

		const YumeVector<SharedPtr<YumeVertexBuffer> >::type& GetVertexBuffers() const { return vertexBuffers_; }		
		const YumeVector<unsigned>::type& GetVertexElementMasks() const { return elementMasks_; }
		unsigned GetNumVertexBuffers() const { return vertexBuffers_.size(); }
		YumeVertexBuffer* GetVertexBuffer(unsigned index) const;
		unsigned GetVertexElementMask(unsigned index) const;
		YumeIndexBuffer* GetIndexBuffer() const { return indexBuffer_; }

		
		PrimitiveType GetPrimitiveType() const { return primitiveType_; }
		
		unsigned GetIndexStart() const { return indexStart_; }		
		unsigned GetIndexCount() const { return indexCount_; }		
		unsigned GetVertexStart() const { return vertexStart_; }		
		unsigned GetVertexCount() const { return vertexCount_; }		
		float GetLodDistance() const { return lodDistance_; }		
		unsigned short GetBufferHash() const;		
		void GetRawData(const unsigned char*& vertexData,unsigned& vertexSize,const unsigned char*& indexData,unsigned& indexSize,			unsigned& elementMask) const;		
		void GetRawDataShared(boost::shared_array<unsigned char>& vertexData,unsigned& vertexSize,boost::shared_array<unsigned char>& indexData,			unsigned& indexSize,unsigned& elementMask) const;		
		float GetHitDistance(const Ray& ray,Vector3* outNormal = 0,Vector2* outUV = 0) const;		
		bool IsInside(const Ray& ray) const;		
		bool IsEmpty() const { return indexCount_ == 0 && vertexCount_ == 0; }
	private:
		void GetPositionBufferIndex();		
		YumeVector<SharedPtr<YumeVertexBuffer> >::type vertexBuffers_;		
		YumeVector<unsigned>::type elementMasks_;		
		SharedPtr<YumeIndexBuffer> indexBuffer_;		
		boost::shared_array<unsigned char> rawVertexData_;		
		boost::shared_array<unsigned char> rawIndexData_;		
		PrimitiveType primitiveType_;		
		unsigned indexStart_;		
		unsigned indexCount_;		
		unsigned vertexStart_;		
		unsigned vertexCount_;		
		unsigned positionBufferIndex_;		
		unsigned rawVertexSize_;		
		unsigned rawElementMask_;		
		unsigned rawIndexSize_;		
		float lodDistance_;
	};
}


//----------------------------------------------------------------------------
#endif

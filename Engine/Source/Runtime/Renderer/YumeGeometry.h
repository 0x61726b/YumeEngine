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

	/// Defines one or more vertex buffers, an index buffer and a draw range.
	class YumeAPIExport YumeGeometry : public YumeBase
	{
	public:
		/// Construct with one empty vertex buffer.
		YumeGeometry();
		/// Destruct.
		virtual ~YumeGeometry();

		/// Set number of vertex buffer.
		bool SetNumVertexBuffers(unsigned num);
		/// Set a vertex buffer by index.
		bool SetVertexBuffer(unsigned index,YumeVertexBuffer* buffer,unsigned elementMask = MASK_DEFAULT);
		/// Set the index buffer.
		void SetIndexBuffer(YumeIndexBuffer* buffer);
		/// Set the draw range.
		bool SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,bool getUsedVertexRange = true);
		/// Set the draw range.
		bool SetDrawRange(PrimitiveType type,unsigned indexStart,unsigned indexCount,unsigned vertexStart,unsigned vertexCount,
			bool checkIllegal = true);
		/// Set the LOD distance.
		void SetLodDistance(float distance);
		/// Override raw vertex data to be returned for CPU-side operations.
		void SetRawVertexData(boost::shared_array<unsigned char> data,unsigned vertexSize,unsigned elementMask);
		/// Override raw index data to be returned for CPU-side operations.
		void SetRawIndexData(boost::shared_array<unsigned char> data,unsigned indexSize);
		/// Draw.
		void Draw(YumeRHI* graphics);

		/// Return all vertex buffers.
		const YumeVector<SharedPtr<YumeVertexBuffer> >::type& GetVertexBuffers() const { return vertexBuffers_; }

		/// Return vertex element masks.
		const YumeVector<unsigned>::type& GetVertexElementMasks() const { return elementMasks_; }

		/// Return number of vertex buffers.
		unsigned GetNumVertexBuffers() const { return vertexBuffers_.size(); }

		/// Return vertex buffer by index.
		YumeVertexBuffer* GetVertexBuffer(unsigned index) const;
		/// Return vertex element mask by index.
		unsigned GetVertexElementMask(unsigned index) const;

		/// Return the index buffer.
		YumeIndexBuffer* GetIndexBuffer() const { return indexBuffer_.get(); }

		/// Return primitive type.
		PrimitiveType GetPrimitiveType() const { return primitiveType_; }

		/// Return start index.
		unsigned GetIndexStart() const { return indexStart_; }

		/// Return number of indices.
		unsigned GetIndexCount() const { return indexCount_; }

		/// Return first used vertex.
		unsigned GetVertexStart() const { return vertexStart_; }

		/// Return number of used vertices.
		unsigned GetVertexCount() const { return vertexCount_; }

		/// Return LOD distance.
		float GetLodDistance() const { return lodDistance_; }

		/// Return buffers' combined hash value for state sorting.
		unsigned short GetBufferHash() const;
		/// Return raw vertex and index data for CPU operations, or null pointers if not available.
		void GetRawData(const unsigned char*& vertexData,unsigned& vertexSize,const unsigned char*& indexData,unsigned& indexSize,
			unsigned& elementMask) const;
		/// Return raw vertex and index data for CPU operations, or null pointers if not available.
		void GetRawDataShared(boost::shared_array<unsigned char>& vertexData,unsigned& vertexSize,boost::shared_array<unsigned char>& indexData,
			unsigned& indexSize,unsigned& elementMask) const;
		/// Return ray hit distance or infinity if no hit. Requires raw data to be set. Optionally return hit normal and hit uv coordinates at intersect point.
		float GetHitDistance(const Ray& ray,Vector3* outNormal = 0,Vector2* outUV = 0) const;
		/// Return whether or not the ray is inside geometry.
		bool IsInside(const Ray& ray) const;

		/// Return whether has empty draw range.
		bool IsEmpty() const { return indexCount_ == 0 && vertexCount_ == 0; }

	private:
		/// Locate vertex buffer with position data.
		void GetPositionBufferIndex();

		/// Vertex buffers.
		YumeVector<SharedPtr<YumeVertexBuffer> >::type vertexBuffers_;
		/// Vertex element masks.
		YumeVector<unsigned>::type elementMasks_;
		/// Index buffer.
		SharedPtr<YumeIndexBuffer> indexBuffer_;
		/// Raw vertex data override.
		boost::shared_array<unsigned char> rawVertexData_;
		/// Raw index data override.
		boost::shared_array<unsigned char> rawIndexData_;
		/// Primitive type.
		PrimitiveType primitiveType_;
		/// Start index.
		unsigned indexStart_;
		/// Number of indices.
		unsigned indexCount_;
		/// First used vertex.
		unsigned vertexStart_;
		/// Number of used vertices.
		unsigned vertexCount_;
		/// Index of vertex buffer with position data.
		unsigned positionBufferIndex_;
		/// Raw vertex data override size.
		unsigned rawVertexSize_;
		/// Raw vertex data override element mask.
		unsigned rawElementMask_;
		/// Raw index data override size.
		unsigned rawIndexSize_;
		/// LOD distance.
		float lodDistance_;
	};
}


//----------------------------------------------------------------------------
#endif

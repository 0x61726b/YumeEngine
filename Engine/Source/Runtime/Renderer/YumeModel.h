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
#ifndef __YumeModel_h__
#define __YumeModel_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRendererDefs.h"
#include "Math/YumeBoundingBox.h"
#include "YumeResource.h"
#include "YumeSkeleton.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGeometry;
	class YumeIndexBuffer;
	class YumeRHI;
	class YumeVertexBuffer;
	class YumeSkeleton;


	struct VertexBufferMorph
	{
		unsigned elementMask_;
		unsigned vertexCount_;
		unsigned dataSize_;
		boost::shared_array<unsigned char> morphData_;
	};


	struct ModelMorph
	{
		YumeString name_;
		YumeHash nameHash_;
		float weight_;
		YumeMap<unsigned,VertexBufferMorph>::type buffers_;
	};

	struct VertexBufferDesc
	{
		unsigned vertexCount_;
		unsigned elementMask_;
		unsigned dataSize_;
		boost::shared_array<unsigned char> data_;
	};

	struct IndexBufferDesc
	{
		unsigned indexCount_;
		unsigned indexSize_;
		unsigned dataSize_;
		boost::shared_array<unsigned char> data_;
	};

	struct GeometryDesc
	{
		PrimitiveType type_;
		unsigned vbRef_;
		unsigned ibRef_;
		unsigned indexStart_;
		unsigned indexCount_;
	};

	class YumeAPIExport YumeModel : public YumeResource
	{
	public:
		YumeModel();
		virtual ~YumeModel();

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;

		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);
		/// Finish resource loading. Always called from the main thread. Return true if successful.
		virtual bool EndLoad();

		/// Set local-space bounding box.
		void SetBoundingBox(const BoundingBox& box);
		/// Set vertex buffers and their morph ranges.
		bool SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& morphRangeStarts,
			const YumeVector<unsigned>::type& morphRangeCounts);
		/// Set index buffers.
		bool SetIndexBuffers(const YumeVector<SharedPtr<YumeIndexBuffer> >::type& buffers);
		/// Set number of geometries.
		void SetNumGeometries(unsigned num);
		/// Set number of LOD levels in a geometry.
		bool SetNumGeometryLodLevels(unsigned index,unsigned num);
		/// Set geometry.
		bool SetGeometry(unsigned index,unsigned lodLevel,YumeGeometry* geometry);
		/// Set geometry center.
		bool SetGeometryCenter(unsigned index,const Vector3& center);
		/// Set bone mappings when model has more bones than the skinning shader can handle.
		void SetGeometryBoneMappings(const YumeVector<YumeVector<unsigned>::type >::type& mappings);
		/// Set vertex morphs.
		void SetMorphs(const YumeVector<ModelMorph>::type& morphs);
		/// Clone the model. The geometry data is deep-copied and can be modified in the clone without affecting the original.
		SharedPtr<YumeModel> Clone(const YumeString& cloneName = EmptyString) const;

		/// Return bounding box.
		const BoundingBox& GetBoundingBox() const { return boundingBox_; }

		/// Return vertex buffers.
		const YumeVector<SharedPtr<YumeVertexBuffer> >::type& GetVertexBuffers() const { return vertexBuffers_; }

		/// Return index buffers.
		const YumeVector<SharedPtr<YumeIndexBuffer> >::type& GetIndexBuffers() const { return indexBuffers_; }

		/// Return number of geometries.
		unsigned GetNumGeometries() const { return geometries_.size(); }

		/// Return number of LOD levels in geometry.
		unsigned GetNumGeometryLodLevels(unsigned index) const;

		/// Return geometry pointers.
		const YumeVector<YumeVector<SharedPtr<YumeGeometry> >::type >::type& GetGeometries() const { return geometries_; }

		/// Return geometry center points.
		const YumeVector<Vector3>::type& GetGeometryCenters() const { return geometryCenters_; }

		/// Return geometry by index and LOD level. The LOD level is clamped if out of range.
		YumeGeometry* GetGeometry(unsigned index,unsigned lodLevel) const;

		/// Return geometry center by index.
		const Vector3& GetGeometryCenter(unsigned index) const
		{
			return index < geometryCenters_.size() ? geometryCenters_[index] : Vector3::ZERO;
		}

		/// Return geometery bone mappings.
		const YumeVector<YumeVector<unsigned>::type >::type& GetGeometryBoneMappings() const { return geometryBoneMappings_; }

		/// Return vertex morphs.
		const YumeVector<ModelMorph>::type& GetMorphs() const { return morphs_; }

		/// Return number of vertex morphs.
		unsigned GetNumMorphs() const { return morphs_.size(); }

		/// Return vertex morph by index.
		const ModelMorph* GetMorph(unsigned index) const;
		/// Return vertex morph by name.
		const ModelMorph* GetMorph(const YumeString& name) const;
		/// Return vertex morph by name hash.
		const ModelMorph* GetMorph(YumeHash nameHash) const;
		/// Return vertex buffer morph range start.
		unsigned GetMorphRangeStart(unsigned bufferIndex) const;
		/// Return vertex buffer morph range vertex count.
		unsigned GetMorphRangeCount(unsigned bufferIndex) const;

	private:
		/// Bounding box.
		BoundingBox boundingBox_;
		YumeSkeleton skeleton_;
		/// Vertex buffers.
		YumeVector<SharedPtr<YumeVertexBuffer> >::type vertexBuffers_;
		/// Index buffers.
		YumeVector<SharedPtr<YumeIndexBuffer> >::type indexBuffers_;
		/// Geometries.
		YumeVector<YumeVector<SharedPtr<YumeGeometry> >::type >::type geometries_;
		/// Geometry bone mappings.
		YumeVector<YumeVector<unsigned>::type >::type geometryBoneMappings_;
		/// Geometry centers.
		YumeVector<Vector3>::type geometryCenters_;
		/// Vertex morphs.
		YumeVector<ModelMorph>::type morphs_;
		/// Vertex buffer morph range start.
		YumeVector<unsigned>::type morphRangeStarts_;
		/// Vertex buffer morph range vertex count.
		YumeVector<unsigned>::type morphRangeCounts_;
		/// Vertex buffer data for asynchronous loading.
		YumeVector<VertexBufferDesc>::type loadVBData_;
		/// Index buffer data for asynchronous loading.
		YumeVector<IndexBufferDesc>::type loadIBData_;
		/// Geometry definitions for asynchronous loading.
		YumeVector<YumeVector<GeometryDesc>::type >::type loadGeometries_;
	};

}


//----------------------------------------------------------------------------
#endif

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
#ifndef __YumeStaticModel_h__
#define __YumeStaticModel_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeDrawable.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeModel;

	/// Static model per-geometry extra data.
	struct StaticModelGeometryData
	{
		/// Geometry center.
		Vector3 center_;
		/// Current LOD level.
		unsigned lodLevel_;
	};

	/// Static model component.
	class YumeAPIExport YumeStaticModel : public YumeDrawable
	{
	public:
		YumeStaticModel();
		~YumeStaticModel();

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;


		/// Process octree raycast. May be called from a worker thread.
		virtual void ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results);
		/// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
		virtual void UpdateBatches(const FrameInfo& frame);
		/// Return the geometry for a specific LOD level.
		virtual YumeGeometry* GetLodGeometry(unsigned batchIndex,unsigned level);
		/// Return number of occlusion geometry triangles.
		virtual unsigned GetNumOccluderTriangles();
		/// Draw to occlusion buffer. Return true if did not run out of triangles.
		virtual bool DrawOcclusion(OcclusionBuffer* buffer);

		/// Set model.
		void SetModel(YumeModel* model);
		/// Set material on all geometries.
		void SetMaterial(YumeMaterial* material);
		/// Set material on one geometry. Return true if successful.
		bool SetMaterial(unsigned index,YumeMaterial* material);
		/// Set occlusion LOD level. By default (M_MAX_UNSIGNED) same as visible.
		void SetOcclusionLodLevel(unsigned level);
		/// Apply default materials from a material list file. If filename is empty (default), the model's resource name with extension .txt will be used.
		void ApplyMaterialList(const YumeString& fileName = EmptyString);

		/// Return model.
		YumeModel* GetModel() const { return model_; }

		/// Return number of geometries.
		unsigned GetNumGeometries() const { return geometries_.size(); }

		/// Return material by geometry index.
		YumeMaterial* GetMaterial(unsigned index = 0) const;

		/// Return occlusion LOD level.
		unsigned GetOcclusionLodLevel() const { return occlusionLodLevel_; }

		/// Determines if the given world space point is within the model geometry.
		bool IsInside(const Vector3& point) const;
		/// Determines if the given local space point is within the model geometry.
		bool IsInsideLocal(const Vector3& point) const;

	protected:
		/// Recalculate the world-space bounding box.
		virtual void OnWorldBoundingBoxUpdate();
		/// Set local-space bounding box.
		void SetBoundingBox(const BoundingBox& box);
		/// Set number of geometries.
		void SetNumGeometries(unsigned num);
		/// Reset LOD levels.
		void ResetLodLevels();
		/// Choose LOD levels based on distance.
		void CalculateLodLevels();

		/// Extra per-geometry data.
		YumeVector<StaticModelGeometryData>::type geometryData_;
		/// All geometries.
		YumeVector<YumeVector<SharedPtr<YumeGeometry> >::type >::type geometries_;
		/// Model.
		SharedPtr<YumeModel> model_;
		/// Occlusion LOD level.
		unsigned occlusionLodLevel_;
	};
}


//----------------------------------------------------------------------------
#endif

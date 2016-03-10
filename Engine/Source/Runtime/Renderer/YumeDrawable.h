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
#ifndef __YumeDrawable_h__
#define __YumeDrawable_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRendererDefs.h"
#include "Math/YumeBoundingBox.h"
#include "Scene/YumeSceneComponent.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const unsigned DRAWABLE_GEOMETRY = 0x1;
	static const unsigned DRAWABLE_LIGHT = 0x2;
	static const unsigned DRAWABLE_ZONE = 0x4;
	static const unsigned DRAWABLE_GEOMETRY2D = 0x8;
	static const unsigned DRAWABLE_ANY = 0xff;
	static const unsigned DEFAULT_VIEWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_LIGHTMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_SHADOWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_ZONEMASK = M_MAX_UNSIGNED;
	static const int MAX_VERTEX_LIGHTS = 4;
	static const float ANIMATION_LOD_BASESCALE = 2500.0f;


	class YumeCamera;
	class YumeFile;
	class YumeGeometry;
	class YumeLight;
	class YumeMaterial;
	class OcclusionBuffer;
	class Octant;
	class RayOctreeQuery;
	class YumeRendererEnvironment;
	struct RayQueryResult;
	struct WorkItem;

	
	enum UpdateGeometryType
	{
		UPDATE_NONE = 0,
		UPDATE_MAIN_THREAD,
		UPDATE_WORKER_THREAD
	};

	
	struct FrameInfo
	{
		
		unsigned frameNumber_;
		
		float timeStep_;
		
		IntVector2 viewSize_;
		
		YumeCamera* camera_;
	};

	
	struct YumeAPIExport SourceBatch
	{
		
		SourceBatch();
		
		SourceBatch(const SourceBatch& batch);
		
		~SourceBatch();

		
		SourceBatch& operator =(const SourceBatch& rhs);

		
		float distance_;
		
		YumeGeometry* geometry_;
		
		SharedPtr<YumeMaterial> material_;
		
		const Matrix3x4* worldTransform_;
		
		unsigned numWorldTransforms_;
		
		GeometryType geometryType_;
	};

	
	class YumeAPIExport YumeDrawable : public YumeSceneComponent
	{
		friend class Octant;
		friend class Octree;
		friend void UpdateDrawablesWork(const WorkItem* item,unsigned threadIndex);

	public:
		
		YumeDrawable(unsigned char drawableFlags);
		
		virtual ~YumeDrawable();

		
		virtual void OnSetEnabled();
		
		virtual void ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results);
		
		virtual void Update(const FrameInfo& frame);
		
		virtual void UpdateBatches(const FrameInfo& frame);
		
		virtual void UpdateGeometry(const FrameInfo& frame);

		
		virtual UpdateGeometryType GetUpdateGeometryType() { return UPDATE_NONE; }

		
		virtual YumeGeometry* GetLodGeometry(unsigned batchIndex,unsigned level);

		
		virtual unsigned GetNumOccluderTriangles() { return 0; }

		
		virtual bool DrawOcclusion(OcclusionBuffer* buffer);
		
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		
		void SetDrawDistance(float distance);
		
		void SetShadowDistance(float distance);
		
		void SetLodBias(float bias);
		
		void SetViewMask(unsigned mask);
		
		void SetLightMask(unsigned mask);
		
		void SetShadowMask(unsigned mask);
		
		void SetZoneMask(unsigned mask);
		
		void SetMaxLights(unsigned num);
		
		void SetCastShadows(bool enable);
		
		void SetOccluder(bool enable);
		
		void SetOccludee(bool enable);
		
		void MarkForUpdate();

		
		const BoundingBox& GetBoundingBox() const { return boundingBox_; }

		
		const BoundingBox& GetWorldBoundingBox();

		
		unsigned char GetDrawableFlags() const { return drawableFlags_; }

		
		float GetDrawDistance() const { return drawDistance_; }

		
		float GetShadowDistance() const { return shadowDistance_; }

		
		float GetLodBias() const { return lodBias_; }

		
		unsigned GetViewMask() const { return viewMask_; }

		
		unsigned GetLightMask() const { return lightMask_; }

		
		unsigned GetShadowMask() const { return shadowMask_; }

		
		unsigned GetZoneMask() const { return zoneMask_; }

		
		unsigned GetMaxLights() const { return maxLights_; }

		
		bool GetCastShadows() const { return castShadows_; }

		
		bool IsOccluder() const { return occluder_; }

		
		bool IsOccludee() const { return occludee_; }

		
		bool IsInView() const;
		
		bool IsInView(YumeCamera* camera) const;

		
		const YumeVector<SourceBatch>::type& GetBatches() const { return batches_; }

		
		void SetZone(YumeRendererEnvironment* zone,bool temporary = false);
		
		void SetSortValue(float value);

		
		void SetMinMaxZ(float minZ,float maxZ)
		{
			minZ_ = minZ;
			maxZ_ = maxZ;
		}

		
		void MarkInView(const FrameInfo& frame);
		
		void MarkInView(unsigned frameNumber);
		
		void LimitLights();
		
		void LimitVertexLights(bool removeConvertedLights);

		
		void SetBasePass(unsigned batchIndex) { basePassFlags_ |= (1 << batchIndex); }

		
		Octant* GetOctant() const { return octant_; }

		
		YumeRendererEnvironment* GetZone() const { return zone_; }

		
		bool IsZoneDirty() const { return zoneDirty_; }

		
		float GetDistance() const { return distance_; }

		
		float GetLodDistance() const { return lodDistance_; }

		
		float GetSortValue() const { return sortValue_; }

		
		bool IsInView(const FrameInfo& frame,bool anyCamera = false) const;

		
		bool HasBasePass(unsigned batchIndex) const { return (basePassFlags_ & (1 << batchIndex)) != 0; }

		
		const YumeVector<YumeLight*>::type& GetLights() const { return lights_; }

		
		const YumeVector<YumeLight*>::type& GetVertexLights() const { return vertexLights_; }

		
		YumeLight* GetFirstLight() const { return firstLight_; }

		
		float GetMinZ() const { return minZ_; }

		
		float GetMaxZ() const { return maxZ_; }

		// Add a per-pixel light affecting the object this frame.
		void AddLight(YumeLight* light)
		{
			if(!firstLight_)
				firstLight_ = light;

			// Need to store into the light list only if the per-pixel lights are being limited.
			// Otherwise recording the first light is enough
			if(maxLights_)
				lights_.push_back(light);
		}

		// Add a per-vertex light affecting the object this frame.
		void AddVertexLight(YumeLight* light)
		{
			vertexLights_.push_back(light);
		}

	protected:
		
		virtual void OnNodeSet(YumeSceneNode* node);
		
		virtual void OnSceneSet(YumeScene* scene);
		
		virtual void OnMarkedDirty(YumeSceneNode* node);
		
		virtual void OnWorldBoundingBoxUpdate() = 0;

		
		virtual void OnRemoveFromOctree() { }

		
		void AddToOctree();
		
		void RemoveFromOctree();

		
		void SetOctant(Octant* octant) { octant_ = octant; }

		
		BoundingBox worldBoundingBox_;
		
		BoundingBox boundingBox_;
		
		YumeVector<SourceBatch>::type batches_;
		
		unsigned char drawableFlags_;
		
		bool worldBoundingBoxDirty_;
		
		bool castShadows_;
		
		bool occluder_;
		
		bool occludee_;
		
		bool updateQueued_;
		
		bool zoneDirty_;
		
		Octant* octant_;
		
		YumeRendererEnvironment* zone_;
		
		unsigned viewMask_;
		
		unsigned lightMask_;
		
		unsigned shadowMask_;
		
		unsigned zoneMask_;
		
		unsigned viewFrameNumber_;
		
		float distance_;
		
		float lodDistance_;
		
		float drawDistance_;
		
		float shadowDistance_;
		
		float sortValue_;
		
		float minZ_;
		
		float maxZ_;
		
		float lodBias_;
		
		unsigned basePassFlags_;
		
		unsigned maxLights_;
		
		YumeVector<YumeCamera*>::type viewCameras_;
		
		YumeLight* firstLight_;
		
		YumeVector<YumeLight*>::type lights_;
		
		YumeVector<YumeLight*>::type vertexLights_;
	};

	inline bool CompareDrawables(YumeDrawable* lhs,YumeDrawable* rhs)
	{
		return lhs->GetSortValue() < rhs->GetSortValue();
	}
}


//----------------------------------------------------------------------------
#endif

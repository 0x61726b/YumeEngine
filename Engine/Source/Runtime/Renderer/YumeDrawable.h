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

	/// Geometry update type.
	enum UpdateGeometryType
	{
		UPDATE_NONE = 0,
		UPDATE_MAIN_THREAD,
		UPDATE_WORKER_THREAD
	};

	/// Rendering frame update parameters.
	struct FrameInfo
	{
		/// Frame number.
		unsigned frameNumber_;
		/// Time elapsed since last frame.
		float timeStep_;
		/// Viewport size.
		IntVector2 viewSize_;
		/// Camera being used.
		YumeCamera* camera_;
	};

	/// Source data for a 3D geometry draw call.
	struct YumeAPIExport SourceBatch
	{
		/// Construct with defaults.
		SourceBatch();
		/// Copy-construct.
		SourceBatch(const SourceBatch& batch);
		/// Destruct.
		~SourceBatch();

		/// Assignment operator.
		SourceBatch& operator =(const SourceBatch& rhs);

		/// Distance from camera.
		float distance_;
		/// Geometry.
		YumeGeometry* geometry_;
		/// Material.
		SharedPtr<YumeMaterial> material_;
		/// World transform(s). For a skinned model, these are the bone transforms.
		const Matrix3x4* worldTransform_;
		/// Number of world transforms.
		unsigned numWorldTransforms_;
		/// %Geometry type.
		GeometryType geometryType_;
	};

	/// Base class for visible components.
	class YumeAPIExport YumeDrawable : public YumeSceneComponent
	{
		friend class Octant;
		friend class Octree;
		friend void UpdateDrawablesWork(const WorkItem* item,unsigned threadIndex);

	public:
		/// Construct.
		YumeDrawable(unsigned char drawableFlags);
		/// Destruct.
		virtual ~YumeDrawable();

		/// Handle enabled/disabled state change.
		virtual void OnSetEnabled();
		/// Process octree raycast. May be called from a worker thread.
		virtual void ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results);
		/// Update before octree reinsertion. Is called from a worker thread.
		virtual void Update(const FrameInfo& frame);
		/// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
		virtual void UpdateBatches(const FrameInfo& frame);
		/// Prepare geometry for rendering.
		virtual void UpdateGeometry(const FrameInfo& frame);

		/// Return whether a geometry update is necessary, and if it can happen in a worker thread.
		virtual UpdateGeometryType GetUpdateGeometryType() { return UPDATE_NONE; }

		/// Return the geometry for a specific LOD level.
		virtual YumeGeometry* GetLodGeometry(unsigned batchIndex,unsigned level);

		/// Return number of occlusion geometry triangles.
		virtual unsigned GetNumOccluderTriangles() { return 0; }

		/// Draw to occlusion buffer. Return true if did not run out of triangles.
		virtual bool DrawOcclusion(OcclusionBuffer* buffer);
		/// Visualize the component as debug geometry.
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		/// Set draw distance.
		void SetDrawDistance(float distance);
		/// Set shadow draw distance.
		void SetShadowDistance(float distance);
		/// Set LOD bias.
		void SetLodBias(float bias);
		/// Set view mask. Is and'ed with camera's view mask to see if the object should be rendered.
		void SetViewMask(unsigned mask);
		/// Set light mask. Is and'ed with light's and zone's light mask to see if the object should be lit.
		void SetLightMask(unsigned mask);
		/// Set shadow mask. Is and'ed with light's light mask and zone's shadow mask to see if the object should be rendered to a shadow map.
		void SetShadowMask(unsigned mask);
		/// Set zone mask. Is and'ed with zone's zone mask to see if the object should belong to the zone.
		void SetZoneMask(unsigned mask);
		/// Set maximum number of per-pixel lights. Default 0 is unlimited.
		void SetMaxLights(unsigned num);
		/// Set shadowcaster flag.
		void SetCastShadows(bool enable);
		/// Set occlusion flag.
		void SetOccluder(bool enable);
		/// Set occludee flag.
		void SetOccludee(bool enable);
		/// Mark for update and octree reinsertion. Update is automatically queued when the drawable's scene node moves or changes scale.
		void MarkForUpdate();

		/// Return local space bounding box. May not be applicable or properly updated on all drawables.
		const BoundingBox& GetBoundingBox() const { return boundingBox_; }

		/// Return world-space bounding box.
		const BoundingBox& GetWorldBoundingBox();

		/// Return drawable flags.
		unsigned char GetDrawableFlags() const { return drawableFlags_; }

		/// Return draw distance.
		float GetDrawDistance() const { return drawDistance_; }

		/// Return shadow draw distance.
		float GetShadowDistance() const { return shadowDistance_; }

		/// Return LOD bias.
		float GetLodBias() const { return lodBias_; }

		/// Return view mask.
		unsigned GetViewMask() const { return viewMask_; }

		/// Return light mask.
		unsigned GetLightMask() const { return lightMask_; }

		/// Return shadow mask.
		unsigned GetShadowMask() const { return shadowMask_; }

		/// Return zone mask.
		unsigned GetZoneMask() const { return zoneMask_; }

		/// Return maximum number of per-pixel lights.
		unsigned GetMaxLights() const { return maxLights_; }

		/// Return shadowcaster flag.
		bool GetCastShadows() const { return castShadows_; }

		/// Return occluder flag.
		bool IsOccluder() const { return occluder_; }

		/// Return occludee flag.
		bool IsOccludee() const { return occludee_; }

		/// Return whether is in view this frame from any viewport camera. Excludes shadow map cameras.
		bool IsInView() const;
		/// Return whether is in view of a specific camera this frame. Pass in a null camera to allow any camera, including shadow map cameras.
		bool IsInView(YumeCamera* camera) const;

		/// Return draw call source data.
		const YumeVector<SourceBatch>::type& GetBatches() const { return batches_; }

		/// Set new zone. Zone assignment may optionally be temporary, meaning it needs to be re-evaluated on the next frame.
		void SetZone(YumeRendererEnvironment* zone,bool temporary = false);
		/// Set sorting value.
		void SetSortValue(float value);

		/// Set view-space depth bounds.
		void SetMinMaxZ(float minZ,float maxZ)
		{
			minZ_ = minZ;
			maxZ_ = maxZ;
		}

		/// Mark in view. Also clear the light list.
		void MarkInView(const FrameInfo& frame);
		/// Mark in view without specifying a camera. Used for shadow casters.
		void MarkInView(unsigned frameNumber);
		/// Sort and limit per-pixel lights to maximum allowed. Convert extra lights into vertex lights.
		void LimitLights();
		/// Sort and limit per-vertex lights to maximum allowed.
		void LimitVertexLights(bool removeConvertedLights);

		/// Set base pass flag for a batch.
		void SetBasePass(unsigned batchIndex) { basePassFlags_ |= (1 << batchIndex); }

		/// Return octree octant.
		Octant* GetOctant() const { return octant_; }

		/// Return current zone.
		YumeRendererEnvironment* GetZone() const { return zone_; }

		/// Return whether current zone is inconclusive or dirty due to the drawable moving.
		bool IsZoneDirty() const { return zoneDirty_; }

		/// Return distance from camera.
		float GetDistance() const { return distance_; }

		/// Return LOD scaled distance from camera.
		float GetLodDistance() const { return lodDistance_; }

		/// Return sorting value.
		float GetSortValue() const { return sortValue_; }

		/// Return whether is in view on the current frame. Called by View.
		bool IsInView(const FrameInfo& frame,bool anyCamera = false) const;

		/// Return whether has a base pass.
		bool HasBasePass(unsigned batchIndex) const { return (basePassFlags_ & (1 << batchIndex)) != 0; }

		/// Return per-pixel lights.
		const YumeVector<YumeLight*>::type& GetLights() const { return lights_; }

		/// Return per-vertex lights.
		const YumeVector<YumeLight*>::type& GetVertexLights() const { return vertexLights_; }

		/// Return the first added per-pixel light.
		YumeLight* GetFirstLight() const { return firstLight_; }

		/// Return the minimum view-space depth.
		float GetMinZ() const { return minZ_; }

		/// Return the maximum view-space depth.
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
		/// Handle node being assigned.
		virtual void OnNodeSet(YumeSceneNode* node);
		/// Handle scene being assigned.
		virtual void OnSceneSet(YumeScene* scene);
		/// Handle node transform being dirtied.
		virtual void OnMarkedDirty(YumeSceneNode* node);
		/// Recalculate the world-space bounding box.
		virtual void OnWorldBoundingBoxUpdate() = 0;

		/// Handle removal from octree.
		virtual void OnRemoveFromOctree() { }

		/// Add to octree.
		void AddToOctree();
		/// Remove from octree.
		void RemoveFromOctree();

		/// Move into another octree octant.
		void SetOctant(Octant* octant) { octant_ = octant; }

		/// World-space bounding box.
		BoundingBox worldBoundingBox_;
		/// Local-space bounding box.
		BoundingBox boundingBox_;
		/// Draw call source data.
		YumeVector<SourceBatch>::type batches_;
		/// Drawable flags.
		unsigned char drawableFlags_;
		/// Bounding box dirty flag.
		bool worldBoundingBoxDirty_;
		/// Shadowcaster flag.
		bool castShadows_;
		/// Occluder flag.
		bool occluder_;
		/// Occludee flag.
		bool occludee_;
		/// Octree update queued flag.
		bool updateQueued_;
		/// Zone inconclusive or dirtied flag.
		bool zoneDirty_;
		/// Octree octant.
		Octant* octant_;
		/// Current zone.
		YumeRendererEnvironment* zone_;
		/// View mask.
		unsigned viewMask_;
		/// Light mask.
		unsigned lightMask_;
		/// Shadow mask.
		unsigned shadowMask_;
		/// Zone mask.
		unsigned zoneMask_;
		/// Last visible frame number.
		unsigned viewFrameNumber_;
		/// Current distance to camera.
		float distance_;
		/// LOD scaled distance.
		float lodDistance_;
		/// Draw distance.
		float drawDistance_;
		/// Shadow distance.
		float shadowDistance_;
		/// Current sort value.
		float sortValue_;
		/// Current minimum view space depth.
		float minZ_;
		/// Current maximum view space depth.
		float maxZ_;
		/// LOD bias.
		float lodBias_;
		/// Base pass flags, bit per batch.
		unsigned basePassFlags_;
		/// Maximum per-pixel lights.
		unsigned maxLights_;
		/// List of cameras from which is seen on the current frame.
		YumeVector<YumeCamera*>::type viewCameras_;
		/// First per-pixel light added this frame.
		YumeLight* firstLight_;
		/// Per-pixel lights affecting this drawable.
		YumeVector<YumeLight*>::type lights_;
		/// Per-vertex lights affecting this drawable.
		YumeVector<YumeLight*>::type vertexLights_;
	};

	inline bool CompareDrawables(YumeDrawable* lhs,YumeDrawable* rhs)
	{
		return lhs->GetSortValue() < rhs->GetSortValue();
	}
}


//----------------------------------------------------------------------------
#endif

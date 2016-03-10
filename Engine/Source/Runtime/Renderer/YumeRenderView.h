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
#ifndef __YumeRenderView_h__
#define __YumeRenderView_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeBatch.h"
#include "YumeLight.h"
#include "YumeRendererEnv.h"
#include "Math/YumePolyhedron.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeCamera;
	class YumeDebugRenderer;
	class YumeLight;
	class YumeDrawable;
	class YumeRHI;
	class OcclusionBuffer;
	class Octree;
	class YumeRenderer;
	class YumeRenderPipeline;
	class YumeRenderable;
	class YumeRenderTechnique;
	class YumeTexture;
	class YumeTexture2D;
	class YumeViewport;
	class YumeRendererEnvironment;
	struct RenderCommand;
	struct WorkItem;


	
	struct LightQueryResult
	{
		
		YumeLight* light_;
		
		YumeVector<YumeDrawable*> litGeometries_;
		
		YumeVector<YumeDrawable*> shadowCasters_;
		
		YumeCamera* shadowCameras_[MAX_LIGHT_SPLITS];
		
		unsigned shadowCasterBegin_[MAX_LIGHT_SPLITS];
		
		unsigned shadowCasterEnd_[MAX_LIGHT_SPLITS];
		
		BoundingBox shadowCasterBox_[MAX_LIGHT_SPLITS];
		
		float shadowNearSplits_[MAX_LIGHT_SPLITS];
		
		float shadowFarSplits_[MAX_LIGHT_SPLITS];
		
		unsigned numSplits_;
	};

	
	struct ScenePassInfo
	{
		
		unsigned passIndex_;
		
		bool allowInstancing_;
		
		bool markToStencil_;
		
		bool vertexLights_;
		
		BatchQueue* batchQueue_;
	};

	
	struct PerThreadSceneResult
	{
		
		YumeVector<YumeDrawable*> geometries_;
		
		YumeVector<YumeLight*> lights_;
		
		float minZ_;
		
		float maxZ_;
	};

	static const unsigned MAX_VIEWPORT_TEXTURES = 2;

	
	class YumeAPIExport YumeRenderView
	{
		friend void CheckVisibilityWork(const WorkItem* item,unsigned threadIndex);
		friend void ProcessLightWork(const WorkItem* item,unsigned threadIndex);
	public:
		
		YumeRenderView();
		
		virtual ~YumeRenderView();

		
		bool Define(YumeRenderable* renderTarget,YumeViewport* viewport);
		
		void Update(const FrameInfo& frame);
		
		void Render();

		
		YumeRHI* GetRHI() const;
		
		YumeRenderer* GetRenderer() const;

		
		YumeScene* GetScene() const { return scene_; }

		
		Octree* GetOctree() const { return octree_; }

		
		YumeCamera* GetCamera() const { return camera_; }

		
		YumeCamera* GetCullCamera() const { return cullCamera_; }

		
		const FrameInfo& GetFrameInfo() const { return frame_; }

		
		YumeRenderable* GetRenderTarget() const { return renderTarget_; }

		
		bool GetDrawDebug() const { return drawDebug_; }

		
		const YumeVector<YumeDrawable*>& GetGeometries() const { return geometries_; }

		
		const YumeVector<YumeDrawable*>& GetOccluders() const { return occluders_; }

		
		const YumeVector<YumeLight*>& GetLights() const { return lights_; }

		
		const YumeVector<LightBatchQueue>::type& GetLightQueues() const { return lightQueues_; }

		
		OcclusionBuffer* GetOcclusionBuffer() const { return occlusionBuffer_; }

		
		unsigned GetNumActiveOccluders() const { return activeOccluders_; }

		
		YumeRenderView* GetSourceView() const;

		
		void SetGlobalShaderParameters();
		
		void SetCameraShaderParameters(YumeCamera* camera,bool setProjectionMatrix);
		
		void SetGBufferShaderParameters(const IntVector2& texSize,const IntRect& viewRect);

		
		void DrawFullscreenQuad(bool nearQuad);

	private:
		
		void GetDrawables();
		
		void GetBatches();
		
		void ProcessLights();
		
		void GetLightBatches();
		
		void GetBaseBatches();
		
		void UpdateGeometries();
		
		void GetLitBatches(YumeDrawable* drawable,LightBatchQueue& lightQueue,BatchQueue* alphaQueue);
		
		void ExecuteRenderPathCommands();
		
		void SetRenderTargets(RenderCommand& command);
		
		bool SetTextures(RenderCommand& command);
		
		void RenderQuad(RenderCommand& command);
		
		bool IsNecessary(const RenderCommand& command);
		
		bool CheckViewportRead(const RenderCommand& command);
		
		bool CheckViewportWrite(const RenderCommand& command);
		
		bool CheckPingpong(unsigned index);
		
		void AllocateScreenBuffers();
		
		void BlitFramebuffer(YumeTexture* source,YumeRenderable* destination,bool depthWrite);
		
		void UpdateOccluders(YumeVector<YumeDrawable*>& occluders,YumeCamera* camera);
		
		void DrawOccluders(OcclusionBuffer* buffer,const YumeVector<YumeDrawable*>& occluders);
		
		void ProcessLight(LightQueryResult& query,unsigned threadIndex);
		
		void ProcessShadowCasters(LightQueryResult& query,const YumeVector<YumeDrawable*>& drawables,unsigned splitIndex);
		
		void SetupShadowCameras(LightQueryResult& query);
		
		void SetupDirLightShadowCamera(YumeCamera* shadowCamera,YumeLight* light,float nearSplit,float farSplit);
		
		void
			FinalizeShadowCamera(YumeCamera* shadowCamera,YumeLight* light,const IntRect& shadowViewport,const BoundingBox& shadowCasterBox);
		
		void
			QuantizeDirLightShadowCamera(YumeCamera* shadowCamera,YumeLight* light,const IntRect& shadowViewport,const BoundingBox& viewBox);
		
		bool IsShadowCasterVisible(YumeDrawable* drawable,BoundingBox lightViewBox,YumeCamera* shadowCamera,const Matrix3x4& lightView,
			const Frustum& lightViewFrustum,const BoundingBox& lightViewFrustumBox);
		
		IntRect GetShadowMapViewport(YumeLight* light,unsigned splitIndex,YumeTexture2D* shadowMap);
		
		void FindZone(YumeDrawable* drawable);
		
		YumeRenderTechnique* GetTechnique(YumeDrawable* drawable,YumeMaterial* material);
		
		void CheckMaterialForAuxView(YumeMaterial* material);
		
		void AddBatchToQueue(BatchQueue& queue,Batch& batch,YumeRenderTechnique* tech,bool allowInstancing = true,bool allowShadows = true);
		
		void PrepareInstancingBuffer();
		
		void SetupLightVolumeBatch(Batch& batch);
		
		void RenderShadowMap(const LightBatchQueue& queue);
		
		YumeRenderable* GetDepthStencil(YumeRenderable* renderTarget);
		
		YumeRenderable* GetRenderSurfaceFromTexture(YumeTexture* texture,CubeMapFace face = FACE_POSITIVE_X);
		
		YumeTexture* FindNamedTexture(const YumeString& name,bool isRenderTarget,bool isVolumeMap = false);

		
		YumeRendererEnvironment* GetZone(YumeDrawable* drawable)
		{
			if(cameraZoneOverride_)
				return cameraZone_;
			YumeRendererEnvironment* drawableZone = drawable->GetZone();
			return drawableZone ? drawableZone : cameraZone_;
		}

		
		unsigned GetLightMask(YumeDrawable* drawable)
		{
			return drawable->GetLightMask() & GetZone(drawable)->GetLightMask();
		}

		
		unsigned GetShadowMask(YumeDrawable* drawable)
		{
			return drawable->GetShadowMask() & GetZone(drawable)->GetShadowMask();
		}

		
		unsigned long long GetVertexLightQueueHash(const YumeVector<YumeLight*>::type& vertexLights)
		{
			unsigned long long hash = 0;
			for(YumeVector<YumeLight*>::const_iterator i = vertexLights.begin(); i != vertexLights.end(); ++i)
				hash += (unsigned long long)(*i);
			return hash;
		}

		
		SharedPtr<YumeRHI> graphics_;
		
		SharedPtr<YumeRenderer> renderer_;
		
		YumeScene* scene_;
		
		Octree* octree_;
		
		YumeCamera* camera_;
		
		YumeCamera* cullCamera_;
		
		YumeRenderView* sourceView_;
		
		YumeRendererEnvironment* cameraZone_;
		
		YumeRendererEnvironment* farClipZone_;
		
		OcclusionBuffer* occlusionBuffer_;
		
		YumeRenderable* renderTarget_;
		
		YumeRenderable* substituteRenderTarget_;
		
		YumeTexture* viewportTextures_[MAX_VIEWPORT_TEXTURES];
		
		YumeRenderable* currentRenderTarget_;
		
		YumeTexture* currentViewportTexture_;
		
		YumeTexture* depthOnlyDummyTexture_;
		
		IntRect viewRect_;
		
		IntVector2 viewSize_;
		
		IntVector2 rtSize_;
		
		FrameInfo frame_;
		
		float aspectRatio_;
		
		float minZ_;
		
		float maxZ_;
		
		int materialQuality_;
		
		int maxOccluderTriangles_;
		
		int minInstances_;
		
		int highestZonePriority_;
		
		bool geometriesUpdated_;
		
		bool cameraZoneOverride_;
		
		bool drawShadows_;
		
		bool deferred_;
		
		bool deferredAmbient_;
		
		bool useLitBase_;
		
		bool hasScenePasses_;
		
		bool noStencil_;
		
		bool drawDebug_;
		
		YumeRenderPipeline* renderPath_;
		
		YumeVector<YumeVector<YumeDrawable*> >::type tempDrawables_;
		
		YumeVector<PerThreadSceneResult>::type sceneResults_;
		
		YumeVector<YumeRendererEnvironment*> zones_;
		
		YumeVector<YumeDrawable*> geometries_;
		
		YumeVector<YumeDrawable*> nonThreadedGeometries_;
		
		YumeVector<YumeDrawable*> threadedGeometries_;
		
		YumeVector<YumeDrawable*> occluders_;
		
		YumeVector<YumeLight*> lights_;
		
		unsigned activeOccluders_;

		
		YumeVector<YumeDrawable*>::type maxLightsDrawables_;
		
		YumeMap<YumeHash,YumeTexture*>::type renderTargets_;
		
		YumeVector<LightQueryResult>::type lightQueryResults_;
		
		YumeVector<ScenePassInfo>::type scenePasses_;
		
		YumeVector<LightBatchQueue>::type lightQueues_;
		
		YumeMap<unsigned long long,LightBatchQueue>::type vertexLightQueues_;
		
		YumeMap<unsigned,BatchQueue>::type batchQueues_;
		
		unsigned gBufferPassIndex_;
		
		unsigned basePassIndex_;
		
		unsigned alphaPassIndex_;
		
		unsigned lightPassIndex_;
		
		unsigned litBasePassIndex_;
		
		unsigned litAlphaPassIndex_;
		
		const RenderCommand* lightVolumeCommand_;
		
		bool usedResolve_;
	};
}


//----------------------------------------------------------------------------
#endif

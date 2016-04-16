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
#ifndef __YumeRenderer_h__
#define __YumeRenderer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeDrawable.h"
#include "YumeViewport.h"
#include "YumeRendererDefs.h"
#include "YumeBatch.h"
#include "Core/YumeMutex.h"

#include "Core/YumeEventHub.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGeometry;
	class YumeDrawable;
	class YumeLight;
	class YumeMaterial;
	class YumeRenderPass;
	class YumeRenderTechnique;
	class Octree;
	class YumeRHI;
	class YumeRenderPipeline;
	class YumeRenderable;
	class YumeResourceManager;
	class OcclusionBuffer;
	class YumeTexture;
	class YumeTexture2D;
	class YumeTextureCube;
	class YumeRenderView;
	class YumeRendererEnvironment;
	class LPVRenderer;


	static const int SHADOW_MIN_PIXELS = 64;
	static const int INSTANCING_BUFFER_DEFAULT_SIZE = 1024;


	enum LightVSVariation
	{
		LVS_DIR = 0,
		LVS_SPOT,
		LVS_POINT,
		LVS_SHADOW,
		LVS_SPOTSHADOW,
		LVS_POINTSHADOW,
		MAX_LIGHT_VS_VARIATIONS
	};


	enum VertexLightVSVariation
	{
		VLVS_NOLIGHTS = 0,
		VLVS_1LIGHT,
		VLVS_2LIGHTS,
		VLVS_3LIGHTS,
		VLVS_4LIGHTS,
		MAX_VERTEXLIGHT_VS_VARIATIONS
	};


	enum LightPSVariation
	{
		LPS_NONE = 0,
		LPS_SPOT,
		LPS_POINT,
		LPS_POINTMASK,
		LPS_SPEC,
		LPS_SPOTSPEC,
		LPS_POINTSPEC,
		LPS_POINTMASKSPEC,
		LPS_SHADOW,
		LPS_SPOTSHADOW,
		LPS_POINTSHADOW,
		LPS_POINTMASKSHADOW,
		LPS_SHADOWSPEC,
		LPS_SPOTSHADOWSPEC,
		LPS_POINTSHADOWSPEC,
		LPS_POINTMASKSHADOWSPEC,
		MAX_LIGHT_PS_VARIATIONS
	};


	enum DeferredLightVSVariation
	{
		DLVS_NONE = 0,
		DLVS_DIR,
		DLVS_ORTHO,
		DLVS_ORTHODIR,
		MAX_DEFERRED_LIGHT_VS_VARIATIONS
	};


	enum DeferredLightPSVariation
	{
		DLPS_NONE = 0,
		DLPS_SPOT,
		DLPS_POINT,
		DLPS_POINTMASK,
		DLPS_SPEC,
		DLPS_SPOTSPEC,
		DLPS_POINTSPEC,
		DLPS_POINTMASKSPEC,
		DLPS_SHADOW,
		DLPS_SPOTSHADOW,
		DLPS_POINTSHADOW,
		DLPS_POINTMASKSHADOW,
		DLPS_SHADOWSPEC,
		DLPS_SPOTSHADOWSPEC,
		DLPS_POINTSHADOWSPEC,
		DLPS_POINTMASKSHADOWSPEC,
		DLPS_ORTHO,
		DLPS_ORTHOSPOT,
		DLPS_ORTHOPOINT,
		DLPS_ORTHOPOINTMASK,
		DLPS_ORTHOSPEC,
		DLPS_ORTHOSPOTSPEC,
		DLPS_ORTHOPOINTSPEC,
		DLPS_ORTHOPOINTMASKSPEC,
		DLPS_ORTHOSHADOW,
		DLPS_ORTHOSPOTSHADOW,
		DLPS_ORTHOPOINTSHADOW,
		DLPS_ORTHOPOINTMASKSHADOW,
		DLPS_ORTHOSHADOWSPEC,
		DLPS_ORTHOSPOTSHADOWSPEC,
		DLPS_ORTHOPOINTSHADOWSPEC,
		DLPS_ORTHOPOINTMASKSHADOWSPEC,
		MAX_DEFERRED_LIGHT_PS_VARIATIONS
	};

	class YumeAPIExport YumeRenderer : public YumeBase,public EngineEventListener,public RHIEventListener
	{
	public:
		typedef void(YumeBase::*ShadowMapFilter)(YumeRenderView* view,YumeTexture2D* shadowMap);

		YumeRenderer(const YumeString& name);
		virtual ~YumeRenderer();

		void HandleRenderUpdate(float timeStep);
		void HandleGraphicsMode();


		void SetNumViewports(unsigned num);
		void SetViewport(unsigned index,YumeViewport* viewport);
		void SetDefaultRenderPath(YumeRenderPipeline* renderPath);
		void SetDefaultRenderPath(YumeXmlFile* file);
		void SetHDRRendering(bool enable);
		void SetGBufferDebugRendering(bool enable);
		void SetSpecularLighting(bool enable);
		void SetTextureAnisotropy(int level);
		void SetTextureFilterMode(TextureFilterMode mode);
		void SetTextureQuality(int quality);
		void SetMaterialQuality(int quality);
		void SetDrawShadows(bool enable);
		void SetShadowMapSize(int size);
		void SetShadowQuality(ShadowQuality quality);
		void SetShadowSoftness(float shadowSoftness);
		void SetVSMShadowParameters(float minVariance,float lightBleedingReduction);
		void SetShadowMapFilter(YumeBase* instance,ShadowMapFilter functionPtr);
		void SetReuseShadowMaps(bool enable);
		void SetMaxShadowMaps(int shadowMaps);
		void SetDynamicInstancing(bool enable);
		void SetMinInstances(int instances);
		void SetMaxSortedInstances(int instances);
		void SetMaxOccluderTriangles(int triangles);
		void SetOcclusionBufferSize(int size);
		void SetOccluderSizeThreshold(float screenSize);
		void SetThreadedOcclusion(bool enable);
		void SetMobileShadowBiasMul(float mul);
		void SetMobileShadowBiasAdd(float add);
		void ReloadShaders();


		void ApplyShadowMapFilter(YumeRenderView* view,YumeTexture2D* shadowMap);


		unsigned GetNumViewports() const { return viewports_.size(); }
		YumeViewport* GetViewport(unsigned index) const;
		SharedPtr<YumeRenderPipeline> GetDefaultPipeline() const;
		bool GetHDRRendering() const { return hdrRendering_; }
		bool GetGBufferDebugRendering() const { return debugGBufferRendering_; }
		bool GetSpecularLighting() const { return specularLighting_; }
		bool GetDrawShadows() const { return drawShadows_; }
		int GetTextureAnisotropy() const { return textureAnisotropy_; }


		TextureFilterMode GetTextureFilterMode() const { return textureFilterMode_; }
		int GetTextureQuality() const { return textureQuality_; }
		int GetMaterialQuality() const { return materialQuality_; }
		int GetShadowMapSize() const { return shadowMapSize_; }
		ShadowQuality GetShadowQuality() const { return shadowQuality_; }
		float GetShadowSoftness() const { return shadowSoftness_; }
		Vector2 GetVSMShadowParameters() const { return vsmShadowParams_; };
		bool GetReuseShadowMaps() const { return reuseShadowMaps_; }
		int GetMaxShadowMaps() const { return maxShadowMaps_; }
		bool GetDynamicInstancing() const { return dynamicInstancing_; }


		int GetMinInstances() const { return minInstances_; }
		int GetMaxSortedInstances() const { return maxSortedInstances_; }
		int GetMaxOccluderTriangles() const { return maxOccluderTriangles_; }
		int GetOcclusionBufferSize() const { return occlusionBufferSize_; }
		float GetOccluderSizeThreshold() const { return occluderSizeThreshold_; }
		bool GetThreadedOcclusion() const { return threadedOcclusion_; }
		float GetMobileShadowBiasMul() const { return mobileShadowBiasMul_; }
		float GetMobileShadowBiasAdd() const { return mobileShadowBiasAdd_; }
		unsigned GetNumViews() const { return views_.size(); }
		unsigned GetNumPrimitives() const { return numPrimitives_; }
		unsigned GetNumBatches() const { return numBatches_; }
		unsigned GetNumGeometries(bool allViews = false) const;
		unsigned GetNumLights(bool allViews = false) const;
		unsigned GetNumShadowMaps(bool allViews = false) const;
		unsigned GetNumOccluders(bool allViews = false) const;

		Vector4* GetFrustumCorners() { return frustumCorners_; };
		Vector4* GetSSAOVectorOffsets() { return vectorOffsets_; }


		YumeRendererEnvironment* GetDefaultZone() const { return defaultZone_; }
		YumeMaterial* GetDefaultMaterial() const { return defaultMaterial_; }
		YumeTexture2D* GetDefaultLightRamp() const { return defaultLightRamp_; }
		YumeTexture2D* GetRandomVectorMap() const { return randomVectorMap_; }
		YumeTexture2D* GetDefaultLightSpot() const { return defaultLightSpot_; }
		YumeTextureCube* GetFaceSelectCubeMap() const { return faceSelectCubeMap_; }
		YumeTextureCube* GetIndirectionCubeMap() const { return indirectionCubeMap_; }
		YumeVertexBuffer* GetInstancingBuffer() const { return dynamicInstancing_ ? instancingBuffer_.Get() : 0; }


		const FrameInfo& GetFrameInfo() const { return frame_; }
		void Update(float timeStep);
		void Render();
		void DrawDebugGeometry(bool depthTest);
		void QueueRenderable(YumeRenderable* renderTarget);
		void QueueViewport(YumeRenderable* renderTarget,YumeViewport* viewport);
		YumeGeometry* GetLightGeometry(YumeLight* light);
		YumeGeometry* GetQuadGeometry();
		YumeGeometry* GetSSAOQuadGeometry();
		YumeGeometry* GetTexturedQuadGeometry();
		YumeGeometry* GetFsTriangle();
		YumeTexture2D* GetShadowMap(YumeLight* light,YumeCamera* camera,unsigned viewWidth,unsigned viewHeight);
		YumeTexture* GetScreenBuffer(int width,int height,unsigned format,bool cubemap,bool filtered,bool srgb,unsigned persistentKey = 0);
		YumeRenderable* GetDepthStencil(int width,int height);
		OcclusionBuffer* GetOcclusionBuffer(YumeCamera* camera);


		YumeCamera* GetShadowCamera();
		void StorePreparedView(YumeRenderView* view,YumeCamera* cullCamera);
		YumeRenderView* GetPreparedView(YumeCamera* cullCamera);


		void SetBatchShaders(Batch& batch,YumeRenderTechnique* tech,bool allowShadows = true);
		void SetLightVolumeBatchShaders(Batch& batch,YumeCamera* camera,const YumeString& vsName,const YumeString& psName,const YumeString& vsDefines,const YumeString& psDefines);
		void SetCullMode(CullMode mode,YumeCamera* camera);
		bool ResizeInstancingBuffer(unsigned numInstances);
		void SaveScreenBufferAllocations();
		void RestoreScreenBufferAllocations();
		void OptimizeLightByScissor(YumeLight* light,YumeCamera* camera);
		void OptimizeLightByStencil(YumeLight* light,YumeCamera* camera);
		const Rect& GetLightScissor(YumeLight* light,YumeCamera* camera);
		static YumeRenderView* GetActualView(YumeRenderView* view);

		typedef YumeVector<RendererEventListener*> RendererListeners;
		RendererListeners::type listeners_;

		void AddListener(RendererEventListener* listener);
		void RemoveListener(RendererEventListener* listener);
		void FireEvent(YumeEngineEvents e);
		void FireEvent(YumeEngineEvents e,YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera);

		const YumeString& GetName() { return name_; }
		void Initialize();

		static YumeHash type_;
		static YumeHash GetTypeStatic() { return type_; }
		virtual YumeHash GetType() { return type_; }

	private:
		void LoadShaders();
		void LoadPassShaders(YumeRenderPass* pass);
		void ReleaseMaterialShaders();
		void ReloadTextures();
		void CreateGeometries();
		void CreateInstancingBuffer();
		void SetIndirectionTextureData();
		void PrepareViewRender();
		void RemoveUnusedBuffers();
		void ResetShadowMapAllocations();
		void ResetScreenBufferAllocations();
		void ResetShadowMaps();
		void ResetBuffers();
		void CreateRandomColors();
		void CreateSSAOResources();

		YumeString GetShadowVariations() const;
		void HandleScreenMode(YumeHash eventType,VariantMap& eventData);
		void BlurShadowMap(YumeRenderView* view,YumeTexture2D* shadowMap);

		YumeString name_;
		SharedPtr<YumeRenderPipeline> defaultPipeline_;
		SharedPtr<YumeRendererEnvironment> defaultZone_;
		SharedPtr<YumeGeometry> dirLightGeometry_;
		SharedPtr<YumeGeometry> fullScreenTriangleGeometry_;
		SharedPtr<YumeGeometry> texturedQuadGeometry;
		SharedPtr<YumeGeometry> ssaoQuad_;
		SharedPtr<YumeGeometry> spotLightGeometry_;
		SharedPtr<YumeGeometry> pointLightGeometry_;
		SharedPtr<YumeVertexBuffer> instancingBuffer_;
		SharedPtr<YumeMaterial> defaultMaterial_;
		SharedPtr<YumeTexture2D> defaultLightRamp_;
		SharedPtr<YumeTexture2D> defaultLightSpot_;
		SharedPtr<YumeTexture2D> randomVectorMap_;
		SharedPtr<YumeTextureCube> faceSelectCubeMap_;
		SharedPtr<YumeTextureCube> indirectionCubeMap_;

	public:
		LPVRenderer* lpvRenderer_;
	private:

		YumeVector<SharedPtr<YumeSceneNode> >::type shadowCameraNodes_;
		YumeVector<SharedPtr<OcclusionBuffer> >::type occlusionBuffers_;
		YumeMap<int,YumeVector<SharedPtr<YumeTexture2D> >::type>::type shadowMaps_;
		YumeMap<int,SharedPtr<YumeTexture2D> >::type colorShadowMaps_;
		YumeMap<int,YumeVector<YumeLight*>::type >::type shadowMapAllocations_;
		YumeBase* shadowMapFilterInstance_;
		ShadowMapFilter shadowMapFilter_;
		YumeMap<long long,YumeVector<SharedPtr<YumeTexture> >::type >::type screenBuffers_;
		YumeMap<long long,unsigned>::type screenBufferAllocations_;
		YumeMap<long long,unsigned>::type savedScreenBufferAllocations_;
		YumeMap<Pair<YumeLight*,YumeCamera*>,Rect>::type lightScissorCache_;
		YumeVector<SharedPtr<YumeViewport> >::type viewports_;
		YumeVector<Pair<WeakPtr<YumeRenderable>,WeakPtr<YumeViewport> > >::type queuedViewports_;
		YumeVector<WeakPtr<YumeRenderView> >::type views_;


		YumeMap<YumeCamera*,YumeRenderView*>::type preparedViews_;
		YumeHashSet<Octree*>::type updatedOctrees_;
		YumeHashSet<YumeRenderTechnique*>::type shaderErrorDisplayed_;
		Mutex rendererMutex_;
		YumeVector<YumeString>::type deferredLightPSVariations_;

		//SSAO
		Vector4 frustumCorners_[4];
		Vector4 vectorOffsets_[14];


		FrameInfo frame_;
		int textureAnisotropy_;
		TextureFilterMode textureFilterMode_;
		int textureQuality_;
		int materialQuality_;
		int shadowMapSize_;
		ShadowQuality shadowQuality_;
		float shadowSoftness_;
		Vector2 vsmShadowParams_;
		int maxShadowMaps_;
		int minInstances_;
		int maxSortedInstances_;
		int maxOccluderTriangles_;
		int occlusionBufferSize_;
		float occluderSizeThreshold_;
		float mobileShadowBiasMul_;
		float mobileShadowBiasAdd_;
		unsigned numOcclusionBuffers_;
		unsigned numShadowCameras_;
		unsigned numPrimitives_;
		unsigned numBatches_;
		unsigned shadersChangedFrameNumber_;
		unsigned char lightStencilValue_;
		bool debugGBufferRendering_;
		bool hdrRendering_;
		bool specularLighting_;
		bool drawShadows_;
		bool reuseShadowMaps_;
		bool dynamicInstancing_;
		bool threadedOcclusion_;
		bool shadersDirty_;
		bool initialized_;
		bool resetViews_;
	};
}


//----------------------------------------------------------------------------
#endif

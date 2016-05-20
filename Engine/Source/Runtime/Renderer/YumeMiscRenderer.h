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
#ifndef __YumeMiscRenderer_h__
#define __YumeMiscRenderer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "YumeLPVCamera.h"
#include "YumeSkydome.h"

#include "RenderCall.h"
#include "LightPropagationVolume.h"
#include "SparseVoxelOctree.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class LPVRenderer;
	class YumePostProcess;
	class RenderPass;
	class Scene;
	class SceneNode;
	class GIVolume;

	enum GISolution
	{
		NoGI = 0,
		SVO = 1,
		LPV = 2
	};

	class YumeAPIExport YumeMiscRenderer : 
		public YumeBase,
		public EngineEventListener
	{
	public:
		YumeMiscRenderer();
		virtual ~YumeMiscRenderer();

		void Setup();
		void Initialize(GISolution gi = GISolution::NoGI);


		//New era starts here
		void ApplyRendererFlags(RenderCallPtr call);
		void RenderReflectiveShadowMap(RenderCall* call);

		bool GetGIEnabled() { return giEnabled_; }

		void ApplyShaderParameters(RenderCall* call);
		void SetSamplers(RenderCall* call);

		void SetGIParameters();
		void SetMainLight();

		bool giEnabled_;
		bool updateRsm_;

		Scene* GetScene() const {return scene_; };
		Scene* scene_;

		GIVolume* giVolume_;
		/*SparseVoxelOctree giVolume_;*/

		unsigned curr_;
		unsigned next_;
		unsigned num_propagations_;

		RenderPass* GetDefaultPass() const { return defaultPass_; }

		GIParameters giParams_;

		void LPVPropagate(RenderCall* call,float iteration);


		void SetRSMSize(unsigned newSize) { rsmSize = newSize;}

		YumeString GetTextureName(const YumeString&,int num);

		void RenderLights(RenderCall* call,TexturePtr target);

		void SetGBufferShaderParameters(const IntVector2& texSize,const IntRect& viewRect);

		void ConstructFrustum(float depth);
		bool CheckPointAgainstFrustum(float x,float y,float z);
		bool CheckBB(float xCenter, float yCenter, float zCenter, float size);
		bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
		bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

		void PrepareRendering();
		TexturePtr AllocateAdditionalBuffers(int width,int height,unsigned format,bool cubemap,bool filtered,bool srgb,bool mips,
		unsigned persistentKey = 0);

		void BlitRenderTarget(TexturePtr source,TexturePtr dest,bool depthWrite);

		DirectX::XMVECTOR frustumPlanes_[6];

		YumeVector<TexturePtr>::type GetFreeTextures();

		bool disableFrustumCull_;

		void UpdateGI();

		Texture2DPtr backBufferSub_;

		TexturePtr adaptLuminance[2];
		bool currentAdaptedLuminance_;

		bool backbufferModified_;
		bool usedResolve_;

		bool CheckPingPong(RenderCall* call,unsigned index);

		TexturePtr viewportTextures_[3];
		TexturePtr currentViewportTexture_;
		YumeMap<long long, YumeVector<SharedPtr<YumeTexture> >::type >::type screenBuffers_;
		YumeMap<long long, unsigned>::type screenBufferAllocations_;

		void RemoveUnusedBuffers();

		YumeRenderable* currentRenderTarget_;

		SharedPtr<YumeGeometry> skyGeometry_;

		void SetFloorRoughness(float f);

		void RenderIntoCubemap();
		void CreateCubemapCameras(float x,float y,float z);
		bool updateCubemap_;

		YumeVector<YumeCamera*>::type cubemapCams_;

		SharedPtr<YumeTextureCube> cubemapRt_;
		SharedPtr<YumeTexture2D> cubemapDsv_;

		void UpdateCamera(float dt);
		void UpdateLights();

		float cameraMoveSpeed_;

		GISolution gi_;
 		//~

		void Render();
		void RenderSky(YumeRenderable* target,YumeCamera* cam);
		void RenderScene();

		void RenderFullScreenTexture(const IntRect& rect,YumeTexture2D*);

		virtual void HandlePostRenderUpdate(float timeStep);

		void SetCameraParameters(bool shadowPass,YumeCamera* camera);
		void SetPerFrameConstants();

		void Update(float timeStep);

		YumeCamera* GetCamera() const { return camera_; }

		YumePostProcess* GetPP() const {return pp_; }

		const DirectX::XMFLOAT3& GetMinBb() const { return bbMin; }
		const DirectX::XMFLOAT3& GetMaxBb() const { return bbMax; }

		YumeGeometry* GetFsTriangle() const { return fullscreenTriangle_; };

	public:
		void SetGIDebug(bool enabled);

		void SetGIScale(float f);
		void SetLPVFluxAmp(float f);

		void SetLPVPos(float x,float y,float z);
		void SetLightFlux(float f);
		void SetLPVNumberIterations(int num);
		
		void UpdateMeshBb(SceneNode* node,const DirectX::XMMATRIX& world);
	private: //Geometry stuff
		SharedPtr<YumeGeometry> fullscreenTriangle_;
		SharedPtr<YumeGeometry> pointLightGeometry_;

		SharedPtr<YumeTexture2D> pointLightAttTexture_;
		DirectX::XMFLOAT3 bbMin;
		DirectX::XMFLOAT3 bbMax;

	private:
		SharedPtr<FirstPersonCamera> camera_;

	private:
		unsigned rsmSize;

	private: //Renderer stuff
		YumeRHI* rhi_;
		SharedPtr<YumePostProcess> pp_;

		SharedPtr<RenderPass> defaultPass_;

		SharedPtr<YumeTexture2D> lightMap;

	public:
		float zNear;
		float zFar;
		DirectX::XMMATRIX MakeProjection();
	};
}


//----------------------------------------------------------------------------
#endif

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

	class YumeAPIExport YumeMiscRenderer : 
		public YumeBase,
		public EngineEventListener
	{
	public:
		YumeMiscRenderer();
		virtual ~YumeMiscRenderer();

		void Setup();
		void Initialize();


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

		LightPropagationVolume giLpvVolume_;
		SparseVoxelOctree giSvoVolume_;

		unsigned curr_;
		unsigned next_;
		unsigned num_propagations_;

		RenderPass* GetDefaultPass() const { return defaultPass_; }

		GIParameters giParams_;

		void LPVPropagate(RenderCall* call,float iteration);


		void SetRSMSize(unsigned newSize) { rsmSize = newSize;}

		YumeString GetTextureName(const YumeString&,int num);

		void RenderLights();

		void SetGBufferShaderParameters(const IntVector2& texSize,const IntRect& viewRect);
 		//~

		void Render();
		void RenderSky();
		void RenderScene();

		void RenderFullScreenTexture(const IntRect& rect,YumeTexture2D*);

		virtual void HandlePostRenderUpdate(float timeStep);

		void SetCameraParameters(bool shadowPass);
		void SetPerFrameConstants();

		void Update(float timeStep);

		YumeLPVCamera* GetCamera() const { return camera_; }

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
		
		void UpdateMeshBb(YumeMesh& mesh);
	private: //Geometry stuff
		SharedPtr<YumeGeometry> fullscreenTriangle_;
		SharedPtr<YumeGeometry> pointLightGeometry_;

		DirectX::XMFLOAT3 bbMin;
		DirectX::XMFLOAT3 bbMax;

	private:
		SharedPtr<YumeLPVCamera> camera_;

	private:
		unsigned rsmSize;

	private: //Renderer stuff
		YumeRHI* rhi_;
		SharedPtr<YumePostProcess> pp_;

		SharedPtr<RenderPass> defaultPass_;

	public:
		float zNear;
		float zFar;
		DirectX::XMMATRIX MakeProjection();
	};
}


//----------------------------------------------------------------------------
#endif

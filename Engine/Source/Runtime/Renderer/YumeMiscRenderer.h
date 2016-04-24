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

		void SetModelMatrix(const DirectX::XMFLOAT4X4& model,const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax);
		bool GetGIEnabled() { return giEnabled_; }

		void SetGIParameters();

		bool giEnabled_;
		bool updateRsm_;

		Scene* GetScene() const {return scene_; };
		Scene* scene_;

		LightPropagationVolume lightPropagator_;

		unsigned curr_;
		unsigned next_;
		unsigned num_iterations_rendered;

		RenderPass* GetDefaultPass() const { return defaultPass_; }

		GIParameters giParams_;

		void LPVPropagate(RenderCall* call,float iteration);
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


		LPVRenderer* GetLPVRenderer() const {return lpv_;}
		YumePostProcess* GetPP() const {return pp_; }

		const DirectX::XMFLOAT3& GetMinBb() const { return bbMin; }
		const DirectX::XMFLOAT3& GetMaxBb() const { return bbMax; }

		YumeTexture2D* GetRenderTarget() const { return renderTarget_; }
		YumeGeometry* GetFsTriangle() const { return fullscreenTriangle_; };

	public:
		void SetGIDebug(bool enabled);

		void SetGIScale(float f);
		void SetLPVFluxAmp(float f);

		void SetLPVPos(float x,float y,float z);
		void SetLightFlux(float f);
		void SetLPVNumberIterations(int num);
		

	private: //Geometry stuff
		void UpdateMeshBb(YumeMesh& mesh);

		SharedPtr<YumeGeometry> fullscreenTriangle_;
	private:
		YumeMesh* mesh_; //Whole scene
		YumeSkydome sky_;

	private: //Scene stuff
		DirectX::XMFLOAT3 bbMin;
		DirectX::XMFLOAT3 bbMax;

		SharedPtr<YumeLPVCamera> camera_;

	private: //Renderer stuff
		YumeRHI* rhi_;
		SharedPtr<YumePostProcess> pp_;
		SharedPtr<LPVRenderer> lpv_;
		SharedPtr<YumeTexture2D> renderTarget_;

		SharedPtr<RenderPass> defaultPass_;

	public:
		float zNear;
		float zFar;
		DirectX::XMMATRIX MakeProjection();
	private:
		YumeShaderVariation* overlayPs_;

	private: //Textures
		SharedPtr<YumeTexture2D> dummyTexture_;
	};
}


//----------------------------------------------------------------------------
#endif

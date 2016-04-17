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
#ifndef __LPVRendererTest_h__
#define __LPVRendererTest_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "LightPropagationVolume.h"
#include "YumeSkydome.h"

#include <DirectXMath.h>

#include "YumeLPVCamera.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	struct directional_light
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT4 color;
	};

	class YumeModel;
	class YumeStaticModel;
	class YumeMesh;
	class YumeSkydome;
	class YumeMiscRenderer;

	class YumeAPIExport LPVRenderer : public YumeBase
	{
	public:
		LPVRenderer(YumeMiscRenderer* m);

		virtual ~LPVRenderer();

		void Setup();
		void Update(float);
		void Render();

		void RenderSceneToGBuffer();

		void LoadMaterials(const YumeString&);

		void SetLightParameters();
		void SetInjectStageTextures();
		void SetMaterialParameters();
		void SetGIParameters();
		void SetDeferredLightParameters();

		void UpdateMeshBb(YumeMesh& mesh);

	public:
		bool GetGIDebug() const { return debug_gi; }
		
		void SetGIDebug(bool enabled) { debug_gi = enabled; }
		void SetGIScale(float f) { gi_scale = f;}
		void SetLPVFlux(float f) { lpv_flux_amplifier = f; updateRsm_ = true;}
		void SetNumIterations(int num) { number_it = num; updateRsm_ = true;}
		void SetLPVPos(float x,float y,float z);
		void SetLightFlux(float f);
		YumeMiscRenderer* misc_;
	private:
		void DrawScene(bool shadowPass);

		bool updateRsm_;
	public:
		directional_light dir_light_;


	private:
		float moveScale;

		void RenderRSM();
		void SetRSMCamera();

		void RenderLPV();

	public:
		DirectX::XMVECTOR dirLightUp;
		DirectX::XMVECTOR dirLightFlux;
	private:
		float gi_scale;
		float lpv_flux_amplifier;
		bool debug_gi;
		unsigned number_it;

		DirectX::XMFLOAT4X4 lightViewProj;
		float yaw_;
		float pitch_;

		YumeGeometry* cornell_;
		YumeGeometry* plane_;
		YumeStaticModel* model_;
		YumeVector<YumeMaterial*>::type materials_;



		YumeGeometry* triangle_;


		DirectX::XMFLOAT4X4 world_;
		DirectX::XMFLOAT4X4 view_;
		DirectX::XMFLOAT4X4 proj_;

		DirectX::XMVECTOR materialDiffuse_;


		YumeShaderVariation* meshVs_;
		YumeShaderVariation* meshPs_;

		YumeShaderVariation* triangleVs_;
		YumeShaderVariation* deferredLpvPs_;

		SharedPtr<YumeTexture2D> noiseTex_;

		//RSM Render Targets
		SharedPtr<YumeTexture2D> RsmColors_;
		SharedPtr<YumeTexture2D> RsmNormals_;
		SharedPtr<YumeTexture2D> RsmLinearDepth_;
		SharedPtr<YumeTexture2D> RsmDummySpec_;
		SharedPtr<YumeTexture2D> RsmDepthStencil_;

		//Scene Render Targetss
		SharedPtr<YumeTexture2D> SceneColors_;
		SharedPtr<YumeTexture2D> SceneNormals_;
		SharedPtr<YumeTexture2D> SceneLinearDepth_;
		SharedPtr<YumeTexture2D> SceneSpecular_;


		LightPropagationVolume lpv_;

		YumeRHI* rhi_;
		YumeRenderer* renderer_;
	};
}


//----------------------------------------------------------------------------
#endif

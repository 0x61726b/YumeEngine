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
#ifndef __YumePostProcess_h_
#define __YumePostProcess_h_
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeTexture3D;

	class YumeAPIExport YumePostProcess : public YumeBase
	{
	public:
		YumePostProcess(YumeMiscRenderer*);

		virtual ~YumePostProcess();

		void Setup();
		void SetPPParameters();
		void Render();
		void Render(YumeShaderVariation* ps,YumeTexture2D* in,YumeTexture2D* out);

		void SSAO(YumeTexture2D* target);

		void DoF(YumeTexture2D* in,YumeTexture2D* out);
		void DoFBlur(YumeTexture2D* in,YumeTexture2D* out);

		void Godrays(YumeTexture2D* in,YumeTexture2D* out);

		void Bloom(YumeTexture2D* target);
		void BloomBlur(YumeTexture2D* in,YumeTexture2D* out);
		
		//Bloom
		void SetBloomThreshold(float f) {bloom_treshold = f;}
		void SetBloomSigma(float f) {bloom_sigma= f;}

		//SSAO
		void SetSSAOScale(float f) {ssao_scale= f;}

		//Auto Exposure
		void SetAutoExposureKey(float f) {exposure_key= f;}
		void SetAutoExposureSpeed(float f) {exposure_speed= f;}

		//DoF
		void SetDoFCoCScale(float f) {dof_coc_scale= f;}
		void SetDoFFocalPlane(float f) {dof_focal_plane= f;}

		//Godrays
		void SetGodraysTAU(float f) { godrays_tau = f;}

		void SetBloomEnabled(bool enabled) { bloom_enabled = enabled; }
		void SetSSAOEnabled(bool enabled) { ssao_enabled = enabled; }
		void SetGodraysEnabled(bool enabled) { godrays_enabled = enabled; }
		void SetDofEnabled(bool enabled) { dof_enabled = enabled; }
		void SetFxaaEnabled(bool enabled) { fxaa_enabled = enabled; }
		void SetExposureAdapt(bool enabled) { exposure_adapt = enabled; }
	private:
		void SetViewport(YumeTexture2D* rendertarget);
	private:
		YumeMiscRenderer* misc_;
	private:
		bool ssao_enabled;
		float ssao_scale;

		bool  godrays_enabled;
		float godrays_tau;

		bool  dof_enabled;
		float dof_focal_plane;
		float dof_coc_scale;

		bool  bloom_enabled;
		float bloom_sigma;
		float bloom_treshold;

		bool  fxaa_enabled;

		bool  exposure_adapt;
		float exposure_key;
		float exposure_speed;

		SharedPtr<YumeTexture2D> temporaryRt_;
		SharedPtr<YumeTexture2D> adaptadLuminanceRt_[2];

		SharedPtr<YumeTexture2D> blurred_[6];
		SharedPtr<YumeTexture2D> bloomFull_;

		SharedPtr<YumeTexture2D> frontBufferBlurred_;

		SharedPtr<YumeTexture3D> noiseTex_;

	private:
		YumeShaderVariation* ssaoPs_;

		YumeShaderVariation* adaptExposure_;
		YumeShaderVariation* bloom_;
		YumeShaderVariation* bloomThreshold_;

		YumeShaderVariation* godrays_;
		YumeShaderVariation* godraysMerge_;

		YumeShaderVariation* dof_;

		YumeShaderVariation* fxaa_;

		YumeShaderVariation* gaussBloomV_;
		YumeShaderVariation* gaussBloomH_;

		YumeShaderVariation* gaussDofV_;
		YumeShaderVariation* gaussDofH_;

		YumeShaderVariation* gaussGodraysV_;
		YumeShaderVariation* gaussGodraysH_;

		YumeShaderVariation* copy_;


	};
}


//----------------------------------------------------------------------------
#endif

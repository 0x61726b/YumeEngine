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
#include "YumeHeaders.h"
#include "Renderer/YumeRHI.h"
#include "YumeRenderer.h"

#include "YumeTexture2D.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeInputLayout.h"
#include "YumeRendererEnv.h"
#include "YumeRenderView.h"
#include "YumeAuxRenderer.h"
#include "YumeTexture3D.h"
#include "YumeTextureCube.h"
#include "YumeOcclusion.h"
#include "YumeRenderPass.h"

#include "YumeGeometry.h"
#include "YumeMaterial.h"
#include "Scene/YumeOctree.h"

#include "Core/YumeXmlFile.h"

#include "YumeRenderPipeline.h"

#include "Renderer/YumeResourceManager.h"
#include "Engine/YumeEngine.h"
#include "Scene/YumeScene.h"
#include "Math/YumeMatrix3x4.h"

#include "YumeCamera.h"

#include "Logging/logging.h"

#include "LPVRendererTest.h"

namespace YumeEngine
{
	struct Vertex32
	{
		Vector3 Pos;
		Vector3 Normal;
		Vector2 Tex;
	};
	static const float dirLightVertexData[] =
	{
		-1,1,0,
		1,1,0,
		1,-1,0,
		-1,-1,0,
	};

	static const float texturedQuadVertexData[] =
	{
		-1,1,0,0,0,
		1,1,0,1,0,
		1,-1,0,1,1,
		-1,-1,0,0,1
	};

	static const unsigned short dirLightIndexData[] =
	{
		0,1,2,
		2,3,0,
	};

	static const float ssaoQuadVertexData[] =
	{
		-1,1,0,1,0,0,0,0,
		1,1,0,2,0,0,1,0,
		1,-1,0,3,0,0,1,1
		-1,1,0,0,0,0,0,1,
	};

	static const unsigned short ssaoQuadIndices[] =
	{
		0,1,2,
		0,2,3
	};


	static const float pointLightVertexData[] =
	{
		-0.423169f,-1.000000f,0.423169f,
		-0.423169f,-1.000000f,-0.423169f,
		0.423169f,-1.000000f,-0.423169f,
		0.423169f,-1.000000f,0.423169f,
		0.423169f,1.000000f,-0.423169f,
		-0.423169f,1.000000f,-0.423169f,
		-0.423169f,1.000000f,0.423169f,
		0.423169f,1.000000f,0.423169f,
		-1.000000f,0.423169f,-0.423169f,
		-1.000000f,-0.423169f,-0.423169f,
		-1.000000f,-0.423169f,0.423169f,
		-1.000000f,0.423169f,0.423169f,
		0.423169f,0.423169f,-1.000000f,
		0.423169f,-0.423169f,-1.000000f,
		-0.423169f,-0.423169f,-1.000000f,
		-0.423169f,0.423169f,-1.000000f,
		1.000000f,0.423169f,0.423169f,
		1.000000f,-0.423169f,0.423169f,
		1.000000f,-0.423169f,-0.423169f,
		1.000000f,0.423169f,-0.423169f,
		0.423169f,-0.423169f,1.000000f,
		0.423169f,0.423169f,1.000000f,
		-0.423169f,0.423169f,1.000000f,
		-0.423169f,-0.423169f,1.000000f
	};

	static const unsigned short pointLightIndexData[] =
	{
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23,
		0,10,9,
		0,9,1,
		13,2,1,
		13,1,14,
		23,0,3,
		23,3,20,
		17,3,2,
		17,2,18,
		21,7,6,
		21,6,22,
		7,16,19,
		7,19,4,
		5,8,11,
		5,11,6,
		4,12,15,
		4,15,5,
		22,11,10,
		22,10,23,
		8,15,14,
		8,14,9,
		12,19,18,
		12,18,13,
		16,21,20,
		16,20,17,
		0,23,10,
		1,9,14,
		2,13,18,
		3,17,20,
		6,11,22,
		5,15,8,
		4,19,12,
		7,21,16
	};

	static const float spotLightVertexData[] =
	{
		0.00001f,0.00001f,0.00001f,
		0.00001f,-0.00001f,0.00001f,
		-0.00001f,-0.00001f,0.00001f,
		-0.00001f,0.00001f,0.00001f,
		1.00000f,1.00000f,0.99999f,
		1.00000f,-1.00000f,0.99999f,
		-1.00000f,-1.00000f,0.99999f,
		-1.00000f,1.00000f,0.99999f,
	};

	static const unsigned short spotLightIndexData[] =
	{
		3,0,1,
		3,1,2,
		0,4,5,
		0,5,1,
		3,7,4,
		3,4,0,
		7,3,2,
		7,2,6,
		6,2,1,
		6,1,5,
		7,5,4,
		7,6,5
	};

	static const char* geometryVSVariations[] =
	{
		"",
		"SKINNED ",
		"INSTANCED ",
		"BILLBOARD ",
		"DIRBILLBOARD "
	};

	static const char* lightVSVariations[] =
	{
		"PERPIXEL DIRLIGHT ",
		"PERPIXEL SPOTLIGHT ",
		"PERPIXEL POINTLIGHT ",
		"PERPIXEL DIRLIGHT SHADOW ",
		"PERPIXEL SPOTLIGHT SHADOW ",
		"PERPIXEL POINTLIGHT SHADOW ",
	};

	static const char* vertexLightVSVariations[] =
	{
		"",
		"NUMVERTEXLIGHTS=1 ",
		"NUMVERTEXLIGHTS=2 ",
		"NUMVERTEXLIGHTS=3 ",
		"NUMVERTEXLIGHTS=4 ",
	};

	static const char* deferredLightVSVariations[] =
	{
		"",
		"DIRLIGHT ",
		"ORTHO ",
		"DIRLIGHT ORTHO "
	};

	static const char* lightPSVariations[] =
	{
		"PERPIXEL DIRLIGHT ",
		"PERPIXEL SPOTLIGHT ",
		"PERPIXEL POINTLIGHT ",
		"PERPIXEL POINTLIGHT CUBEMASK ",
		"PERPIXEL DIRLIGHT SPECULAR ",
		"PERPIXEL SPOTLIGHT SPECULAR ",
		"PERPIXEL POINTLIGHT SPECULAR ",
		"PERPIXEL POINTLIGHT CUBEMASK SPECULAR ",
		"PERPIXEL DIRLIGHT SHADOW ",
		"PERPIXEL SPOTLIGHT SHADOW ",
		"PERPIXEL POINTLIGHT SHADOW ",
		"PERPIXEL POINTLIGHT CUBEMASK SHADOW ",
		"PERPIXEL DIRLIGHT SPECULAR SHADOW ",
		"PERPIXEL SPOTLIGHT SPECULAR SHADOW ",
		"PERPIXEL POINTLIGHT SPECULAR SHADOW ",
		"PERPIXEL POINTLIGHT CUBEMASK SPECULAR SHADOW "
	};

	static const char* heightFogVariations[] =
	{
		"",
		"HEIGHTFOG "
	};

	static const unsigned INSTANCING_BUFFER_MASK = MASK_INSTANCEMATRIX1 | MASK_INSTANCEMATRIX2 | MASK_INSTANCEMATRIX3;
	static const unsigned MAX_BUFFER_AGE = 1000;

	YumeHash YumeRenderer::type_ = "Renderer";

	YumeRenderer::YumeRenderer(const YumeString& name):
		name_(name),
		defaultZone_(SharedPtr<YumeRendererEnvironment>(new YumeRendererEnvironment)),
		shadowMapFilterInstance_(0),
		shadowMapFilter_(0),
		textureAnisotropy_(4),
		textureFilterMode_(FILTER_TRILINEAR),
		textureQuality_(QUALITY_HIGH),
		materialQuality_(QUALITY_HIGH),
		shadowMapSize_(2048),
		shadowQuality_(SHADOWQUALITY_PCF_24BIT),
		shadowSoftness_(2.0f),
		vsmShadowParams_(0.0000001f,0.2f),
		maxShadowMaps_(1),
		minInstances_(2),
		maxSortedInstances_(1000),
		maxOccluderTriangles_(5000),
		occlusionBufferSize_(256),
		occluderSizeThreshold_(0.025f),
		mobileShadowBiasMul_(2.0f),
		mobileShadowBiasAdd_(0.0001f),
		numOcclusionBuffers_(0),
		numShadowCameras_(0),
		shadersChangedFrameNumber_(M_MAX_UNSIGNED),
		hdrRendering_(false),
		debugGBufferRendering_(false),
		specularLighting_(true),
		drawShadows_(true),
		reuseShadowMaps_(true),
		dynamicInstancing_(true),
		threadedOcclusion_(false),
		shadersDirty_(true),
		initialized_(false),
		resetViews_(false),
		randomVectorMap_(0),
		lpvRenderer_(0)
	{
		REGISTER_ENGINE_LISTENER;
	}

	YumeRenderer::~YumeRenderer()
	{

	}


	void YumeRenderer::AddListener(RendererEventListener* listener)
	{
		if(listeners_.Contains(listener))
			return;
		listeners_.push_back(listener);
	}

	void YumeRenderer::RemoveListener(RendererEventListener* listener)
	{
		RendererListeners::iterator i = listeners_.find(listener);
		if(i != listeners_.end())
			listeners_.erase(i);
	}

	void YumeRenderer::FireEvent(YumeEngineEvents e)
	{
		switch(e)
		{
		case R_RENDERTARGETUPDATE:
		{
			for(RendererListeners::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleRenderTargetUpdate();
		}
		break;
		}
	}

	void YumeRenderer::FireEvent(YumeEngineEvents e,YumeRenderView* view,YumeRenderable* renderable,YumeTexture* texture,YumeScene* scene,YumeCamera* camera)
	{
		switch(e)
		{
		case R_BEGINVIEWUPDATE:
		{
			for(RendererListeners::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleBeginViewUpdate(view,renderable,texture,scene,camera);
		}
		break;
		case R_ENDVIEWUPDATE:
		{
			for(RendererListeners::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
				(*i)->HandleEndViewUpdate(view,renderable,texture,scene,camera);
		}
		break;
		}
	}

	void YumeRenderer::SetNumViewports(unsigned num)
	{
		viewports_.resize(num);
	}

	void YumeRenderer::SetViewport(unsigned index,YumeViewport* viewport)
	{
		if(index >= viewports_.size())
			viewports_.resize(index + 1);

		viewports_[index] = (viewport);

		CreateSSAOResources();
	}


	void YumeRenderer::SetDefaultRenderPath(YumeXmlFile* xmlFile)
	{
		SharedPtr<YumeRenderPipeline> newRenderPath(new YumeRenderPipeline());
		if(newRenderPath->Load(xmlFile))
			defaultPipeline_ = newRenderPath;
	}

	void YumeRenderer::SetHDRRendering(bool enable)
	{
		hdrRendering_ = enable;
	}
	void YumeRenderer::SetGBufferDebugRendering(bool enable)
	{
		debugGBufferRendering_ = enable;
	}
	void YumeRenderer::SetSpecularLighting(bool enable)
	{
		specularLighting_ = enable;
	}

	void YumeRenderer::SetTextureAnisotropy(int level)
	{
		textureAnisotropy_ = Max(level,1);
	}

	void YumeRenderer::SetTextureFilterMode(TextureFilterMode mode)
	{
		textureFilterMode_ = mode;
	}

	void YumeRenderer::SetTextureQuality(int quality)
	{
		quality = Clamp(quality,QUALITY_LOW,QUALITY_HIGH);

		if(quality != textureQuality_)
		{
			textureQuality_ = quality;
			ReloadTextures();
		}
	}

	void YumeRenderer::SetMaterialQuality(int quality)
	{
		quality = Clamp(quality,QUALITY_LOW,QUALITY_MAX);

		if(quality != materialQuality_)
		{
			materialQuality_ = quality;
			shadersDirty_ = true;
			// Reallocate views to not store eg. pass information that might be unnecessary on the new material quality level
			resetViews_ = true;
		}
	}

	void YumeRenderer::SetDrawShadows(bool enable)
	{
		if(!gYume->pRHI || !gYume->pRHI->GetShadowMapFormat())
			return;

		drawShadows_ = enable;
		if(!drawShadows_)
			ResetShadowMaps();
	}

	void YumeRenderer::SetShadowMapSize(int size)
	{
		if(!gYume->pRHI)
			return;

		size = NextPowerOfTwo((unsigned)Max(size,SHADOW_MIN_PIXELS));
		if(size != shadowMapSize_)
		{
			shadowMapSize_ = size;
			ResetShadowMaps();
		}
	}

	void YumeRenderer::SetShadowQuality(ShadowQuality quality)
	{
		if(!gYume->pRHI)
			return;

		// If no hardware PCF, do not allow to select one-sample quality
		if(!gYume->pRHI->GetHardwareShadowSupport())
		{
			if(quality == SHADOWQUALITY_SIMPLE_16BIT)
				quality = SHADOWQUALITY_PCF_16BIT;

			if(quality == SHADOWQUALITY_SIMPLE_24BIT)
				quality = SHADOWQUALITY_PCF_24BIT;
		}
		// if high resolution is not allowed
		if(!gYume->pRHI->GetHiresShadowMapFormat())
		{
			if(quality == SHADOWQUALITY_SIMPLE_24BIT)
				quality = SHADOWQUALITY_SIMPLE_16BIT;

			if(quality == SHADOWQUALITY_PCF_24BIT)
				quality = SHADOWQUALITY_PCF_16BIT;
		}
		if(quality != shadowQuality_)
		{
			shadowQuality_ = quality;
			shadersDirty_ = true;
			if(quality == SHADOWQUALITY_BLUR_VSM)
				SetShadowMapFilter(this,static_cast<ShadowMapFilter>(&YumeRenderer::BlurShadowMap));
			else
				SetShadowMapFilter(0,0);

			ResetShadowMaps();
		}
	}

	void YumeRenderer::SetShadowSoftness(float shadowSoftness)
	{
		shadowSoftness_ = Max(shadowSoftness,0.0f);
	}

	void YumeRenderer::SetVSMShadowParameters(float minVariance,float lightBleedingReduction)
	{
		vsmShadowParams_.x_ = Max(minVariance,0.0f);
		vsmShadowParams_.y_ = Clamp(lightBleedingReduction,0.0f,1.0f);
	}

	void YumeRenderer::SetShadowMapFilter(YumeBase* instance,ShadowMapFilter functionPtr)
	{
		shadowMapFilterInstance_ = instance;
		shadowMapFilter_ = functionPtr;
	}

	void YumeRenderer::SetReuseShadowMaps(bool enable)
	{
		reuseShadowMaps_ = enable;
	}

	void YumeRenderer::SetMaxShadowMaps(int shadowMaps)
	{
		if(shadowMaps < 1)
			return;

		maxShadowMaps_ = shadowMaps;
		for(YumeMap<int,YumeVector<SharedPtr<YumeTexture2D> >::type >::iterator i = shadowMaps_.begin(); i != shadowMaps_.end(); ++i)
		{
			if((int)i->second.size() > maxShadowMaps_)
				i->second.resize((unsigned)maxShadowMaps_);
		}
	}

	void YumeRenderer::SetDynamicInstancing(bool enable)
	{
		if(!instancingBuffer_)
			enable = false;

		dynamicInstancing_ = enable;
	}

	void YumeRenderer::SetMinInstances(int instances)
	{
		minInstances_ = Max(instances,2);
	}

	void YumeRenderer::SetMaxSortedInstances(int instances)
	{
		maxSortedInstances_ = Max(instances,0);
	}

	void YumeRenderer::SetMaxOccluderTriangles(int triangles)
	{
		maxOccluderTriangles_ = Max(triangles,0);
	}

	void YumeRenderer::SetOcclusionBufferSize(int size)
	{
		occlusionBufferSize_ = Max(size,1);
		occlusionBuffers_.clear();
	}

	void YumeRenderer::SetMobileShadowBiasMul(float mul)
	{
		mobileShadowBiasMul_ = mul;
	}

	void YumeRenderer::SetMobileShadowBiasAdd(float add)
	{
		mobileShadowBiasAdd_ = add;
	}

	void YumeRenderer::SetOccluderSizeThreshold(float screenSize)
	{
		occluderSizeThreshold_ = Max(screenSize,0.0f);
	}

	void YumeRenderer::SetThreadedOcclusion(bool enable)
	{
		if(enable != threadedOcclusion_)
		{
			threadedOcclusion_ = enable;
			occlusionBuffers_.clear();
		}
	}

	void YumeRenderer::ReloadShaders()
	{
		shadersDirty_ = true;
	}

	void YumeRenderer::ApplyShadowMapFilter(YumeRenderView* view,YumeTexture2D* shadowMap)
	{
		if(shadowMapFilterInstance_ && shadowMapFilter_)
			(shadowMapFilterInstance_->*shadowMapFilter_)(view,shadowMap);
	}

	YumeViewport* YumeRenderer::GetViewport(unsigned index) const
	{
		return index < viewports_.size() ? viewports_[index] : (YumeViewport*)0;
	}

	SharedPtr<YumeRenderPipeline> YumeRenderer::GetDefaultPipeline() const
	{
		return defaultPipeline_;
	}

	unsigned YumeRenderer::GetNumGeometries(bool allViews) const
	{
		unsigned numGeometries = 0;
		unsigned lastView = allViews ? views_.size() : 1;

		for(unsigned i = 0; i < lastView; ++i)
		{
			// Use the source view's statistics if applicable
			YumeRenderView* view = GetActualView(views_[i]);
			if(!view)
				continue;

			numGeometries += view->GetGeometries().size();
		}

		return numGeometries;
	}

	unsigned YumeRenderer::GetNumLights(bool allViews) const
	{
		unsigned numLights = 0;
		unsigned lastView = allViews ? views_.size() : 1;

		for(unsigned i = 0; i < lastView; ++i)
		{
			YumeRenderView* view = GetActualView(views_[i]);
			if(!view)
				continue;

			numLights += view->GetLights().size();
		}

		return numLights;
	}

	unsigned YumeRenderer::GetNumShadowMaps(bool allViews) const
	{
		unsigned numShadowMaps = 0;
		unsigned lastView = allViews ? views_.size() : 1;

		for(unsigned i = 0; i < lastView; ++i)
		{
			YumeRenderView* view = GetActualView(views_[i]);
			if(!view)
				continue;

			const YumeVector<LightBatchQueue>::type& lightQueues = view->GetLightQueues();
			for(YumeVector<LightBatchQueue>::const_iterator i = lightQueues.begin(); i != lightQueues.end(); ++i)
			{
				if(i->shadowMap_)
					++numShadowMaps;
			}
		}

		return numShadowMaps;
	}

	unsigned YumeRenderer::GetNumOccluders(bool allViews) const
	{
		unsigned numOccluders = 0;
		unsigned lastView = allViews ? views_.size() : 1;

		for(unsigned i = 0; i < lastView; ++i)
		{
			YumeRenderView* view = GetActualView(views_[i]);
			if(!view)
				continue;

			numOccluders += view->GetNumActiveOccluders();
		}

		return numOccluders;
	}

	void YumeRenderer::Update(float timeStep)
	{
		views_.clear();
		preparedViews_.clear();

		if(!gYume->pRHI || !gYume->pRHI->IsInitialized() || gYume->pRHI->IsDeviceLost())
			return;

		frame_.frameNumber_ = gYume->pTimer->GetFrameNumber();
		frame_.timeStep_ = timeStep;
		frame_.camera_ = 0;
		numShadowCameras_ = 0;
		numOcclusionBuffers_ = 0;
		updatedOctrees_.clear();


		if(shadersDirty_)
			LoadShaders();

		for(unsigned i = viewports_.size() - 1; i < viewports_.size(); --i)
			QueueViewport(0,viewports_[i]);


		FireEvent(R_RENDERTARGETUPDATE);

		// Process gathered views. This may queue further views (render surfaces that are only updated when visible)
		for(unsigned i = 0; i < queuedViewports_.size(); ++i)
		{
			WeakPtr<YumeRenderable>& renderTarget = queuedViewports_[i].first;
			WeakPtr<YumeViewport>& viewport = queuedViewports_[i].second;

			// Null pointer means backbuffer view. Differentiate between that and an expired rendersurface
			if((renderTarget.NotNull() && renderTarget.Expired()) || viewport.Expired())
				continue;

			// (Re)allocate the view structure if necessary
			if(!viewport->GetView() || resetViews_)
				viewport->AllocateView();

			YumeRenderView* view = viewport->GetView();
			assert(view);
			// Check if view can be defined successfully (has either valid scene, camera and octree, or no scene passes)
			if(!view->Define(renderTarget,viewport))
				continue;

			views_.push_back(WeakPtr<YumeRenderView>(view));

			const IntRect& viewRect = viewport->GetRect();
			YumeScene* scene = viewport->GetScene();
			if(!scene)
				continue;

			Octree* octree = scene->GetComponent<Octree>();

			// Update octree (perform early update for drawables which need that, and reinsert moved drawables.)
			// However, if the same scene is viewed from multiple cameras, update the octree only once
			if(!updatedOctrees_.Contains(octree))
			{
				frame_.camera_ = viewport->GetCamera();
				frame_.viewSize_ = viewRect.Size();
				if(frame_.viewSize_ == IntVector2::ZERO)
					frame_.viewSize_ = IntVector2(gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight());
				octree->Update(frame_);
				updatedOctrees_.insert(octree);

				// Set also the view for the debug renderer already here, so that it can use culling
				/// \todo May result in incorrect debug geometry culling if the same scene is drawn from multiple viewports
				YumeDebugRenderer* debug = scene->GetComponent<YumeDebugRenderer>();
				if(debug)
					debug->SetView(viewport->GetCamera());
			}

			// Update view. This may queue further views. View will send update begin/end events once its state is set
			ResetShadowMapAllocations(); // Each view can reuse the same shadow maps
			view->Update(frame_);
		}

		queuedViewports_.clear();
		resetViews_ = false;
	}

	void YumeRenderer::Render()
	{
		// Engine does not render when window is closed or device is lost
		assert(gYume->pRHI && gYume->pRHI->IsInitialized() && !gYume->pRHI->IsDeviceLost());

		lpvRenderer_->Render();

		//// If the indirection textures have lost content (OpenGL mode only), restore them now
		//if(faceSelectCubeMap_ && faceSelectCubeMap_->IsDataLost())
		//	SetIndirectionTextureData();

		//gYume->pRHI->SetDefaultTextureFilterMode(textureFilterMode_);
		//gYume->pRHI->SetTextureAnisotropy((unsigned)textureAnisotropy_);

		//// If no views, just clear the screen
		//if(views_.empty())
		//{
		//	gYume->pRHI->SetBlendMode(BLEND_REPLACE);
		//	gYume->pRHI->SetColorWrite(true);
		//	gYume->pRHI->SetDepthWrite(true);
		//	gYume->pRHI->SetScissorTest(false);
		//	gYume->pRHI->SetStencilTest(false);
		//	gYume->pRHI->ResetRenderTargets();
		//	gYume->pRHI->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL,defaultZone_->GetFogColor());

		//	numPrimitives_ = 0;
		//	numBatches_ = 0;
		//}
		//else
		//{
		//	// Render views from last to first (each main view is rendered after the auxiliary views it depends on)
		//	for(unsigned i = views_.size() - 1; i < views_.size(); --i)
		//	{
		//		if(views_[i].Expired())
		//			continue;

		//		YumeRenderable* renderTarget = views_[i]->GetRenderTarget();
		//		FireEvent(R_BEGINVIEWRENDER,views_[i],renderTarget,(renderTarget ? renderTarget->GetParentTexture() : 0),views_[i]->GetScene(),views_[i]->GetCamera());

		//		RHIEvent event("Begin Rendering View");

		//		// Screen buffers can be reused between views, as each is rendered completely
		//		PrepareViewRender();
		//		views_[i]->Render();

		//		FireEvent(R_ENDVIEWRENDER,views_[i],renderTarget,(renderTarget ? renderTarget->GetParentTexture() : 0),views_[i]->GetScene(),views_[i]->GetCamera());
		//	}

		//	// Copy the number of batches & primitives from Graphics so that we can account for 3D geometry only
		//	numPrimitives_ = gYume->pRHI->GetNumPrimitives();
		//	numBatches_ = gYume->pRHI->GetNumBatches();
		//}

		//// Remove unused occlusion buffers and renderbuffers
		//RemoveUnusedBuffers();
	}

	void YumeRenderer::DrawDebugGeometry(bool depthTest)
	{
		YumeVector<YumeDrawable*>::type processedGeometries;
		YumeVector<YumeLight*>::type processedLights;

		for(unsigned i = 0; i < views_.size(); ++i)
		{
			YumeRenderView* view = views_[i];
			if(!view || !view->GetDrawDebug())
				continue;
			Octree* octree = view->GetOctree();
			if(!octree)
				continue;
			YumeDebugRenderer* debug = octree->GetComponent<YumeDebugRenderer>();
			if(!debug || !debug->IsEnabledEffective())
				continue;

			// Process geometries / lights only once
			const YumePodVector<YumeDrawable*>::type& geometries = view->GetGeometries();
			const YumePodVector<YumeLight*>::type& lights = view->GetLights();

			for(unsigned i = 0; i < geometries.size(); ++i)
			{
				if(!processedGeometries.Contains(geometries[i]))
				{
					geometries[i]->DrawDebugGeometry(debug,depthTest);
					processedGeometries.push_back(geometries[i]);
				}
			}
			for(unsigned i = 0; i < lights.size(); ++i)
			{
				if(!processedLights.Contains(lights[i]))
				{
					lights[i]->DrawDebugGeometry(debug,depthTest);
					processedLights.push_back(lights[i]);
				}
			}
		}
	}

	void YumeRenderer::QueueRenderable(YumeRenderable* renderTarget)
	{
		if(renderTarget)
		{
			unsigned numViewports = renderTarget->GetNumViewports();

			for(unsigned i = 0; i < numViewports; ++i)
				QueueViewport(renderTarget,renderTarget->GetViewport(i));
		}
	}

	void YumeRenderer::QueueViewport(YumeRenderable* renderTarget,YumeViewport* viewport)
	{
		if(viewport)
		{
			Pair<WeakPtr<YumeRenderable>,WeakPtr<YumeViewport> > newView = MakePair(WeakPtr<YumeRenderable>(renderTarget),
				WeakPtr<YumeViewport>(viewport));

			if(!queuedViewports_.Contains(newView))
				queuedViewports_.push_back(newView);
		}
	}

	YumeGeometry* YumeRenderer::GetLightGeometry(YumeLight* light)
	{
		switch(light->GetLightType())
		{
		case LIGHT_DIRECTIONAL:
			return dirLightGeometry_;
		case LIGHT_SPOT:
			return spotLightGeometry_;
		case LIGHT_POINT:
			return pointLightGeometry_;
		}

		return 0;
	}

	YumeGeometry* YumeRenderer::GetQuadGeometry()
	{
		return dirLightGeometry_;
	}
	YumeGeometry* YumeRenderer::GetTexturedQuadGeometry()
	{
		return texturedQuadGeometry;
	}
	YumeGeometry* YumeRenderer::GetSSAOQuadGeometry()
	{
		return ssaoQuad_;
	}

	YumeTexture2D* YumeRenderer::GetShadowMap(YumeLight* light,YumeCamera* camera,unsigned viewWidth,unsigned viewHeight)
	{
		LightType type = light->GetLightType();
		const FocusParameters& parameters = light->GetShadowFocus();
		float size = (float)shadowMapSize_ * light->GetShadowResolution();
		// Automatically reduce shadow map size when far away
		if(parameters.autoSize_ && type != LIGHT_DIRECTIONAL)
		{
			const Matrix3x4& view = camera->GetView();
			const Matrix4& projection = camera->GetProjection();
			BoundingBox lightBox;
			float lightPixels;

			if(type == LIGHT_POINT)
			{
				// Calculate point light pixel size from the projection of its diagonal
				Vector3 center = view * light->GetNode()->GetWorldPosition();
				float extent = 0.58f * light->GetRange();
				lightBox.Define(center + Vector3(extent,extent,extent),center - Vector3(extent,extent,extent));
			}
			else
			{
				// Calculate spot light pixel size from the projection of its frustum far vertices
				Frustum lightFrustum = light->GetFrustum().Transformed(view);
				lightBox.Define(&lightFrustum.vertices_[4],4);
			}

			Vector2 projectionSize = lightBox.Projected(projection).Size();
			lightPixels = Max(0.5f * (float)viewWidth * projectionSize.x_,0.5f * (float)viewHeight * projectionSize.y_);

			// Clamp pixel amount to a sufficient minimum to avoid self-shadowing artifacts due to loss of precision
			if(lightPixels < SHADOW_MIN_PIXELS)
				lightPixels = SHADOW_MIN_PIXELS;

			size = Min(size,lightPixels);
		}

		/// \todo Allow to specify maximum shadow maps per resolution, as smaller shadow maps take less memory
		int width = NextPowerOfTwo((unsigned)size);
		int height = width;

		// Adjust the size for directional or point light shadow map atlases
		if(type == LIGHT_DIRECTIONAL)
		{
			unsigned numSplits = (unsigned)light->GetNumShadowSplits();
			if(numSplits > 1)
				width *= 2;
			if(numSplits > 2)
				height *= 2;
		}
		else if(type == LIGHT_POINT)
		{
			width *= 2;
			height *= 3;
		}

		int searchKey = (width << 16) | height;
		if(shadowMaps_.find(searchKey) != shadowMaps_.end())
		{
			// If shadow maps are reused, always return the first
			if(reuseShadowMaps_)
				return shadowMaps_[searchKey][0];
			else
			{
				// If not reused, check allocation count and return existing shadow map if possible
				unsigned allocated = shadowMapAllocations_[searchKey].size();
				if(allocated < shadowMaps_[searchKey].size())
				{
					shadowMapAllocations_[searchKey].push_back(light);
					return shadowMaps_[searchKey][allocated];
				}
				else if((int)allocated >= maxShadowMaps_)
					return 0;
			}
		}

		// Find format and usage of the shadow map
		unsigned shadowMapFormat = 0;
		TextureUsage shadowMapUsage = TEXTURE_DEPTHSTENCIL;

		switch(shadowQuality_)
		{
		case SHADOWQUALITY_SIMPLE_16BIT:
		case SHADOWQUALITY_PCF_16BIT:
			shadowMapFormat = gYume->pRHI->GetShadowMapFormat();
			break;

		case SHADOWQUALITY_SIMPLE_24BIT:
		case SHADOWQUALITY_PCF_24BIT:
			shadowMapFormat = gYume->pRHI->GetHiresShadowMapFormat();
			break;

		case SHADOWQUALITY_VSM:
		case SHADOWQUALITY_BLUR_VSM:
			shadowMapFormat = gYume->pRHI->GetRGFloat32FormatNs();
			shadowMapUsage = TEXTURE_RENDERTARGET;
			break;
		}

		if(!shadowMapFormat)
			return 0;

		SharedPtr<YumeTexture2D> newShadowMap = SharedPtr<YumeTexture2D>(gYume->pRHI->CreateTexture2D());
		int retries = 3;
		unsigned dummyColorFormat = gYume->pRHI->GetDummyColorFormat();

		while(retries)
		{
			if(!newShadowMap->SetSize(width,height,shadowMapFormat,shadowMapUsage))
			{
				width >>= 1;
				height >>= 1;
				--retries;
			}
			else
			{
				// OpenGL (desktop) and D3D11: shadow compare mode needs to be specifically enabled for the shadow map
				newShadowMap->SetFilterMode(FILTER_BILINEAR);
				newShadowMap->SetShadowCompare(shadowMapUsage == TEXTURE_DEPTHSTENCIL);
				if(GetName() != "OpenGL")
				{
					newShadowMap->SetFilterMode(gYume->pRHI->GetHardwareShadowSupport() ? FILTER_BILINEAR : FILTER_NEAREST);
				}
				// Create dummy color texture for the shadow map if necessary: Direct3D9, or OpenGL when working around an OS X +
				// Intel driver bug
				if(shadowMapUsage == TEXTURE_DEPTHSTENCIL && dummyColorFormat)
				{
					// If no dummy color rendertarget for this size exists yet, create one now
					if(colorShadowMaps_.find(searchKey) == colorShadowMaps_.end())
					{
						colorShadowMaps_[searchKey] = SharedPtr<YumeTexture2D>(gYume->pRHI->CreateTexture2D());
						colorShadowMaps_[searchKey]->SetSize(width,height,dummyColorFormat,TEXTURE_RENDERTARGET);
					}
					// Link the color rendertarget to the shadow map
					newShadowMap->GetRenderSurface()->SetLinkedRenderTarget(colorShadowMaps_[searchKey]->GetRenderSurface());
				}
				break;
			}
		}

		// If failed to set size, store a null pointer so that we will not retry
		if(!retries)
			newShadowMap.Reset();

		shadowMaps_[searchKey].push_back(newShadowMap);
		if(!reuseShadowMaps_)
			shadowMapAllocations_[searchKey].push_back(light);

		return newShadowMap;
	}

	YumeTexture* YumeRenderer::GetScreenBuffer(int width,int height,unsigned format,bool cubemap,bool filtered,bool srgb,
		unsigned persistentKey)
	{
		bool depthStencil = (format == gYume->pRHI->GetDepthStencilFormatNs()) || (format == gYume->pRHI->GetReadableDepthFormatNs());
		if(depthStencil)
		{
			filtered = false;
			srgb = false;
		}

		if(cubemap)
			height = width;

		long long searchKey = ((long long)format << 32) | (width << 16) | height;
		if(filtered)
			searchKey |= 0x8000000000000000LL;
		if(srgb)
			searchKey |= 0x4000000000000000LL;
		if(cubemap)
			searchKey |= 0x2000000000000000LL;

		// Add persistent key if defined
		if(persistentKey)
			searchKey += ((long long)persistentKey << 32);

		// If new size or format, initialize the allocation stats
		if(screenBuffers_.find(searchKey) == screenBuffers_.end())
			screenBufferAllocations_[searchKey] = 0;

		// Reuse depth-stencil buffers whenever the size matches, instead of allocating new
		unsigned allocations = screenBufferAllocations_[searchKey];
		if(!depthStencil)
			++screenBufferAllocations_[searchKey];

		if(allocations >= screenBuffers_[searchKey].size())
		{
			SharedPtr<YumeTexture> newBuffer;

			if(!cubemap)
			{
				SharedPtr<YumeTexture2D> newTex2D = SharedPtr<YumeTexture2D>(gYume->pRHI->CreateTexture2D());
				newTex2D->SetSize(width,height,format,depthStencil ? TEXTURE_DEPTHSTENCIL : TEXTURE_RENDERTARGET);

				if(GetName() == "OpenGL")
				{
					// OpenGL hack: clear persistent floating point screen buffers to ensure the initial contents aren't illegal (NaN)?
					// Otherwise eg. the AutoExposure post process will not work correctly
					if(persistentKey && gYume->pRHI->GetOpenGLOnlyTextureDataType(format) == 0x1406) //GL_FLOAT
					{
						// Note: this loses current rendertarget assignment
						gYume->pRHI->ResetRenderTargets();
						gYume->pRHI->SetRenderTarget(0,newTex2D);
						gYume->pRHI->SetDepthStencil((YumeRenderable*)0);
						gYume->pRHI->SetViewport(IntRect(0,0,width,height));
						gYume->pRHI->Clear(CLEAR_COLOR);
					}
				}

				newBuffer = newTex2D;
			}
			else
			{
				SharedPtr<YumeTextureCube> newTexCube = SharedPtr<YumeTextureCube>(gYume->pRHI->CreateTextureCube());
				newTexCube->SetSize(width,format,TEXTURE_RENDERTARGET);

				newBuffer = newTexCube;
			}

			newBuffer->SetSRGB(srgb);
			newBuffer->SetFilterMode(filtered ? FILTER_BILINEAR : FILTER_NEAREST);
			newBuffer->ResetUseTimer();
			screenBuffers_[searchKey].push_back(newBuffer);

			YUMELOG_DEBUG("Allocated new screen buffer size " + std::to_string(width) + "x" + std::to_string(height) + " format " + std::to_string(format));
			return newBuffer;
		}
		else
		{
			YumeTexture* buffer = screenBuffers_[searchKey][allocations];
			buffer->ResetUseTimer();
			return buffer;
		}
	}

	YumeRenderable* YumeRenderer::GetDepthStencil(int width,int height)
	{
		// Return the default depth-stencil surface if applicable
		// (when using OpenGL Graphics will allocate right size surfaces on demand to emulate Direct3D9)
		if(width == gYume->pRHI->GetWidth() && height == gYume->pRHI->GetHeight() && gYume->pRHI->GetMultiSample() <= 1)
			return 0;
		else
		{
			return static_cast<YumeTexture2D*>(GetScreenBuffer(width,height,gYume->pRHI->GetDepthStencilFormatNs(),false,false,
				false))->GetRenderSurface();
		}
	}

	OcclusionBuffer* YumeRenderer::GetOcclusionBuffer(YumeCamera* camera)
	{
		assert(numOcclusionBuffers_ <= occlusionBuffers_.size());
		if(numOcclusionBuffers_ == occlusionBuffers_.size())
		{
			SharedPtr<OcclusionBuffer> newBuffer(new OcclusionBuffer());
			occlusionBuffers_.push_back(newBuffer);
		}

		int width = occlusionBufferSize_;
		int height = (int)((float)occlusionBufferSize_ / camera->GetAspectRatio() + 0.5f);

		OcclusionBuffer* buffer = occlusionBuffers_[numOcclusionBuffers_++];
		buffer->SetSize(width,height,threadedOcclusion_);
		buffer->SetView(camera);
		buffer->ResetUseTimer();

		return buffer;
	}

	YumeCamera* YumeRenderer::GetShadowCamera()
	{
		MutexLock lock(rendererMutex_);

		assert(numShadowCameras_ <= shadowCameraNodes_.size());
		if(numShadowCameras_ == shadowCameraNodes_.size())
		{
			SharedPtr<YumeSceneNode> newNode(new YumeSceneNode);
			newNode->CreateComponent<YumeCamera>();
			shadowCameraNodes_.push_back(newNode);
		}

		YumeCamera* camera = shadowCameraNodes_[numShadowCameras_++]->GetComponent<YumeCamera>();
		camera->SetOrthographic(false);
		camera->SetZoom(1.0f);

		return camera;
	}

	void YumeRenderer::StorePreparedView(YumeRenderView* view,YumeCamera* camera)
	{
		if(view && camera)
			preparedViews_[camera] = view;
	}

	YumeRenderView* YumeRenderer::GetPreparedView(YumeCamera* camera)
	{
		YumeMap<YumeCamera*,YumeRenderView*>::iterator i = preparedViews_.find(camera);
		return i != preparedViews_.end() ? i->second : (YumeRenderView*)0;
	}

	YumeRenderView* YumeRenderer::GetActualView(YumeRenderView* view)
	{
		if(view && view->GetSourceView())
			return view->GetSourceView();
		else
			return view;
	}

	void YumeRenderer::SetBatchShaders(Batch& batch,YumeRenderTechnique* tech,bool allowShadows)
	{
		// Check if shaders are unloaded or need reloading
		YumeRenderPass* pass = batch.pass_;
		YumeVector<SharedPtr<YumeShaderVariation> >::type& vertexShaders = pass->GetVertexShaders();
		YumeVector<SharedPtr<YumeShaderVariation> >::type& pixelShaders = pass->GetPixelShaders();
		YumeVector<SharedPtr<YumeShaderVariation> >::type& geometryShaders = pass->GetGeometryShaders();

		if(!vertexShaders.size() || !pixelShaders.size() || pass->GetShadersLoadedFrameNumber() != shadersChangedFrameNumber_)
		{
			// First release all previous shaders, then load
			pass->ReleaseShaders();
			LoadPassShaders(pass);
		}

		// Make sure shaders are loaded now
		if(vertexShaders.size() && pixelShaders.size())
		{
			bool heightFog = batch.zone_ && batch.zone_->GetHeightFog();

			// If instancing is not supported, but was requested, choose static geometry vertex shader instead
			if(batch.geometryType_ == GEOM_INSTANCED && !GetDynamicInstancing())
				batch.geometryType_ = GEOM_STATIC;

			if(batch.geometryType_ == GEOM_STATIC_NOINSTANCING)
				batch.geometryType_ = GEOM_STATIC;

			//  Check whether is a pixel lit forward pass. If not, there is only one pixel shader
			if(pass->GetLightingMode() == LIGHTING_PERPIXEL)
			{
				LightBatchQueue* lightQueue = batch.lightQueue_;
				if(!lightQueue)
				{
					// Do not log error, as it would result in a lot of spam
					batch.vertexShader_ = 0;
					batch.pixelShader_ = 0;
					batch.geometryShader_ = 0;
					return;
				}

				YumeLight* light = lightQueue->light_;
				unsigned vsi = 0;
				unsigned psi = 0;
				vsi = batch.geometryType_ * MAX_LIGHT_VS_VARIATIONS;

				bool materialHasSpecular = batch.material_ ? batch.material_->GetSpecular() : true;
				if(specularLighting_ && light->GetSpecularIntensity() > 0.0f && materialHasSpecular)
					psi += LPS_SPEC;
				if(allowShadows && lightQueue->shadowMap_)
				{
					vsi += LVS_SHADOW;
					psi += LPS_SHADOW;
				}

				switch(light->GetLightType())
				{
				case LIGHT_DIRECTIONAL:
					vsi += LVS_DIR;
					break;

				case LIGHT_SPOT:
					psi += LPS_SPOT;
					vsi += LVS_SPOT;
					break;

				case LIGHT_POINT:
					if(light->GetShapeTexture())
						psi += LPS_POINTMASK;
					else
						psi += LPS_POINT;
					vsi += LVS_POINT;
					break;
				}

				if(heightFog)
					psi += MAX_LIGHT_PS_VARIATIONS;

				batch.vertexShader_ = vertexShaders[vsi];
				batch.pixelShader_ = pixelShaders[psi];
			}
			else
			{
				// Check if pass has vertex lighting support
				if(pass->GetLightingMode() == LIGHTING_PERVERTEX)
				{
					unsigned numVertexLights = 0;
					if(batch.lightQueue_)
						numVertexLights = batch.lightQueue_->vertexLights_.size();

					unsigned vsi = batch.geometryType_ * MAX_VERTEXLIGHT_VS_VARIATIONS + numVertexLights;
					batch.vertexShader_ = vertexShaders[vsi];
				}
				else
				{
					unsigned vsi = batch.geometryType_;
					batch.vertexShader_ = vertexShaders[vsi];
				}

				batch.pixelShader_ = pixelShaders[heightFog ? 1 : 0];
			}
		}

		if(pass->GetGeometryShader().length())
		{
			batch.geometryShader_ = geometryShaders[0];
		}
		else
		{
			batch.geometryShader_ = 0;
		}

		// Log error if shaders could not be assigned, but only once per technique
		if(!batch.vertexShader_ || !batch.pixelShader_ || !batch.geometryShader_)
		{
			if(!shaderErrorDisplayed_.Contains(tech))
			{
				shaderErrorDisplayed_.insert(tech);
				YUMELOG_ERROR("Technique has missing shaders" << tech->GetName().c_str());
			}
		}
	}

	void YumeRenderer::SetLightVolumeBatchShaders(Batch& batch,YumeCamera* camera,const YumeString& vsName,const YumeString& psName,const YumeString& vsDefines,
		const YumeString& psDefines)
	{
		assert(deferredLightPSVariations_.size());

		unsigned vsi = DLVS_NONE;
		unsigned psi = DLPS_NONE;
		YumeLight* light = batch.lightQueue_->light_;

		switch(light->GetLightType())
		{
		case LIGHT_DIRECTIONAL:
			vsi += DLVS_DIR;
			break;

		case LIGHT_SPOT:
			psi += DLPS_SPOT;
			break;

		case LIGHT_POINT:
			if(light->GetShapeTexture())
				psi += DLPS_POINTMASK;
			else
				psi += DLPS_POINT;
			break;
		}


		if(batch.lightQueue_->shadowMap_)
			psi += DLPS_SHADOW;

		if(specularLighting_ && light->GetSpecularIntensity() > 0.0f)
			psi += DLPS_SPEC;

		if(camera->IsOrthographic())
		{
			vsi += DLVS_ORTHO;
			psi += DLPS_ORTHO;
		}

		if(vsDefines.length())
			batch.vertexShader_ = gYume->pRHI->GetShader(VS,vsName,deferredLightVSVariations[vsi] + vsDefines);
		else
			batch.vertexShader_ = gYume->pRHI->GetShader(VS,vsName,deferredLightVSVariations[vsi]);

		if(psDefines.length())
			batch.pixelShader_ = gYume->pRHI->GetShader(PS,psName,deferredLightPSVariations_[psi] + psDefines);
		else
			batch.pixelShader_ = gYume->pRHI->GetShader(PS,psName,deferredLightPSVariations_[psi]);

		batch.geometryShader_ = 0;
	}

	void YumeRenderer::SetCullMode(CullMode mode,YumeCamera* camera)
	{
		// If a camera is specified, check whether it reverses culling due to vertical flipping or reflection
		if(camera && camera->GetReverseCulling())
		{
			if(mode == CULL_CW)
				mode = CULL_CCW;
			else if(mode == CULL_CCW)
				mode = CULL_CW;
		}

		gYume->pRHI->SetCullMode(mode);
	}

	bool YumeRenderer::ResizeInstancingBuffer(unsigned numInstances)
	{
		if(!instancingBuffer_ || !dynamicInstancing_)
			return false;

		unsigned oldSize = instancingBuffer_->GetVertexCount();
		if(numInstances <= oldSize)
			return true;

		unsigned newSize = INSTANCING_BUFFER_DEFAULT_SIZE;
		while(newSize < numInstances)
			newSize <<= 1;

		if(!instancingBuffer_->SetSize(newSize,INSTANCING_BUFFER_MASK,true))
		{
			YUMELOG_ERROR("Failed to resize instancing buffer to " + std::to_string(newSize));
			// If failed, try to restore the old size
			instancingBuffer_->SetSize(oldSize,INSTANCING_BUFFER_MASK,true);
			return false;
		}

		YUMELOG_DEBUG("Resized instancing buffer to " + std::to_string(newSize));
		return true;
	}

	void YumeRenderer::SaveScreenBufferAllocations()
	{
		savedScreenBufferAllocations_ = screenBufferAllocations_;
	}

	void YumeRenderer::RestoreScreenBufferAllocations()
	{
		screenBufferAllocations_ = savedScreenBufferAllocations_;
	}


	void YumeRenderer::OptimizeLightByScissor(YumeLight* light,YumeCamera* camera)
	{
		if(light && light->GetLightType() != LIGHT_DIRECTIONAL)
			gYume->pRHI->SetScissorTest(true,GetLightScissor(light,camera));
		else
			gYume->pRHI->SetScissorTest(false);
	}

	void YumeRenderer::OptimizeLightByStencil(YumeLight* light,YumeCamera* camera)
	{
		if(light)
		{
			LightType type = light->GetLightType();
			if(type == LIGHT_DIRECTIONAL)
			{
				gYume->pRHI->SetStencilTest(false);
				return;
			}

			YumeGeometry* geometry = GetLightGeometry(light);
			const Matrix3x4& view = camera->GetView();
			const Matrix4& projection = camera->GetProjection();
			Vector3 cameraPos = camera->GetNode()->GetWorldPosition();
			float lightDist;

			if(type == LIGHT_POINT)
				lightDist = Sphere(light->GetNode()->GetWorldPosition(),light->GetRange() * 1.25f).Distance(cameraPos);
			else
				lightDist = light->GetFrustum().Distance(cameraPos);

			// If the camera is actually inside the light volume, do not draw to stencil as it would waste fillrate
			if(lightDist < M_EPSILON)
			{
				gYume->pRHI->SetStencilTest(false);
				return;
			}

			// If the stencil value has wrapped, clear the whole stencil first
			if(!lightStencilValue_)
			{
				gYume->pRHI->Clear(CLEAR_STENCIL);
				lightStencilValue_ = 1;
			}

			// If possible, render the stencil volume front faces. However, close to the near clip plane render back faces instead
			// to avoid clipping.
			if(lightDist < camera->GetNearClip() * 2.0f)
			{
				SetCullMode(CULL_CW,camera);
				gYume->pRHI->SetDepthTest(CMP_GREATER);
			}
			else
			{
				SetCullMode(CULL_CCW,camera);
				gYume->pRHI->SetDepthTest(CMP_LESSEQUAL);
			}

			gYume->pRHI->SetColorWrite(false);
			gYume->pRHI->SetDepthWrite(false);
			gYume->pRHI->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,lightStencilValue_);
			gYume->pRHI->SetShaders(gYume->pRHI->GetShader(VS,"Stencil"),gYume->pRHI->GetShader(PS,"Stencil"));
			gYume->pRHI->SetShaderParameter(VSP_VIEW,view);
			gYume->pRHI->SetShaderParameter(VSP_VIEWINV,camera->GetEffectiveWorldTransform());
			gYume->pRHI->SetShaderParameter(VSP_VIEWPROJ,projection * view);
			gYume->pRHI->SetShaderParameter(VSP_MODEL,light->GetVolumeTransform(camera));

			geometry->Draw(gYume->pRHI);

			gYume->pRHI->ClearTransformSources();
			gYume->pRHI->SetColorWrite(true);
			gYume->pRHI->SetStencilTest(true,CMP_EQUAL,OP_KEEP,OP_KEEP,OP_KEEP,lightStencilValue_);

			// Increase stencil value for next light
			++lightStencilValue_;
		}
		else
			gYume->pRHI->SetStencilTest(false);
	}

	const Rect& YumeRenderer::GetLightScissor(YumeLight* light,YumeCamera* camera)
	{
		Pair<YumeLight*,YumeCamera*> combination(light,camera);

		YumeMap<Pair<YumeLight*,YumeCamera*>,Rect>::iterator i = lightScissorCache_.find(combination);
		if(i != lightScissorCache_.end())
			return i->second;

		const Matrix3x4& view = camera->GetView();
		const Matrix4& projection = camera->GetProjection();

		assert(light->GetLightType() != LIGHT_DIRECTIONAL);
		if(light->GetLightType() == LIGHT_SPOT)
		{
			Frustum viewFrustum(light->GetFrustum().Transformed(view));
			return lightScissorCache_[combination] = viewFrustum.Projected(projection);
		}
		else // LIGHT_POINT
		{
			BoundingBox viewBox(light->GetWorldBoundingBox().Transformed(view));
			return lightScissorCache_[combination] = viewBox.Projected(projection);
		}
	}

	void YumeRenderer::PrepareViewRender()
	{
		ResetScreenBufferAllocations();
		lightScissorCache_.clear();
		lightStencilValue_ = 1;


	}

	void YumeRenderer::RemoveUnusedBuffers()
	{
		for(unsigned i = occlusionBuffers_.size() - 1; i < occlusionBuffers_.size(); --i)
		{
			if(occlusionBuffers_[i]->GetUseTimer() > MAX_BUFFER_AGE)
			{
				YUMELOG_DEBUG("Removed unused occlusion buffer");
				occlusionBuffers_.erase(occlusionBuffers_.begin()+i);
			}
		}

		for(YumeMap<long long,YumeVector<SharedPtr<YumeTexture> >::type >::iterator i = screenBuffers_.begin(); i != screenBuffers_.end();)
		{
			YumeMap<long long,YumeVector<SharedPtr<YumeTexture> >::type >::iterator current = i++;
			YumeVector<SharedPtr<YumeTexture> >::type& buffers = current->second;
			for(unsigned j = buffers.size() - 1; j < buffers.size(); --j)
			{
				YumeTexture* buffer = buffers[j];
				if(buffer->GetUseTimer() > MAX_BUFFER_AGE)
				{
					YUMELOG_DEBUG("Removed unused screen buffer size " + std::to_string(buffer->GetWidth()) + "x" + std::to_string(buffer->GetHeight()) +
						" format " + std::to_string(buffer->GetFormat()));
					buffers.erase(buffers.begin()+j);
				}
			}
			if(buffers.empty())
			{
				screenBufferAllocations_.erase(current->first);
				screenBuffers_.erase(current);
			}
		}
	}

	void YumeRenderer::ResetShadowMapAllocations()
	{
		for(YumeMap<int,YumeVector<YumeLight*>::type >::iterator i = shadowMapAllocations_.begin(); i != shadowMapAllocations_.end(); ++i)
			i->second.clear();
	}

	void YumeRenderer::ResetScreenBufferAllocations()
	{
		for(YumeMap<long long,unsigned>::iterator i = screenBufferAllocations_.begin(); i != screenBufferAllocations_.end(); ++i)
			i->second = 0;
	}


	void YumeRenderer::Initialize()
	{
		YumeResourceManager* cache = gYume->pResourceManager;

		if(!gYume->pRHI || !gYume->pRHI->IsInitialized() || !cache)
			return;


		if(!gYume->pRHI->GetShadowMapFormat())
			drawShadows_ = false;
		// Validate the shadow quality level
		SetShadowQuality(shadowQuality_);

		defaultLightRamp_ = cache->PrepareResource<YumeTexture2D>("Textures/Ramp.png");
		defaultLightSpot_ = cache->PrepareResource<YumeTexture2D>("Textures/Spot.png");
		defaultMaterial_ = (new YumeMaterial);

		defaultPipeline_ = (new YumeRenderPipeline());
		defaultPipeline_->Load(cache->PrepareResource<YumeXmlFile>("Pipelines/DeferredSSAO.xml"));
		CreateGeometries();
		CreateInstancingBuffer();

		viewports_.resize(1);
		ResetShadowMaps();
		ResetBuffers();


		lpvRenderer_ = new LPVRenderer;





		randomVectorMap_ = gYume->pRHI->CreateTexture2D();
		randomVectorMap_->SetSize(256,256,gYume->pRHI->GetRGBAFormatNs());

		shadersDirty_ = true;
		initialized_ = true;


		YUMELOG_INFO("Initialized renderer");
	}

	void YumeRenderer::LoadShaders()
	{
		YUMELOG_INFO("Reloading shaders..");

		// Release old material shaders, mark them for reload
		ReleaseMaterialShaders();
		shadersChangedFrameNumber_ = gYume->pTimer->GetFrameNumber();

		// Construct new names for deferred light volume pixel shaders based on rendering options
		deferredLightPSVariations_.resize(MAX_DEFERRED_LIGHT_PS_VARIATIONS);

		for(unsigned i = 0; i < MAX_DEFERRED_LIGHT_PS_VARIATIONS; ++i)
		{
			deferredLightPSVariations_[i] = lightPSVariations[i % DLPS_ORTHO];
			if(i & DLPS_SHADOW)
				deferredLightPSVariations_[i] += GetShadowVariations();
			if(i & DLPS_ORTHO)
				deferredLightPSVariations_[i] += "ORTHO ";
		}

		shadersDirty_ = false;
	}

	void YumeRenderer::LoadPassShaders(YumeRenderPass* pass)
	{
		YumeVector<SharedPtr<YumeShaderVariation> >::type& vertexShaders = pass->GetVertexShaders();
		YumeVector<SharedPtr<YumeShaderVariation> >::type& pixelShaders = pass->GetPixelShaders();
		YumeVector<SharedPtr<YumeShaderVariation> >::type& geometryShaders = pass->GetGeometryShaders();

		// Forget all the old shaders
		vertexShaders.clear();
		pixelShaders.clear();
		geometryShaders.clear();

		YumeString extraShaderDefines = " ";
		if(pass->GetName() == "shadow"
			&& (shadowQuality_ == SHADOWQUALITY_VSM || shadowQuality_ == SHADOWQUALITY_BLUR_VSM))
		{
			extraShaderDefines = " VSM_SHADOW ";
		}

		if(pass->GetLightingMode() == LIGHTING_PERPIXEL)
		{
			// Load forward pixel lit variations
			vertexShaders.resize(MAX_GEOMETRYTYPES * MAX_LIGHT_VS_VARIATIONS);
			pixelShaders.resize(MAX_LIGHT_PS_VARIATIONS * 2);

			for(unsigned j = 0; j < MAX_GEOMETRYTYPES * MAX_LIGHT_VS_VARIATIONS; ++j)
			{
				unsigned g = j / MAX_LIGHT_VS_VARIATIONS;
				unsigned l = j % MAX_LIGHT_VS_VARIATIONS;

				vertexShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(VS,pass->GetVertexShader(),
					pass->GetVertexShaderDefines() + extraShaderDefines + lightVSVariations[l] + geometryVSVariations[g]));
			}
			for(unsigned j = 0; j < MAX_LIGHT_PS_VARIATIONS * 2; ++j)
			{
				unsigned l = j % MAX_LIGHT_PS_VARIATIONS;
				unsigned h = j / MAX_LIGHT_PS_VARIATIONS;

				if(l & LPS_SHADOW)
				{
					pixelShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(PS,pass->GetPixelShader(),
						pass->GetPixelShaderDefines() + extraShaderDefines + lightPSVariations[l] + GetShadowVariations() +
						heightFogVariations[h]));
				}
				else
					pixelShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(PS,pass->GetPixelShader(),
					pass->GetPixelShaderDefines() + extraShaderDefines + lightPSVariations[l] + heightFogVariations[h]));
			}
		}
		else
		{
			// Load vertex light variations
			if(pass->GetLightingMode() == LIGHTING_PERVERTEX)
			{
				vertexShaders.resize(MAX_GEOMETRYTYPES * MAX_VERTEXLIGHT_VS_VARIATIONS);
				for(unsigned j = 0; j < MAX_GEOMETRYTYPES * MAX_VERTEXLIGHT_VS_VARIATIONS; ++j)
				{
					unsigned g = j / MAX_VERTEXLIGHT_VS_VARIATIONS;
					unsigned l = j % MAX_VERTEXLIGHT_VS_VARIATIONS;
					vertexShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(VS,pass->GetVertexShader(),
						pass->GetVertexShaderDefines() + extraShaderDefines + vertexLightVSVariations[l] + geometryVSVariations[g]));
				}
			}
			else
			{
				vertexShaders.resize(MAX_GEOMETRYTYPES);
				for(unsigned j = 0; j < MAX_GEOMETRYTYPES; ++j)
				{
					vertexShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(VS,pass->GetVertexShader(),
						pass->GetVertexShaderDefines() + extraShaderDefines + geometryVSVariations[j]));
				}
			}

			pixelShaders.resize(2);
			for(unsigned j = 0; j < 2; ++j)
			{
				pixelShaders[j] = SharedPtr<YumeShaderVariation>(gYume->pRHI->GetShader(PS,pass->GetPixelShader(),pass->GetPixelShaderDefines() + extraShaderDefines + heightFogVariations[j]));
			}
		}

		if(pass->GetGeometryShader().length() > 0)
		{
			geometryShaders.resize(1);
			geometryShaders[0] = gYume->pRHI->GetShader(GS,pass->GetGeometryShader(),pass->GetGeometryShaderDefines());
		}

		pass->MarkShadersLoaded(shadersChangedFrameNumber_);
	}

	void YumeRenderer::ReleaseMaterialShaders()
	{
		YumeResourceManager* cache = gYume->pResourceManager;
		YumeVector<YumeMaterial*>::type materials;

		cache->GetResources<YumeMaterial>(materials);

		for(unsigned i = 0; i < materials.size(); ++i)
			materials[i]->ReleaseShaders();
	}

	void YumeRenderer::ReloadTextures()
	{
		YumeResourceManager* cache = gYume->pResourceManager;
		YumeVector<YumeResource*>::type textures;

		cache->GetResources(textures,YumeTexture2D::GetTypeStatic());
		for(unsigned i = 0; i < textures.size(); ++i)
			cache->ReloadResource(textures[i]);

		cache->GetResources(textures,YumeTextureCube::GetTypeStatic());
		for(unsigned i = 0; i < textures.size(); ++i)
			cache->ReloadResource(textures[i]);
	}

	void YumeRenderer::CreateGeometries()
	{
		SharedPtr<YumeVertexBuffer> dlvb = SharedPtr<YumeVertexBuffer>(gYume->pRHI->CreateVertexBuffer());
		dlvb->SetShadowed(true);
		dlvb->SetSize(4,MASK_POSITION);
		dlvb->SetData(dirLightVertexData);

		SharedPtr<YumeIndexBuffer> dlib = SharedPtr<YumeIndexBuffer>(gYume->pRHI->CreateIndexBuffer());
		dlib->SetShadowed(true);
		dlib->SetSize(6,false);
		dlib->SetData(dirLightIndexData);

		dirLightGeometry_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		dirLightGeometry_->SetVertexBuffer(0,dlvb);
		dirLightGeometry_->SetIndexBuffer(dlib);
		dirLightGeometry_->SetDrawRange(TRIANGLE_LIST,0,dlib->GetIndexCount());

		SharedPtr<YumeVertexBuffer> slvb = SharedPtr<YumeVertexBuffer>(gYume->pRHI->CreateVertexBuffer());
		slvb->SetShadowed(true);
		slvb->SetSize(8,MASK_POSITION);
		slvb->SetData(spotLightVertexData);

		SharedPtr<YumeIndexBuffer> slib = SharedPtr<YumeIndexBuffer>(gYume->pRHI->CreateIndexBuffer());
		slib->SetShadowed(true);
		slib->SetSize(36,false);
		slib->SetData(spotLightIndexData);

		spotLightGeometry_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		spotLightGeometry_->SetVertexBuffer(0,slvb);
		spotLightGeometry_->SetIndexBuffer(slib);
		spotLightGeometry_->SetDrawRange(TRIANGLE_LIST,0,slib->GetIndexCount());

		SharedPtr<YumeVertexBuffer> plvb = SharedPtr<YumeVertexBuffer>(gYume->pRHI->CreateVertexBuffer());
		plvb->SetShadowed(true);
		plvb->SetSize(24,MASK_POSITION);
		plvb->SetData(pointLightVertexData);

		SharedPtr<YumeIndexBuffer> plib = SharedPtr<YumeIndexBuffer>(gYume->pRHI->CreateIndexBuffer());
		plib->SetShadowed(true);
		plib->SetSize(132,false);
		plib->SetData(pointLightIndexData);

		pointLightGeometry_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		pointLightGeometry_->SetVertexBuffer(0,plvb);
		pointLightGeometry_->SetIndexBuffer(plib);
		pointLightGeometry_->SetDrawRange(TRIANGLE_LIST,0,plib->GetIndexCount());

		SharedPtr<YumeVertexBuffer> spvb(gYume->pRHI->CreateVertexBuffer());
		spvb->SetShadowed(true);
		spvb->SetSize(4,MASK_POSITION | MASK_TEXCOORD1) ;
		spvb->SetData(texturedQuadVertexData);


		SharedPtr<YumeIndexBuffer> spib(gYume->pRHI->CreateIndexBuffer());
		spib->SetShadowed(true);
		spib->SetSize(6,false);
		spib->SetData(dirLightIndexData);

		texturedQuadGeometry = new YumeGeometry();
		texturedQuadGeometry->SetVertexBuffer(0,spvb);
		texturedQuadGeometry->SetIndexBuffer(spib);
		texturedQuadGeometry->SetDrawRange(TRIANGLE_LIST,0,spib->GetIndexCount());

		//SSAO Quad
		Vertex32 v[4];

		v[0].Pos = Vector3(-1.0f,-1.0f,0.0f);
		v[1].Pos = Vector3(-1.0f,+1.0f,0.0f);
		v[2].Pos = Vector3(+1.0f,+1.0f,0.0f);
		v[3].Pos = Vector3(+1.0f,-1.0f,0.0f);

		// Store far plane frustum corner indices in Normal.x slot.
		v[0].Normal = Vector3(0.0f,0.0f,0.0f);
		v[1].Normal = Vector3(1.0f,0.0f,0.0f);
		v[2].Normal = Vector3(2.0f,0.0f,0.0f);
		v[3].Normal = Vector3(3.0f,0.0f,0.0f);

		v[0].Tex = Vector2(0.0f,1.0f);
		v[1].Tex = Vector2(0.0f,0.0f);
		v[2].Tex = Vector2(1.0f,0.0f);
		v[3].Tex = Vector2(1.0f,1.0f);

		int sizeTest = sizeof(Vertex32);

		SharedPtr<YumeVertexBuffer> ssaoVb(gYume->pRHI->CreateVertexBuffer());
		ssaoVb->SetShadowed(true);
		ssaoVb->SetSize(4,MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1) ;
		ssaoVb->SetData(&v[0]);
		unsigned sizeTest2 = ssaoVb->GetVertexSize();

		SharedPtr<YumeIndexBuffer> ssaoIb(gYume->pRHI->CreateIndexBuffer());
		ssaoIb->SetShadowed(true);
		ssaoIb->SetSize(6,false);
		ssaoIb->SetData(ssaoQuadIndices);

		ssaoQuad_ = new YumeGeometry();
		ssaoQuad_->SetVertexBuffer(0,ssaoVb);
		ssaoQuad_->SetIndexBuffer(ssaoIb);
		ssaoQuad_->SetDrawRange(TRIANGLE_LIST,0,ssaoIb->GetIndexCount());


		//




		//Not opengl
		if(gYume->pRHI->GetShadowMapFormat())
		{
			faceSelectCubeMap_ = SharedPtr<YumeTextureCube>(gYume->pRHI->CreateTextureCube());
			faceSelectCubeMap_->SetNumLevels(1);
			faceSelectCubeMap_->SetSize(1,gYume->pRHI->GetRGBAFormatNs());
			faceSelectCubeMap_->SetFilterMode(FILTER_NEAREST);

			indirectionCubeMap_ =SharedPtr<YumeTextureCube>(gYume->pRHI->CreateTextureCube());
			indirectionCubeMap_->SetNumLevels(1);
			indirectionCubeMap_->SetSize(256,gYume->pRHI->GetRGBAFormatNs());
			indirectionCubeMap_->SetFilterMode(FILTER_BILINEAR);
			indirectionCubeMap_->SetAddressMode(COORD_U,ADDRESS_CLAMP);
			indirectionCubeMap_->SetAddressMode(COORD_V,ADDRESS_CLAMP);
			indirectionCubeMap_->SetAddressMode(COORD_W,ADDRESS_CLAMP);

			SetIndirectionTextureData();
		}
	}

	void YumeRenderer::CreateSSAOResources()
	{
		CreateRandomColors();

		YumeViewport* viewport = gYume->pRenderer->GetViewport(0);
		YumeRenderPipeline* fx = viewport->GetRenderPath();
		YumeCamera* cam = viewport->GetCamera();

		float aspect = (float)gYume->pRHI->GetWidth()/ (float)gYume->pRHI->GetHeight();

		float fovy = 0.25f * M_PI;
		float halfHeight = cam->GetFarClip() * tanf(0.5f*fovy);
		float halfWidth  = aspect * halfHeight;

		frustumCorners_[0] = Vector4(-halfWidth,-halfHeight,cam->GetFarClip(),0.0f);
		frustumCorners_[1] = Vector4(-halfWidth,+halfHeight,cam->GetFarClip(),0.0f);
		frustumCorners_[2] = Vector4(+halfWidth,+halfHeight,cam->GetFarClip(),0.0f);
		frustumCorners_[3] = Vector4(+halfWidth,-halfHeight,cam->GetFarClip(),0.0f);

		vectorOffsets_[0] = Vector4(+1.0f,+1.0f,+1.0f,0.0f);
		vectorOffsets_[1] = Vector4(-1.0f,-1.0f,-1.0f,0.0f);

		vectorOffsets_[2] = Vector4(-1.0f,+1.0f,+1.0f,0.0f);
		vectorOffsets_[3] = Vector4(+1.0f,-1.0f,-1.0f,0.0f);

		vectorOffsets_[4] = Vector4(+1.0f,+1.0f,-1.0f,0.0f);
		vectorOffsets_[5] = Vector4(-1.0f,-1.0f,+1.0f,0.0f);

		vectorOffsets_[6] = Vector4(-1.0f,+1.0f,-1.0f,0.0f);
		vectorOffsets_[7] = Vector4(+1.0f,-1.0f,+1.0f,0.0f);

		// 6 centers of cube faces
		vectorOffsets_[8] = Vector4(-1.0f,0.0f,0.0f,0.0f);
		vectorOffsets_[9] = Vector4(+1.0f,0.0f,0.0f,0.0f);

		vectorOffsets_[10] = Vector4(0.0f,-1.0f,0.0f,0.0f);
		vectorOffsets_[11] = Vector4(0.0f,+1.0f,0.0f,0.0f);

		vectorOffsets_[12] = Vector4(0.0f,0.0f,-1.0f,0.0f);
		vectorOffsets_[13] = Vector4(0.0f,0.0f,+1.0f,0.0f);

		for(int i = 0; i < 14; ++i)
		{
			// Create random lengths in [0.25, 1.0].
			float s = Random(0.25f,1.0f);


			Vector4 v = vectorOffsets_[i].Normalized();

			vectorOffsets_[i] = s * v;
		}
	}

	void YumeRenderer::CreateRandomColors()
	{

		unsigned char data[256*256*4];
		unsigned char* dest = data;
		for(int i = 0; i < 256; ++i)
		{
			for(int j = 0; j < 256; ++j)
			{
				dest[0] = Random(0,255);
				dest[1] = Random(0,255);
				dest[2] = Random(0,255);
				dest[3] = Random(0,255);

				dest += 4;
			}
		}

		randomVectorMap_->SetData(0,0,0,256,256,&data);
	}

	void YumeRenderer::SetIndirectionTextureData()
	{
		unsigned char data[256 * 256 * 4];

		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			unsigned axis = i / 2;
			data[0] = (unsigned char)((axis == 0) ? 255 : 0);
			data[1] = (unsigned char)((axis == 1) ? 255 : 0);
			data[2] = (unsigned char)((axis == 2) ? 255 : 0);
			data[3] = 0;
			faceSelectCubeMap_->SetData((CubeMapFace)i,0,0,0,1,1,data);
		}

		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			unsigned char faceX = (unsigned char)((i & 1) * 255);
			unsigned char faceY = (unsigned char)((i / 2) * 255 / 3);
			unsigned char* dest = data;
			for(unsigned y = 0; y < 256; ++y)
			{
				for(unsigned x = 0; x < 256; ++x)
				{
#ifdef URHO3D_OPENGL
					dest[0] = (unsigned char)x;
					dest[1] = (unsigned char)(255 - y);
					dest[2] = faceX;
					dest[3] = (unsigned char)(255 * 2 / 3 - faceY);
#else
					dest[0] = (unsigned char)x;
					dest[1] = (unsigned char)y;
					dest[2] = faceX;
					dest[3] = faceY;
#endif
					dest += 4;
		}
	}

			indirectionCubeMap_->SetData((CubeMapFace)i,0,0,0,256,256,data);
}

		faceSelectCubeMap_->ClearDataLost();
		indirectionCubeMap_->ClearDataLost();
	}

	void YumeRenderer::CreateInstancingBuffer()
	{
		// Do not create buffer if instancing not supported
		if(!gYume->pRHI->GetInstancingSupport())
		{
			instancingBuffer_.Reset();
			dynamicInstancing_ = false;
			return;
		}

		instancingBuffer_ = SharedPtr<YumeVertexBuffer>(gYume->pRHI->CreateVertexBuffer());
		if(!instancingBuffer_->SetSize(INSTANCING_BUFFER_DEFAULT_SIZE,INSTANCING_BUFFER_MASK,true))
		{
			instancingBuffer_.Reset();
			dynamicInstancing_ = false;
		}
	}

	void YumeRenderer::ResetShadowMaps()
	{
		shadowMaps_.clear();
		shadowMapAllocations_.clear();
		colorShadowMaps_.clear();
	}

	void YumeRenderer::ResetBuffers()
	{
		occlusionBuffers_.clear();
		screenBuffers_.clear();
		screenBufferAllocations_.clear();
	}

	YumeString YumeRenderer::GetShadowVariations() const
	{
		switch(shadowQuality_)
		{
		case SHADOWQUALITY_SIMPLE_16BIT:
#ifdef URHO3D_OPENGL
			return "SIMPLE_SHADOW ";
#else
			if(gYume->pRHI->GetHardwareShadowSupport())
				return "SIMPLE_SHADOW ";
			else
				return "SIMPLE_SHADOW SHADOWCMP ";
#endif
		case SHADOWQUALITY_SIMPLE_24BIT:
			return "SIMPLE_SHADOW ";
		case SHADOWQUALITY_PCF_16BIT:
#ifdef URHO3D_OPENGL
			return "PCF_SHADOW ";
#else
			if(gYume->pRHI->GetHardwareShadowSupport())
				return "PCF_SHADOW ";
			else
				return "PCF_SHADOW SHADOWCMP ";
#endif
		case SHADOWQUALITY_PCF_24BIT:
			return "PCF_SHADOW ";
		case SHADOWQUALITY_VSM:
			return "VSM_SHADOW ";
		case SHADOWQUALITY_BLUR_VSM:
			return "VSM_SHADOW ";
	}
		return "";
	};

	void YumeRenderer::HandleGraphicsMode()
	{
		if(!initialized_)
			Initialize();
		else
			resetViews_ = true;
	}

	void YumeRenderer::HandleRenderUpdate(float timeStep)
	{
		Update(timeStep);
	}


	void YumeRenderer::BlurShadowMap(YumeRenderView* view,YumeTexture2D* shadowMap)
	{
		gYume->pRHI->SetBlendMode(BLEND_REPLACE);
		gYume->pRHI->SetDepthTest(CMP_ALWAYS);
		gYume->pRHI->SetClipPlane(false);
		gYume->pRHI->SetScissorTest(false);

		// Get a temporary render buffer
		YumeTexture2D* tmpBuffer = static_cast<YumeTexture2D*>(GetScreenBuffer(shadowMap->GetWidth(),shadowMap->GetHeight(),shadowMap->GetFormat(),false,false,false));
		gYume->pRHI->SetRenderTarget(0,tmpBuffer->GetRenderSurface());
		gYume->pRHI->SetDepthStencil(GetDepthStencil(shadowMap->GetWidth(),shadowMap->GetHeight()));
		gYume->pRHI->SetViewport(IntRect(0,0,shadowMap->GetWidth(),shadowMap->GetHeight()));

		// Get shaders
		static const YumeString shaderName("ShadowBlur");
		YumeShaderVariation* vs = gYume->pRHI->GetShader(VS,shaderName);
		YumeShaderVariation* ps = gYume->pRHI->GetShader(PS,shaderName);
		gYume->pRHI->SetShaders(vs,ps);

		view->SetGBufferShaderParameters(IntVector2(shadowMap->GetWidth(),shadowMap->GetHeight()),IntRect(0,0,shadowMap->GetWidth(),shadowMap->GetHeight()));

		// Horizontal blur of the shadow map
		static const YumeString blurOffsetParam("BlurOffsets");
		gYume->pRHI->SetShaderParameter(blurOffsetParam,Vector2(shadowSoftness_ / shadowMap->GetWidth(),0.0f));

		gYume->pRHI->SetTexture(TU_DIFFUSE,shadowMap);
		view->DrawFullscreenQuad(false);

		// Vertical blur
		gYume->pRHI->SetRenderTarget(0,shadowMap);
		gYume->pRHI->SetViewport(IntRect(0,0,shadowMap->GetWidth(),shadowMap->GetHeight()));

		gYume->pRHI->SetShaderParameter(blurOffsetParam,Vector2(0.0f,shadowSoftness_ / shadowMap->GetHeight()));

		gYume->pRHI->SetTexture(TU_DIFFUSE,tmpBuffer);
		view->DrawFullscreenQuad(false);
	}

}

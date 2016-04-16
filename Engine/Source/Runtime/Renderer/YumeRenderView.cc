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
#include "YumeRenderView.h"
#include "YumeCamera.h"
#include "YumeAuxRenderer.h"
#include "YumeRHI.h"
#include "YumeMaterial.h"
#include "YumeOcclusion.h"
#include "Scene/YumeOctree.h"
#include "YumeRenderer.h"
#include "YumeRenderPipeline.h"
#include "YumeShaderVariation.h"
#include "YumeRenderPass.h"
#include "YumeVertexBuffer.h"
#include "YumeTexture2D.h"
#include "YumeTexture3D.h"
#include "YumeTextureCube.h"
#include "YumeGeometry.h"
#include "YumeResourceManager.h"
#include "Core/YumeIO.h"

#include "Logging/logging.h"
#include "Engine/YumeEngine.h"
#include "Scene/YumeScene.h"
#include "Scene/YumeSceneNode.h"
#include "Scene/YumeSceneComponent.h"

#include "Core/YumeWorkQueue.h"

#include "Core/YumeSortAlgorithms.h"


namespace YumeEngine
{

	static const Vector3* directions[] =
	{
		&Vector3::RIGHT,
		&Vector3::LEFT,
		&Vector3::UP,
		&Vector3::DOWN,
		&Vector3::FORWARD,
		&Vector3::BACK
	};


	/// %Frustum octree query for shadowcasters.
	class ShadowCasterOctreeQuery : public FrustumOctreeQuery
	{
	public:
		/// Construct with frustum and query parameters.
		ShadowCasterOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Frustum& frustum,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			FrustumOctreeQuery(result,frustum,drawableFlags,viewMask)
		{
		}

		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
		{
			while(start != end)
			{
				YumeDrawable* drawable = *start++;

				if(drawable->GetCastShadows() && (drawable->GetDrawableFlags() & drawableFlags_) &&
					(drawable->GetViewMask() & viewMask_))
				{
					if(inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
						result_.push_back(drawable);
				}
			}
		}
	};

	/// %Frustum octree query for zones and occluders.
	class ZoneOccluderOctreeQuery : public FrustumOctreeQuery
	{
	public:
		/// Construct with frustum and query parameters.
		ZoneOccluderOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Frustum& frustum,unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK):
			FrustumOctreeQuery(result,frustum,drawableFlags,viewMask)
		{
		}

		/// Intersection test for drawables.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
		{
			while(start != end)
			{
				YumeDrawable* drawable = *start++;
				unsigned char flags = drawable->GetDrawableFlags();

				if((flags == DRAWABLE_ZONE || (flags == DRAWABLE_GEOMETRY && drawable->IsOccluder())) &&
					(drawable->GetViewMask() & viewMask_))
				{
					if(inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
						result_.push_back(drawable);
				}
			}
		}
	};

	/// %Frustum octree query with occlusion.
	class OccludedFrustumOctreeQuery : public FrustumOctreeQuery
	{
	public:
		/// Construct with frustum, occlusion buffer and query parameters.
		OccludedFrustumOctreeQuery(YumePodVector<YumeDrawable*>::type& result,const Frustum& frustum,OcclusionBuffer* buffer,
			unsigned char drawableFlags = DRAWABLE_ANY,unsigned viewMask = DEFAULT_VIEWMASK):
			FrustumOctreeQuery(result,frustum,drawableFlags,viewMask),
			buffer_(buffer)
		{
		}

		/// Intersection test for an octant.
		virtual Intersection TestOctant(const BoundingBox& box,bool inside)
		{
			if(inside)
				return buffer_->IsVisible(box) ? INSIDE : OUTSIDE;
			else
			{
				Intersection result = frustum_.IsInside(box);
				if(result != OUTSIDE && !buffer_->IsVisible(box))
					result = OUTSIDE;
				return result;
			}
		}

		/// Intersection test for drawables. Note: drawable occlusion is performed later in worker threads.
		virtual void TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
		{
			while(start != end)
			{
				YumeDrawable* drawable = *start++;

				if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
				{
					if(inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
						result_.push_back(drawable);
				}
			}
		}

		/// Occlusion buffer.
		OcclusionBuffer* buffer_;
	};

	void CheckVisibilityWork(const WorkItem* item,unsigned threadIndex)
	{
		YumeRenderView* view = reinterpret_cast<YumeRenderView*>(item->aux_);
		YumeDrawable** start = reinterpret_cast<YumeDrawable**>(item->start_);
		YumeDrawable** end = reinterpret_cast<YumeDrawable**>(item->end_);
		OcclusionBuffer* buffer = view->occlusionBuffer_;
		const Matrix3x4& viewMatrix = view->cullCamera_->GetView();
		Vector3 viewZ = Vector3(viewMatrix.m20_,viewMatrix.m21_,viewMatrix.m22_);
		Vector3 absViewZ = viewZ.Abs();
		unsigned cameraViewMask = view->cullCamera_->GetViewMask();
		bool cameraZoneOverride = view->cameraZoneOverride_;
		PerThreadSceneResult& result = view->sceneResults_[threadIndex];

		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if(!buffer || !drawable->IsOccludee() || buffer->IsVisible(drawable->GetWorldBoundingBox()))
			{
				drawable->UpdateBatches(view->frame_);
				// If draw distance non-zero, update and check it
				float maxDistance = drawable->GetDrawDistance();
				if(maxDistance > 0.0f)
				{
					if(drawable->GetDistance() > maxDistance)
						continue;
				}

				drawable->MarkInView(view->frame_);

				// For geometries, find zone, clear lights and calculate view space Z range
				if(drawable->GetDrawableFlags() & DRAWABLE_GEOMETRY)
				{
					YumeRendererEnvironment* drawableZone = drawable->GetZone();
					if(!cameraZoneOverride &&
						(drawable->IsZoneDirty() || !drawableZone || (drawableZone->GetViewMask() & cameraViewMask) == 0))
						view->FindZone(drawable);

					const BoundingBox& geomBox = drawable->GetWorldBoundingBox();
					Vector3 center = geomBox.Center();
					Vector3 edge = geomBox.Size() * 0.5f;

					// Do not add "infinite" objects like skybox to prevent shadow map focusing behaving erroneously
					if(edge.LengthSquared() < M_LARGE_VALUE * M_LARGE_VALUE)
					{
						float viewCenterZ = viewZ.DotProduct(center) + viewMatrix.m23_;
						float viewEdgeZ = absViewZ.DotProduct(edge);
						float minZ = viewCenterZ - viewEdgeZ;
						float maxZ = viewCenterZ + viewEdgeZ;
						drawable->SetMinMaxZ(viewCenterZ - viewEdgeZ,viewCenterZ + viewEdgeZ);
						result.minZ_ = Min(result.minZ_,minZ);
						result.maxZ_ = Max(result.maxZ_,maxZ);
					}
					else
						drawable->SetMinMaxZ(M_LARGE_VALUE,M_LARGE_VALUE);

					result.geometries_.push_back(drawable);
				}
				else if(drawable->GetDrawableFlags() & DRAWABLE_LIGHT)
				{
					YumeLight* light = static_cast<YumeLight*>(drawable);
					// Skip lights with zero brightness or black color
					if(!light->GetEffectiveColor().Equals(YumeColor::BLACK))
						result.lights_.push_back(light);
				}
			}
		}
	}

	void ProcessLightWork(const WorkItem* item,unsigned threadIndex)
	{
		YumeRenderView* view = reinterpret_cast<YumeRenderView*>(item->aux_);
		LightQueryResult* query = reinterpret_cast<LightQueryResult*>(item->start_);

		view->ProcessLight(*query,threadIndex);
	}

	void UpdateDrawableGeometriesWork(const WorkItem* item,unsigned threadIndex)
	{
		const FrameInfo& frame = *(reinterpret_cast<FrameInfo*>(item->aux_));
		YumeDrawable** start = reinterpret_cast<YumeDrawable**>(item->start_);
		YumeDrawable** end = reinterpret_cast<YumeDrawable**>(item->end_);

		while(start != end)
		{
			YumeDrawable* drawable = *start++;
			// We may leave null pointer holes in the queue if a drawable is found out to require a main thread update
			if(drawable)
				drawable->UpdateGeometry(frame);
		}
	}

	void SortBatchQueueFrontToBackWork(const WorkItem* item,unsigned threadIndex)
	{
		BatchQueue* queue = reinterpret_cast<BatchQueue*>(item->start_);

		queue->SortFrontToBack();
	}

	void SortBatchQueueBackToFrontWork(const WorkItem* item,unsigned threadIndex)
	{
		BatchQueue* queue = reinterpret_cast<BatchQueue*>(item->start_);

		queue->SortBackToFront();
	}

	void SortLightQueueWork(const WorkItem* item,unsigned threadIndex)
	{
		LightBatchQueue* start = reinterpret_cast<LightBatchQueue*>(item->start_);
		start->litBaseBatches_.SortFrontToBack();
		start->litBatches_.SortFrontToBack();
	}

	void SortShadowQueueWork(const WorkItem* item,unsigned threadIndex)
	{
		LightBatchQueue* start = reinterpret_cast<LightBatchQueue*>(item->start_);
		for(unsigned i = 0; i < start->shadowSplits_.size(); ++i)
			start->shadowSplits_[i].shadowBatches_.SortFrontToBack();
	}


	YumeRenderView::YumeRenderView():
		graphics_(gYume->pRHI),
		renderer_(gYume->pRenderer),
		scene_(0),
		octree_(0),
		cullCamera_(0),
		camera_(0),
		sourceView_(0),
		cameraZone_(0),
		farClipZone_(0),
		occlusionBuffer_(0),
		renderTarget_(0),
		substituteRenderTarget_(0)
	{
		// Create octree query and scene results vector for each thread
		unsigned numThreads = gYume->pWorkSystem->GetNumThreads() + 1; // Worker threads + main thread
		tempDrawables_.resize(numThreads);
		sceneResults_.resize(numThreads);
		frame_.camera_ = 0;
	}

	YumeRenderView::~YumeRenderView()
	{
	}

	bool YumeRenderView::Define(YumeRenderable* renderTarget,YumeViewport* viewport)
	{
		sourceView_ = 0;
		renderPath_ = viewport->GetRenderPath();
		if(!renderPath_)
			return false;

		renderTarget_ = renderTarget;
		drawDebug_ = viewport->GetDrawDebug();

		// Validate the rect and calculate size. If zero rect, use whole rendertarget size
		int rtWidth = renderTarget ? renderTarget->GetWidth() : graphics_->GetWidth();
		int rtHeight = renderTarget ? renderTarget->GetHeight() : graphics_->GetHeight();
		const IntRect& rect = viewport->GetRect();

		if(rect != IntRect::ZERO)
		{
			viewRect_.left_ = Clamp(rect.left_,0,rtWidth - 1);
			viewRect_.top_ = Clamp(rect.top_,0,rtHeight - 1);
			viewRect_.right_ = Clamp(rect.right_,viewRect_.left_ + 1,rtWidth);
			viewRect_.bottom_ = Clamp(rect.bottom_,viewRect_.top_ + 1,rtHeight);
		}
		else
			viewRect_ = IntRect(0,0,rtWidth,rtHeight);

		viewSize_ = viewRect_.Size();
		rtSize_ = IntVector2(rtWidth,rtHeight);

		if(gYume->pEngine->GetRendererName() == "OpenGL")
		{
			if(renderTarget_)
			{
				viewRect_.bottom_ = rtHeight - viewRect_.top_;
				viewRect_.top_ = viewRect_.bottom_ - viewSize_.y_;
			}
		}
		scene_ = viewport->GetScene();
		cullCamera_ = viewport->GetCullCamera();
		camera_ = viewport->GetCamera();
		if(!cullCamera_)
			cullCamera_ = camera_;
		else
		{
			// If view specifies a culling camera (view preparation sharing), check if already prepared
			sourceView_ = renderer_->GetPreparedView(cullCamera_);
			if(sourceView_ && sourceView_->scene_ == scene_ && sourceView_->renderPath_ == renderPath_)
			{
				// Copy properties needed later in rendering
				deferred_ = sourceView_->deferred_;
				deferredAmbient_ = sourceView_->deferredAmbient_;
				useLitBase_ = sourceView_->useLitBase_;
				hasScenePasses_ = sourceView_->hasScenePasses_;
				noStencil_ = sourceView_->noStencil_;
				lightVolumeCommand_ = sourceView_->lightVolumeCommand_;
				octree_ = sourceView_->octree_;
				return true;
			}
			else
			{
				// Mismatch in scene or renderpath, fall back to unique view preparation
				sourceView_ = 0;
			}
		}

		// Set default passes
		gBufferPassIndex_ = M_MAX_UNSIGNED;
		basePassIndex_ = YumeRenderTechnique::GetPassIndex("base");
		alphaPassIndex_ = YumeRenderTechnique::GetPassIndex("alpha");
		lightPassIndex_ = YumeRenderTechnique::GetPassIndex("light");
		litBasePassIndex_ = YumeRenderTechnique::GetPassIndex("litbase");
		litAlphaPassIndex_ = YumeRenderTechnique::GetPassIndex("litalpha");

		deferred_ = false;
		deferredAmbient_ = false;
		useLitBase_ = false;
		hasScenePasses_ = false;
		noStencil_ = false;
		lightVolumeCommand_ = 0;

		scenePasses_.clear();
		geometriesUpdated_ = false;

		if(gYume->pEngine->GetRendererName() == "OpenGL")
		{
			for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
			{
				const RenderCommand& command = renderPath_->commands_[i];
				if(!command.enabled_)
					continue;
				if(command.depthStencilName_.length())
				{
					// Using a readable depth texture will disable light stencil optimizations on OpenGL, as for compatibility reasons
					// we are using a depth format without stencil channel
					noStencil_ = true;
					break;
				}
			}
		}

		// Make sure that all necessary batch queues exist
		for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
		{
			RenderCommand& command = renderPath_->commands_[i];
			if(!command.enabled_)
				continue;

			if(command.type_ == CMD_SCENEPASS)
			{
				hasScenePasses_ = true;

				ScenePassInfo info;
				info.passIndex_ = command.passIndex_ = YumeRenderTechnique::GetPassIndex(command.pass_);
				info.allowInstancing_ = command.sortMode_ != SORT_BACKTOFRONT;
				info.markToStencil_ = !noStencil_ && command.markToStencil_;
				info.vertexLights_ = command.vertexLights_;

				// Check scenepass metadata for defining custom passes which interact with lighting
				if(!command.metadata_.empty())
				{
					if(command.metadata_ == "gbuffer")
						gBufferPassIndex_ = command.passIndex_;
					else if(command.metadata_ == "base" && command.pass_ != "base")
					{
						basePassIndex_ = command.passIndex_;
						litBasePassIndex_ = YumeRenderTechnique::GetPassIndex("lit" + command.pass_);
					}
					else if(command.metadata_ == "alpha" && command.pass_ != "alpha")
					{
						alphaPassIndex_ = command.passIndex_;
						litAlphaPassIndex_ = YumeRenderTechnique::GetPassIndex("lit" + command.pass_);
					}
				}

				YumeMap<unsigned,BatchQueue>::iterator j = batchQueues_.find(info.passIndex_);
				if(j == batchQueues_.end())
				{
					j= batchQueues_.insert(MakePair(info.passIndex_,BatchQueue()));
				}
				info.batchQueue_ = &j->second;

				scenePasses_.push_back(info);
			}
			// Allow a custom forward light pass
			else if(command.type_ == CMD_FORWARDLIGHTS && !command.pass_.empty())
				lightPassIndex_ = command.passIndex_ = YumeRenderTechnique::GetPassIndex(command.pass_);
		}

		octree_ = 0;
		// Get default zone first in case we do not have zones defined
		cameraZone_ = farClipZone_ = renderer_->GetDefaultZone();

		if(hasScenePasses_)
		{
			if(!scene_ || !cullCamera_ || !cullCamera_->IsEnabledEffective())
				return false;

			// If scene is loading scene content asynchronously, it is incomplete and should not be rendered
			/*if(scene_->IsAsyncLoading() && scene_->GetAsyncLoadMode() > LOAD_RESOURCES_ONLY)
				return false;*/

			octree_ = scene_->GetComponent<Octree>();
			if(!octree_)
				return false;

			// Do not accept view if camera projection is illegal
			// (there is a possibility of crash if occlusion is used and it can not clip properly)
			if(!cullCamera_->IsProjectionValid())
				return false;
		}

		// Go through commands to check for deferred rendering and other flags
		for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
		{
			const RenderCommand& command = renderPath_->commands_[i];
			if(!command.enabled_)
				continue;

			// Check if ambient pass and G-buffer rendering happens at the same time
			if(command.type_ == CMD_SCENEPASS && command.outputs_.size() > 1)
			{
				if(CheckViewportWrite(command))
					deferredAmbient_ = true;
			}
			else if(command.type_ == CMD_LIGHTVOLUMES)
			{
				lightVolumeCommand_ = &command;
				deferred_ = true;
			}
			else if(command.type_ == CMD_FORWARDLIGHTS)
				useLitBase_ = command.useLitBase_;
		}

		drawShadows_ = renderer_->GetDrawShadows();
		materialQuality_ = renderer_->GetMaterialQuality();
		maxOccluderTriangles_ = renderer_->GetMaxOccluderTriangles();
		minInstances_ = renderer_->GetMinInstances();

		// Set possible quality overrides from the camera
		// Note that the culling camera is used here (its settings are authoritative) while the render camera
		// will be just used for the final view & projection matrices
		unsigned viewOverrideFlags = cullCamera_ ? cullCamera_->GetViewOverrideFlags() : VO_NONE;
		if(viewOverrideFlags & VO_LOW_MATERIAL_QUALITY)
			materialQuality_ = QUALITY_LOW;
		if(viewOverrideFlags & VO_DISABLE_SHADOWS)
			drawShadows_ = false;
		if(viewOverrideFlags & VO_DISABLE_OCCLUSION)
			maxOccluderTriangles_ = 0;

		// Occlusion buffer has constant width. If resulting height would be too large due to aspect ratio, disable occlusion
		if(viewSize_.y_ > viewSize_.x_ * 4)
			maxOccluderTriangles_ = 0;

		return true;
	}


	void YumeRenderView::Update(const FrameInfo& frame)
	{
		// No need to update if using another prepared view
		if(sourceView_)
			return;

		frame_.camera_ = cullCamera_;
		frame_.timeStep_ = frame.timeStep_;
		frame_.frameNumber_ = frame.frameNumber_;
		frame_.viewSize_ = viewSize_;

		//Update listeners
		renderer_->FireEvent(R_BEGINVIEWUPDATE,this,renderTarget_,(renderTarget_ ? renderTarget_->GetParentTexture() : 0),scene_,cullCamera_);


		int maxSortedInstances = renderer_->GetMaxSortedInstances();

		// Clear buffers, geometry, light, occluder & batch list
		renderTargets_.clear();
		geometries_.clear();
		lights_.clear();
		zones_.clear();
		occluders_.clear();
		activeOccluders_ = 0;
		vertexLightQueues_.clear();
		for(YumeMap<unsigned,BatchQueue>::iterator i = batchQueues_.begin(); i != batchQueues_.end(); ++i)
			i->second.Clear(maxSortedInstances);

		if(hasScenePasses_ && (!cullCamera_ || !octree_))
		{
			renderer_->FireEvent(R_ENDVIEWUPDATE,this,renderTarget_,(renderTarget_ ? renderTarget_->GetParentTexture() : 0),scene_,cullCamera_);
			return;
		}

		// Set automatic aspect ratio if required
		if(cullCamera_ && cullCamera_->GetAutoAspectRatio())
			cullCamera_->SetAspectRatioInternal((float)frame_.viewSize_.x_ / (float)frame_.viewSize_.y_);

		GetDrawables();
		GetBatches();
		renderer_->StorePreparedView(this,cullCamera_);

		renderer_->FireEvent(R_ENDVIEWUPDATE,this,renderTarget_,(renderTarget_ ? renderTarget_->GetParentTexture() : 0),scene_,cullCamera_);
	}


	void YumeRenderView::Render()
	{
		if(hasScenePasses_ && (!octree_ || !camera_))
			return;

		UpdateGeometries();

		// Allocate screen buffers as necessary
		AllocateScreenBuffers();

		// Forget parameter sources from the previous view
		graphics_->ClearParameterSources();

		if(renderer_->GetDynamicInstancing() && graphics_->GetInstancingSupport())
			PrepareInstancingBuffer();

		// It is possible, though not recommended, that the same camera is used for multiple main views. Set automatic aspect ratio
		// to ensure correct projection will be used
		if(camera_)
		{
			if(camera_->GetAutoAspectRatio())
				camera_->SetAspectRatioInternal((float)(viewSize_.x_) / (float)(viewSize_.y_));
		}

		// Bind the face selection and indirection cube maps for point light shadows
#ifndef GL_ES_VERSION_2_0
		if(renderer_->GetDrawShadows())
		{
			graphics_->SetTexture(TU_FACESELECT,renderer_->GetFaceSelectCubeMap());
			graphics_->SetTexture(TU_INDIRECTION,renderer_->GetIndirectionCubeMap());
		}
#endif

		if(renderTarget_)
		{
			// On OpenGL, flip the projection if rendering to a texture so that the texture can be addressed in the same way
			// as a render texture produced on Direct3D9
			if(renderer_->GetName() == "OpenGL")
			{
				if(camera_)
					camera_->SetFlipVertical(true);
			}

		}

		// Render
		ExecuteRenderPathCommands();

		// Reset state after commands
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetClipPlane(false);
		graphics_->SetColorWrite(true);
		graphics_->SetDepthBias(0.0f,0.0f);
		graphics_->SetScissorTest(false);
		graphics_->SetStencilTest(false);


		if(deferred_ && renderer_->GetGBufferDebugRendering())
		{
			YumeDebugRenderer* debug = octree_->GetComponent<YumeDebugRenderer>();

			/*if(debug)
			{
				debug->RenderInternalTexture(IntRect(0,0,400,400),FindNamedTexture("albedo",true));
				debug->RenderInternalTexture(IntRect(0,0,300,0),FindNamedTexture("normal",true));
				debug->RenderInternalTexture(IntRect(0,0,600,0),FindNamedTexture("depth",true));
			}*/

		}

		// Draw the associated debug geometry now if enabled
		if(drawDebug_ && octree_ && camera_)
		{
			YumeDebugRenderer* debug = octree_->GetComponent<YumeDebugRenderer>();
			if(debug && debug->IsEnabledEffective() && debug->HasContent())
			{
				// If used resolve from backbuffer, blit first to the backbuffer to ensure correct depth buffer on OpenGL
				// Otherwise use the last rendertarget and blit after debug geometry
				if(usedResolve_ && currentRenderTarget_ != renderTarget_)
				{
					BlitFramebuffer(currentRenderTarget_->GetParentTexture(),renderTarget_,false);
					currentRenderTarget_ = renderTarget_;
				}

				graphics_->SetRenderTarget(0,currentRenderTarget_);
				for(unsigned i = 1; i < MAX_RENDERTARGETS; ++i)
					graphics_->SetRenderTarget(i,(YumeRenderable*)0);
				graphics_->SetDepthStencil(GetDepthStencil(currentRenderTarget_));
				IntVector2 rtSizeNow = graphics_->GetRenderTargetDimensions();
				IntRect viewport = (currentRenderTarget_ == renderTarget_) ? viewRect_ : IntRect(0,0,rtSizeNow.x_,
					rtSizeNow.y_);
				graphics_->SetViewport(viewport);

				debug->SetView(camera_);
				debug->Render();
			}
		}

		if(renderer_->GetName() == "OpenGL")
		{
			if(camera_)
				camera_->SetFlipVertical(false);
		}

		// Run framebuffer blitting if necessary. If scene was resolved from backbuffer, do not touch depth
		// (backbuffer should contain proper depth already)
		if(currentRenderTarget_ != renderTarget_)
			BlitFramebuffer(currentRenderTarget_->GetParentTexture(),renderTarget_,!usedResolve_);
	}


	YumeRHI* YumeRenderView::GetRHI() const
	{
		return graphics_;
	}

	YumeRenderer* YumeRenderView::GetRenderer() const
	{
		return renderer_;
	}

	YumeRenderView* YumeRenderView::GetSourceView() const
	{
		return sourceView_;
	}

	void YumeRenderView::SetGlobalShaderParameters()
	{
		graphics_->SetShaderParameter(VSP_DELTATIME,frame_.timeStep_);
		graphics_->SetShaderParameter(PSP_DELTATIME,frame_.timeStep_);

		if(scene_)
		{
			float elapsedTime = scene_->GetElapsedTime();
			graphics_->SetShaderParameter(VSP_ELAPSEDTIME,elapsedTime);
			graphics_->SetShaderParameter(PSP_ELAPSEDTIME,elapsedTime);
		}
	}


	void YumeRenderView::SetCameraShaderParameters(YumeCamera* camera,bool setProjection)
	{
		if(!camera)
			return;


		Matrix3x4 cameraEffectiveTransform = camera->GetEffectiveWorldTransform();

		graphics_->SetShaderParameter(VSP_CAMERAPOS,cameraEffectiveTransform.Translation());
		graphics_->SetShaderParameter(VSP_CAMERAROT,cameraEffectiveTransform.RotationMatrix());
		graphics_->SetShaderParameter(PSP_CAMERAPOS,cameraEffectiveTransform.Translation());

		float nearClip = camera->GetNearClip();
		float farClip = camera->GetFarClip();
		graphics_->SetShaderParameter(VSP_NEARCLIP,nearClip);
		graphics_->SetShaderParameter(VSP_FARCLIP,farClip);
		graphics_->SetShaderParameter(PSP_NEARCLIP,nearClip);
		graphics_->SetShaderParameter(PSP_FARCLIP,farClip);



		Vector4 depthMode = Vector4::ZERO;
		if(camera->IsOrthographic())
		{
			depthMode.x_ = 1.0f;
			if(renderer_->GetName() == "OpenGL")
			{
				depthMode.z_ = 0.5f;
				depthMode.w_ = 0.5f;
			}
			else
			{
				depthMode.z_ = 1.0f;
			}
		}
		else
			depthMode.w_ = 1.0f / camera->GetFarClip();

		graphics_->SetShaderParameter(VSP_DEPTHMODE,depthMode);

		Vector4 depthReconstruct
			(farClip / (farClip - nearClip),-nearClip / (farClip - nearClip),camera->IsOrthographic() ? 1.0f : 0.0f,
			camera->IsOrthographic() ? 0.0f : 1.0f);
		graphics_->SetShaderParameter(PSP_DEPTHRECONSTRUCT,depthReconstruct);

		Vector3 nearVector,farVector;
		camera->GetFrustumSize(nearVector,farVector);
		graphics_->SetShaderParameter(VSP_FRUSTUMSIZE,farVector);


		if(setProjection)
		{
			Matrix4 projection = camera->GetProjection();
			if(renderer_->GetName() == "OpenGL")
			{
				// Add constant depth bias manually to the projection matrix due to glPolygonOffset() inconsistency
				float constantBias = 2.0f * graphics_->GetDepthConstantBias();
				projection.m22_ += projection.m32_ * constantBias;
				projection.m23_ += projection.m33_ * constantBias;
			}

			graphics_->SetShaderParameter(VSP_VIEWINV,camera->GetEffectiveWorldTransform());
			graphics_->SetShaderParameter(VSP_VIEW,camera->GetView());
			graphics_->SetShaderParameter(VSP_VIEWPROJ,projection * camera->GetView());


		}
	}

	void YumeRenderView::SetGBufferShaderParameters(const IntVector2& texSize,const IntRect& viewRect)
	{
		float texWidth = (float)texSize.x_;
		float texHeight = (float)texSize.y_;
		float widthRange = 0.5f * viewRect.Width() / texWidth;
		float heightRange = 0.5f * viewRect.Height() / texHeight;

		Vector4 bufferUVOffset = Vector4::ZERO;
		if(renderer_->GetName() == "OpenGL")
		{
			bufferUVOffset = Vector4(((float)viewRect.left_) / texWidth + widthRange,
				1.0f - (((float)viewRect.top_) / texHeight + heightRange),widthRange,heightRange);
		}
		else
		{
			const Vector2& pixelUVOffset = graphics_->GetPixelUVOffset();
			bufferUVOffset = Vector4((pixelUVOffset.x_ + (float)viewRect.left_) / texWidth + widthRange,
				(pixelUVOffset.y_ + (float)viewRect.top_) / texHeight + heightRange,widthRange,heightRange);
		}
		graphics_->SetShaderParameter(VSP_GBUFFEROFFSETS,bufferUVOffset);

		float invSizeX = 1.0f / texWidth;
		float invSizeY = 1.0f / texHeight;
		graphics_->SetShaderParameter(PSP_GBUFFERINVSIZE,Vector2(invSizeX,invSizeY));

	}



	void YumeRenderView::GetDrawables()
	{
		if(!octree_ || !cullCamera_)
			return;

		YumeWorkQueue* queue = gYume->pWorkSystem;
		YumePodVector<YumeDrawable*>::type& tempDrawables = tempDrawables_[0];

		// Get zones and occluders first
		{
			ZoneOccluderOctreeQuery
				query(tempDrawables,cullCamera_->GetFrustum(),DRAWABLE_GEOMETRY | DRAWABLE_ZONE,cullCamera_->GetViewMask());
			octree_->GetDrawables(query);
		}

		highestZonePriority_ = M_MIN_INT;
		int bestPriority = M_MIN_INT;
		YumeSceneNode* cameraNode = cullCamera_->GetNode();
		Vector3 cameraPos = cameraNode->GetWorldPosition();

		for(YumeVector<YumeDrawable*>::const_iterator i = tempDrawables.begin(); i != tempDrawables.end(); ++i)
		{
			YumeDrawable* drawable = *i;
			unsigned char flags = drawable->GetDrawableFlags();

			if(flags & DRAWABLE_ZONE)
			{
				YumeRendererEnvironment* zone = static_cast<YumeRendererEnvironment*>(drawable);
				zones_.push_back(zone);
				int priority = zone->GetPriority();
				if(priority > highestZonePriority_)
					highestZonePriority_ = priority;
				if(priority > bestPriority && zone->IsInside(cameraPos))
				{
					cameraZone_ = zone;
					bestPriority = priority;
				}
			}
			else
				occluders_.push_back(drawable);
		}

		// Determine the zone at far clip distance. If not found, or camera zone has override mode, use camera zone
		cameraZoneOverride_ = cameraZone_->GetOverride();
		if(!cameraZoneOverride_)
		{
			Vector3 farClipPos = cameraPos + cameraNode->GetWorldDirection() * Vector3(0.0f,0.0f,cullCamera_->GetFarClip());
			bestPriority = M_MIN_INT;

			for(YumeVector<YumeRendererEnvironment*>::iterator i = zones_.begin(); i != zones_.end(); ++i)
			{
				int priority = (*i)->GetPriority();
				if(priority > bestPriority && (*i)->IsInside(farClipPos))
				{
					farClipZone_ = *i;
					bestPriority = priority;
				}
			}
		}
		if(farClipZone_ == renderer_->GetDefaultZone())
			farClipZone_ = cameraZone_;

		// If occlusion in use, get & render the occluders
		occlusionBuffer_ = 0;
		if(maxOccluderTriangles_ > 0)
		{
			UpdateOccluders(occluders_,cullCamera_);
			if(occluders_.size())
			{
				occlusionBuffer_ = renderer_->GetOcclusionBuffer(cullCamera_);
				DrawOccluders(occlusionBuffer_,occluders_);
			}
		}
		else
			occluders_.clear();

		// Get lights and geometries. Coarse occlusion for octants is used at this point
		if(occlusionBuffer_)
		{
			OccludedFrustumOctreeQuery query
				(tempDrawables,cullCamera_->GetFrustum(),occlusionBuffer_,DRAWABLE_GEOMETRY | DRAWABLE_LIGHT,cullCamera_->GetViewMask());
			octree_->GetDrawables(query);
		}
		else
		{
			FrustumOctreeQuery query(tempDrawables,cullCamera_->GetFrustum(),DRAWABLE_GEOMETRY | DRAWABLE_LIGHT,cullCamera_->GetViewMask());
			octree_->GetDrawables(query);
		}

		// Check drawable occlusion, find zones for moved drawables and collect geometries & lights in worker threads
	{
		for(unsigned i = 0; i < sceneResults_.size(); ++i)
		{
			PerThreadSceneResult& result = sceneResults_[i];

			result.geometries_.clear();
			result.lights_.clear();
			result.minZ_ = M_INFINITY;
			result.maxZ_ = 0.0f;
		}

		int numWorkItems = queue->GetNumThreads() + 1; // Worker threads + main thread
		int drawablesPerItem = tempDrawables.size() / numWorkItems;

		YumeVector<YumeDrawable*>::iterator start = tempDrawables.begin();
		// Create a work item for each thread
		for(int i = 0; i < numWorkItems; ++i)
		{
			SharedPtr<WorkItem> item = queue->GetFreeItem();
			item->priority_ = M_MAX_UNSIGNED;
			item->workFunction_ = CheckVisibilityWork;
			item->aux_ = this;

			YumeVector<YumeDrawable*>::iterator end = tempDrawables.end();
			if(i < numWorkItems - 1 && end - start > drawablesPerItem)
				end = start + drawablesPerItem;


			item->start_ = &(*start);
			item->end_ = &(*end);
			queue->AddWorkItem(item);

			start = end;
		}

		queue->Complete(M_MAX_UNSIGNED);
	}

	// Combine lights, geometries & scene Z range from the threads
	geometries_.clear();
	lights_.clear();
	minZ_ = M_INFINITY;
	maxZ_ = 0.0f;

	if(sceneResults_.size() > 1)
	{
		for(unsigned i = 0; i < sceneResults_.size(); ++i)
		{
			PerThreadSceneResult& result = sceneResults_[i];
			geometries_.push_back(result.geometries_);
			lights_.push_back(result.lights_);
			minZ_ = Min(minZ_,result.minZ_);
			maxZ_ = Max(maxZ_,result.maxZ_);
		}
	}
	else
	{
		// If just 1 thread, copy the results directly
		PerThreadSceneResult& result = sceneResults_[0];
		minZ_ = result.minZ_;
		maxZ_ = result.maxZ_;

		Swap(geometries_,result.geometries_);
		Swap(lights_,result.lights_);
	}

	if(minZ_ == M_INFINITY)
		minZ_ = 0.0f;

	// Sort the lights to brightest/closest first, and per-vertex lights first so that per-vertex base pass can be evaluated first
	for(unsigned i = 0; i < lights_.size(); ++i)
	{
		YumeLight* light = lights_[i];
		light->SetIntensitySortValue(cullCamera_->GetDistance(light->GetNode()->GetWorldPosition()));
		light->SetLightQueue(0);
	}

	Sort(lights_.begin(),lights_.end(),CompareLights);
	}


	void YumeRenderView::GetBatches()
	{
		if(!octree_ || !cullCamera_)
			return;

		nonThreadedGeometries_.clear();
		threadedGeometries_.clear();

		ProcessLights();
		GetLightBatches();
		GetBaseBatches();
	}


	void YumeRenderView::ProcessLights()
	{
		// Process lit geometries and shadow casters for each light
		YumeWorkQueue* queue = gYume->pWorkSystem;
		lightQueryResults_.resize(lights_.size());

		for(unsigned i = 0; i < lightQueryResults_.size(); ++i)
		{
			SharedPtr<WorkItem> item = queue->GetFreeItem();
			item->priority_ = M_MAX_UNSIGNED;
			item->workFunction_ = ProcessLightWork;
			item->aux_ = this;

			LightQueryResult& query = lightQueryResults_[i];
			query.light_ = lights_[i];

			item->start_ = &query;
			queue->AddWorkItem(item);
		}

		// Ensure all lights have been processed before proceeding
		queue->Complete(M_MAX_UNSIGNED);
	}

	void YumeRenderView::GetLightBatches()
	{
		BatchQueue* alphaQueue = batchQueues_.find(alphaPassIndex_) != batchQueues_.end() ? &batchQueues_[alphaPassIndex_] : (BatchQueue*)0;

		// Build light queues and lit batches
		{

			// Preallocate light queues: per-pixel lights which have lit geometries
			unsigned numLightQueues = 0;
			unsigned usedLightQueues = 0;
			for(YumeVector<LightQueryResult>::const_iterator i = lightQueryResults_.begin(); i != lightQueryResults_.end(); ++i)
			{
				if(!i->light_->GetPerVertex() && i->litGeometries_.size())
					++numLightQueues;
			}

			lightQueues_.resize(numLightQueues);
			maxLightsDrawables_.clear();
			unsigned maxSortedInstances = (unsigned)renderer_->GetMaxSortedInstances();

			for(YumeVector<LightQueryResult>::iterator i = lightQueryResults_.begin(); i != lightQueryResults_.end(); ++i)
			{
				LightQueryResult& query = *i;

				// If light has no affected geometries, no need to process further
				if(query.litGeometries_.empty())
					continue;

				YumeLight* light = query.light_;

				// Per-pixel light
				if(!light->GetPerVertex())
				{
					unsigned shadowSplits = query.numSplits_;

					// Initialize light queue and store it to the light so that it can be found later
					LightBatchQueue& lightQueue = lightQueues_[usedLightQueues++];
					light->SetLightQueue(&lightQueue);
					lightQueue.light_ = light;
					lightQueue.negative_ = light->IsNegative();
					lightQueue.shadowMap_ = 0;
					lightQueue.litBaseBatches_.Clear(maxSortedInstances);
					lightQueue.litBatches_.Clear(maxSortedInstances);
					lightQueue.volumeBatches_.clear();

					// Allocate shadow map now
					if(shadowSplits > 0)
					{
						lightQueue.shadowMap_ = renderer_->GetShadowMap(light,cullCamera_,(unsigned)viewSize_.x_,(unsigned)viewSize_.y_);
						// If did not manage to get a shadow map, convert the light to unshadowed
						if(!lightQueue.shadowMap_)
							shadowSplits = 0;
					}

					// Setup shadow batch queues
					lightQueue.shadowSplits_.resize(shadowSplits);
					for(unsigned j = 0; j < shadowSplits; ++j)
					{
						ShadowBatchQueue& shadowQueue = lightQueue.shadowSplits_[j];
						YumeCamera* shadowCamera = query.shadowCameras_[j];
						shadowQueue.shadowCamera_ = shadowCamera;
						shadowQueue.nearSplit_ = query.shadowNearSplits_[j];
						shadowQueue.farSplit_ = query.shadowFarSplits_[j];
						shadowQueue.shadowBatches_.Clear(maxSortedInstances);

						// Setup the shadow split viewport and finalize shadow camera parameters
						shadowQueue.shadowViewport_ = GetShadowMapViewport(light,j,lightQueue.shadowMap_);
						FinalizeShadowCamera(shadowCamera,light,shadowQueue.shadowViewport_,query.shadowCasterBox_[j]);

						// Loop through shadow casters
						for(YumeVector<YumeDrawable*>::const_iterator k = query.shadowCasters_.begin() + query.shadowCasterBegin_[j];
							k < query.shadowCasters_.begin() + query.shadowCasterEnd_[j]; ++k)
						{
							YumeDrawable* drawable = *k;
							// If drawable is not in actual view frustum, mark it in view here and check its geometry update type
							if(!drawable->IsInView(frame_,true))
							{
								drawable->MarkInView(frame_.frameNumber_);
								UpdateGeometryType type = drawable->GetUpdateGeometryType();
								if(type == UPDATE_MAIN_THREAD)
									nonThreadedGeometries_.push_back(drawable);
								else if(type == UPDATE_WORKER_THREAD)
									threadedGeometries_.push_back(drawable);
							}

							YumeRendererEnvironment* zone = GetZone(drawable);
							const YumeVector<SourceBatch>::type& batches = drawable->GetBatches();

							for(unsigned l = 0; l < batches.size(); ++l)
							{
								const SourceBatch& srcBatch = batches[l];

								YumeRenderTechnique* tech = GetTechnique(drawable,srcBatch.material_);
								if(!srcBatch.geometry_ || !srcBatch.numWorldTransforms_ || !tech)
									continue;

								YumeRenderPass* pass = tech->GetSupportedPass(YumeRenderTechnique::shadowPassIndex);
								// Skip if material has no shadow pass
								if(!pass)
									continue;

								Batch destBatch(srcBatch);
								destBatch.pass_ = pass;
								destBatch.zone_ = zone;

								AddBatchToQueue(shadowQueue.shadowBatches_,destBatch,tech);
							}
						}
					}

					// Process lit geometries
					for(YumeVector<YumeDrawable*>::const_iterator j = query.litGeometries_.begin(); j != query.litGeometries_.end(); ++j)
					{
						YumeDrawable* drawable = *j;
						drawable->AddLight(light);

						// If drawable limits maximum lights, only record the light, and check maximum count / build batches later
						if(!drawable->GetMaxLights())
							GetLitBatches(drawable,lightQueue,alphaQueue);
						else
							maxLightsDrawables_.insert(drawable);
					}

					// In deferred modes, store the light volume batch now
					if(deferred_)
					{
						Batch volumeBatch;
						volumeBatch.geometry_ = renderer_->GetLightGeometry(light);
						volumeBatch.geometryType_ = GEOM_STATIC;
						volumeBatch.worldTransform_ = &light->GetVolumeTransform(cullCamera_);
						volumeBatch.numWorldTransforms_ = 1;
						volumeBatch.lightQueue_ = &lightQueue;
						volumeBatch.distance_ = light->GetDistance();
						volumeBatch.material_ = 0;
						volumeBatch.pass_ = 0;
						volumeBatch.zone_ = 0;
						renderer_->SetLightVolumeBatchShaders(volumeBatch,cullCamera_,lightVolumeCommand_->vertexShaderName_,
							lightVolumeCommand_->pixelShaderName_,lightVolumeCommand_->vertexShaderDefines_,
							lightVolumeCommand_->pixelShaderDefines_);
						lightQueue.volumeBatches_.push_back(volumeBatch);
					}
				}
				// Per-vertex light
				else
				{
					// Add the vertex light to lit drawables. It will be processed later during base pass batch generation
					for(YumeVector<YumeDrawable*>::const_iterator j = query.litGeometries_.begin(); j != query.litGeometries_.end(); ++j)
					{
						YumeDrawable* drawable = *j;
						drawable->AddVertexLight(light);
					}
				}
			}
		}

		// Process drawables with limited per-pixel light count
		if(maxLightsDrawables_.size())
		{
			for(YumeHashSet<YumeDrawable*>::iterator i = maxLightsDrawables_.begin(); i != maxLightsDrawables_.end(); ++i)
			{
				YumeDrawable* drawable = *i;
				drawable->LimitLights();
				const YumePodVector<YumeLight*>::type& lights = drawable->GetLights();

				for(unsigned i = 0; i < lights.size(); ++i)
				{
					YumeLight* light = lights[i];
					// Find the correct light queue again
					LightBatchQueue* queue = light->GetLightQueue();
					if(queue)
						GetLitBatches(drawable,*queue,alphaQueue);
				}
			}
		}
	}

	void YumeRenderView::GetBaseBatches()
	{
		for(YumeVector<YumeDrawable*>::const_iterator i = geometries_.begin(); i != geometries_.end(); ++i)
		{
			YumeDrawable* drawable = *i;
			UpdateGeometryType type = drawable->GetUpdateGeometryType();
			if(type == UPDATE_MAIN_THREAD)
				nonThreadedGeometries_.push_back(drawable);
			else if(type == UPDATE_WORKER_THREAD)
				threadedGeometries_.push_back(drawable);

			const YumeVector<SourceBatch>::type& batches = drawable->GetBatches();
			bool vertexLightsProcessed = false;

			for(unsigned j = 0; j < batches.size(); ++j)
			{
				const SourceBatch& srcBatch = batches[j];

				// Check here if the material refers to a rendertarget texture with camera(s) attached
				// Only check this for backbuffer views (null rendertarget)
				if(srcBatch.material_ && srcBatch.material_->GetAuxViewFrameNumber() != frame_.frameNumber_ && !renderTarget_)
					CheckMaterialForAuxView(srcBatch.material_);

				YumeRenderTechnique* tech = GetTechnique(drawable,srcBatch.material_);
				if(!srcBatch.geometry_ || !srcBatch.numWorldTransforms_ || !tech)
					continue;

				// Check each of the scene passes
				for(unsigned k = 0; k < scenePasses_.size(); ++k)
				{
					ScenePassInfo& info = scenePasses_[k];
					// Skip forward base pass if the corresponding litbase pass already exists
					if(info.passIndex_ == basePassIndex_ && j < 32 && drawable->HasBasePass(j))
						continue;

					YumeRenderPass* pass = tech->GetSupportedPass(info.passIndex_);
					if(!pass)
						continue;

					Batch destBatch(srcBatch);
					destBatch.pass_ = pass;
					destBatch.zone_ = GetZone(drawable);
					destBatch.isBase_ = true;
					destBatch.lightMask_ = (unsigned char)GetLightMask(drawable);

					if(info.vertexLights_)
					{
						const YumePodVector<YumeLight*>::type& drawableVertexLights = drawable->GetVertexLights();
						if(drawableVertexLights.size() && !vertexLightsProcessed)
						{
							// Limit vertex lights. If this is a deferred opaque batch, remove converted per-pixel lights,
							// as they will be rendered as light volumes in any case, and drawing them also as vertex lights
							// would result in double lighting
							drawable->LimitVertexLights(deferred_ && destBatch.pass_->GetBlendMode() == BLEND_REPLACE);
							vertexLightsProcessed = true;
						}

						if(drawableVertexLights.size())
						{
							// Find a vertex light queue. If not found, create new
							unsigned long long hash = GetVertexLightQueueHash(drawableVertexLights);
							YumeMap<unsigned long long,LightBatchQueue>::iterator i = vertexLightQueues_.find(hash);
							if(i == vertexLightQueues_.end())
							{
								i = vertexLightQueues_.insert(MakePair(hash,LightBatchQueue()));
								i->second.light_ = 0;
								i->second.shadowMap_ = 0;
								i->second.vertexLights_ = drawableVertexLights;
							}

							destBatch.lightQueue_ = &(i->second);
						}
					}
					else
						destBatch.lightQueue_ = 0;

					bool allowInstancing = info.allowInstancing_;
					if(allowInstancing && info.markToStencil_ && destBatch.lightMask_ != (destBatch.zone_->GetLightMask() & 0xff))
						allowInstancing = false;

					AddBatchToQueue(*info.batchQueue_,destBatch,tech,allowInstancing);
				}
			}
		}
	}

	void YumeRenderView::UpdateGeometries()
	{
		// Update geometries in the source view if necessary (prepare order may differ from render order)
		if(sourceView_ && !sourceView_->geometriesUpdated_)
		{
			sourceView_->UpdateGeometries();
			return;
		}

		YumeWorkQueue* queue = gYume->pWorkSystem;

		// Sort batches
		{
			for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
			{
				const RenderCommand& command = renderPath_->commands_[i];
				if(!IsNecessary(command))
					continue;

				if(command.type_ == CMD_SCENEPASS)
				{
					SharedPtr<WorkItem> item = queue->GetFreeItem();
					item->priority_ = M_MAX_UNSIGNED;
					item->workFunction_ =
						command.sortMode_ == SORT_FRONTTOBACK ? SortBatchQueueFrontToBackWork : SortBatchQueueBackToFrontWork;
					item->start_ = &batchQueues_[command.passIndex_];
					queue->AddWorkItem(item);
				}
			}

			for(YumeVector<LightBatchQueue>::iterator i = lightQueues_.begin(); i != lightQueues_.end(); ++i)
			{
				SharedPtr<WorkItem> lightItem = queue->GetFreeItem();
				lightItem->priority_ = M_MAX_UNSIGNED;
				lightItem->workFunction_ = SortLightQueueWork;
				lightItem->start_ = &(*i);
				queue->AddWorkItem(lightItem);

				if(i->shadowSplits_.size())
				{
					SharedPtr<WorkItem> shadowItem = queue->GetFreeItem();
					shadowItem->priority_ = M_MAX_UNSIGNED;
					shadowItem->workFunction_ = SortShadowQueueWork;
					shadowItem->start_ = &(*i);
					queue->AddWorkItem(shadowItem);
				}
			}
		}

		// Update geometries. Split into threaded and non-threaded updates.
	{
		if(threadedGeometries_.size())
		{
			// In special cases (context loss, multi-view) a drawable may theoretically first have reported a threaded update, but will actually
			// require a main thread update. Check these cases first and move as applicable. The threaded work routine will tolerate the null
			// pointer holes that we leave to the threaded update queue.
			for(YumeVector<YumeDrawable*>::iterator i = threadedGeometries_.begin(); i != threadedGeometries_.end(); ++i)
			{
				if((*i)->GetUpdateGeometryType() == UPDATE_MAIN_THREAD)
				{
					nonThreadedGeometries_.push_back(*i);
					*i = 0;
				}
			}

			int numWorkItems = queue->GetNumThreads() + 1; // Worker threads + main thread
			int drawablesPerItem = threadedGeometries_.size() / numWorkItems;

			YumeVector<YumeDrawable*>::iterator start = threadedGeometries_.begin();
			for(int i = 0; i < numWorkItems; ++i)
			{
				YumeVector<YumeDrawable*>::iterator end = threadedGeometries_.end();
				if(i < numWorkItems - 1 && end - start > drawablesPerItem)
					end = start + drawablesPerItem;

				SharedPtr<WorkItem> item = queue->GetFreeItem();
				item->priority_ = M_MAX_UNSIGNED;
				item->workFunction_ = UpdateDrawableGeometriesWork;
				item->aux_ = const_cast<FrameInfo*>(&frame_);
				item->start_ = &(*start);
				item->end_ = &(*end);
				queue->AddWorkItem(item);

				start = end;
			}
		}

		// While the work queue is processed, update non-threaded geometries
		for(YumeVector<YumeDrawable*>::const_iterator i = nonThreadedGeometries_.begin(); i != nonThreadedGeometries_.end(); ++i)
			(*i)->UpdateGeometry(frame_);
	}

	// Finally ensure all threaded work has completed
	queue->Complete(M_MAX_UNSIGNED);
	geometriesUpdated_ = true;
	}

	void YumeRenderView::GetLitBatches(YumeDrawable* drawable,LightBatchQueue& lightQueue,BatchQueue* alphaQueue)
	{
		YumeLight* light = lightQueue.light_;
		YumeRendererEnvironment* zone = GetZone(drawable);
		const YumeVector<SourceBatch>::type& batches = drawable->GetBatches();

		bool allowLitBase =
			useLitBase_ && !lightQueue.negative_ && light == drawable->GetFirstLight() && drawable->GetVertexLights().empty() &&
			!zone->GetAmbientGradient();

		for(unsigned i = 0; i < batches.size(); ++i)
		{
			const SourceBatch& srcBatch = batches[i];

			YumeRenderTechnique* tech = GetTechnique(drawable,srcBatch.material_);
			if(!srcBatch.geometry_ || !srcBatch.numWorldTransforms_ || !tech)
				continue;

			// Do not create pixel lit forward passes for materials that render into the G-buffer
			if(gBufferPassIndex_ != M_MAX_UNSIGNED && tech->HasPass(gBufferPassIndex_))
				continue;

			Batch destBatch(srcBatch);
			bool isLitAlpha = false;

			// Check for lit base pass. Because it uses the replace blend mode, it must be ensured to be the first light
			// Also vertex lighting or ambient gradient require the non-lit base pass, so skip in those cases
			if(i < 32 && allowLitBase)
			{
				destBatch.pass_ = tech->GetSupportedPass(litBasePassIndex_);
				if(destBatch.pass_)
				{
					destBatch.isBase_ = true;
					drawable->SetBasePass(i);
				}
				else
					destBatch.pass_ = tech->GetSupportedPass(lightPassIndex_);
			}
			else
				destBatch.pass_ = tech->GetSupportedPass(lightPassIndex_);

			// If no lit pass, check for lit alpha
			if(!destBatch.pass_)
			{
				destBatch.pass_ = tech->GetSupportedPass(litAlphaPassIndex_);
				isLitAlpha = true;
			}

			// Skip if material does not receive light at all
			if(!destBatch.pass_)
				continue;

			destBatch.lightQueue_ = &lightQueue;
			destBatch.zone_ = zone;

			if(!isLitAlpha)
			{
				if(destBatch.isBase_)
					AddBatchToQueue(lightQueue.litBaseBatches_,destBatch,tech);
				else
					AddBatchToQueue(lightQueue.litBatches_,destBatch,tech);
			}
			else if(alphaQueue)
			{
				// Transparent batches can not be instanced, and shadows on transparencies can only be rendered if shadow maps are
				// not reused
				AddBatchToQueue(*alphaQueue,destBatch,tech,false,!renderer_->GetReuseShadowMaps());
			}
		}
	}

	void YumeRenderView::ExecuteRenderPathCommands()
	{
		YumeRenderView* actualView = sourceView_ ? sourceView_ : this;

		// If not reusing shadowmaps, render all of them first
		if(!renderer_->GetReuseShadowMaps() && renderer_->GetDrawShadows() && !actualView->lightQueues_.empty())
		{
			for(YumeVector<LightBatchQueue>::iterator i = actualView->lightQueues_.begin(); i != actualView->lightQueues_.end(); ++i)
			{
				if(i->shadowMap_)
					RenderShadowMap(*i);
			}
		}

	{
		// Set for safety in case of empty renderpath
		currentRenderTarget_ = substituteRenderTarget_ ? substituteRenderTarget_ : renderTarget_;
		currentViewportTexture_ = 0;

		bool viewportModified = false;
		bool isPingponging = false;
		usedResolve_ = false;

		unsigned lastCommandIndex = 0;
		for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
		{
			RenderCommand& command = renderPath_->commands_[i];
			if(actualView->IsNecessary(command))
				lastCommandIndex = i;
		}



		for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
		{
			RenderCommand& command = renderPath_->commands_[i];
			if(!actualView->IsNecessary(command))
				continue;

			bool viewportRead = actualView->CheckViewportRead(command);
			bool viewportWrite = actualView->CheckViewportWrite(command);
			bool beginPingpong = actualView->CheckPingpong(i);

			// Has the viewport been modified and will be read as a texture by the current command?
			if(viewportRead && viewportModified)
			{
				// Start pingponging without a blit if already rendering to the substitute render target
				if(currentRenderTarget_ && currentRenderTarget_ == substituteRenderTarget_ && beginPingpong)
					isPingponging = true;

				// If not using pingponging, simply resolve/copy to the first viewport texture
				if(!isPingponging)
				{
					if(!currentRenderTarget_)
					{
						graphics_->ResolveToTexture(dynamic_cast<YumeTexture2D*>(viewportTextures_[0]),viewRect_);
						currentViewportTexture_ = viewportTextures_[0];
						viewportModified = false;
						usedResolve_ = true;
					}
					else
					{
						if(viewportWrite)
						{
							BlitFramebuffer(currentRenderTarget_->GetParentTexture(),
								GetRenderSurfaceFromTexture(viewportTextures_[0]),false);
							currentViewportTexture_ = viewportTextures_[0];
							viewportModified = false;
						}
						else
						{
							// If the current render target is already a texture, and we are not writing to it, can read that
							// texture directly instead of blitting. However keep the viewport dirty flag in case a later command
							// will do both read and write, and then we need to blit / resolve
							currentViewportTexture_ = currentRenderTarget_->GetParentTexture();
						}
					}
				}
				else
				{
					// Swap the pingpong double buffer sides. Texture 0 will be read next
					viewportTextures_[1] = viewportTextures_[0];
					viewportTextures_[0] = currentRenderTarget_->GetParentTexture();
					currentViewportTexture_ = viewportTextures_[0];
					viewportModified = false;
				}
			}

			if(beginPingpong)
				isPingponging = true;

			// Determine viewport write target
			if(viewportWrite)
			{
				if(isPingponging)
				{
					currentRenderTarget_ = GetRenderSurfaceFromTexture(viewportTextures_[1]);
					// If the render path ends into a quad, it can be redirected to the final render target
					// However, on OpenGL we can not reliably do this in case the final target is the backbuffer, and we want to
					// render depth buffer sensitive debug geometry afterward (backbuffer and textures can not share depth)
					if(renderer_->GetName() == "OpenGL")
					{
						if(i == lastCommandIndex && command.type_ == CMD_QUAD && renderTarget_)
							currentRenderTarget_ = renderTarget_;
					}
					else
					{
						if(i == lastCommandIndex && command.type_ == CMD_QUAD)
							currentRenderTarget_ = renderTarget_;
					}
				}
				else
					currentRenderTarget_ = substituteRenderTarget_ ? substituteRenderTarget_ : renderTarget_;

			}
			switch(command.type_)
			{
			case CMD_CLEAR:
			{
				RHIEvent event("Clear");

				YumeColor clearColor = command.clearColor_;
				if(command.useFogColor_)
					clearColor = actualView->farClipZone_->GetFogColor();

				SetRenderTargets(command);
				graphics_->Clear(command.clearFlags_,clearColor,command.clearDepth_,command.clearStencil_);
			}
			break;

			case CMD_SCENEPASS:
			{
				BatchQueue& queue = actualView->batchQueues_[command.passIndex_];
				if(!queue.IsEmpty())
				{
					YumeString eventName("Scene Pass ");
					eventName.AppendWithFormat("%i",command.passIndex_);
					RHIEvent rhiEvent(eventName);
					SetRenderTargets(command);
					bool allowDepthWrite = SetTextures(command);
					graphics_->SetClipPlane(camera_->GetUseClipping(),camera_->GetClipPlane(),camera_->GetView(),
						camera_->GetProjection());

					if(command.passIndex_ == 3 && renderer_->GetGBufferDebugRendering())
					{
						graphics_->ClearRenderTarget(0,CLEAR_COLOR | CLEAR_DEPTH);
						graphics_->ClearRenderTarget(1,CLEAR_COLOR);
						graphics_->ClearRenderTarget(2,CLEAR_COLOR);
						graphics_->ClearRenderTarget(3,CLEAR_COLOR);
					}

					queue.Draw(this,camera_,command.markToStencil_,false,allowDepthWrite);


				}
			}
			break;
			case CMD_QUAD:
			{
				RHIEvent event("Render Quad");
				SetRenderTargets(command);
				SetTextures(command);
				RenderQuad(command);
			}
			break;

			case CMD_FORWARDLIGHTS:
				// Render shadow maps + opaque objects' additive lighting
				if(!actualView->lightQueues_.empty())
				{
					RHIEvent event("Begin Rendering Forward Lights");
					SetRenderTargets(command);

					for(YumeVector<LightBatchQueue>::iterator i = actualView->lightQueues_.begin(); i != actualView->lightQueues_.end(); ++i)
					{
						// If reusing shadowmaps, render each of them before the lit batches
						if(renderer_->GetReuseShadowMaps() && i->shadowMap_)
						{
							RHIEvent event("Render Shadowmap");
							RenderShadowMap(*i);
							SetRenderTargets(command);
						}

						bool allowDepthWrite = SetTextures(command);
						graphics_->SetClipPlane(camera_->GetUseClipping(),camera_->GetClipPlane(),camera_->GetView(),
							camera_->GetProjection());

						RHIEvent event("Render Forward Lights");
						// Draw base (replace blend) batches first
						i->litBaseBatches_.Draw(this,camera_,false,false,allowDepthWrite);

						// Then, if there are additive passes, optimize the light and draw them
						if(!i->litBatches_.IsEmpty())
						{
							RHIEvent event("Optimize Light");
							renderer_->OptimizeLightByScissor(i->light_,camera_);
							if(!noStencil_)
								renderer_->OptimizeLightByStencil(i->light_,camera_);
							i->litBatches_.Draw(this,camera_,false,true,allowDepthWrite);
						}
					}

					graphics_->SetScissorTest(false);
					graphics_->SetStencilTest(false);
				}
				break;

			case CMD_LIGHTVOLUMES:
				// Render shadow maps + light volumes
				if(!actualView->lightQueues_.empty())
				{
					RHIEvent event("Begin Rendering Light Volumes");
					SetRenderTargets(command);
					for(YumeVector<LightBatchQueue>::iterator i = actualView->lightQueues_.begin(); i != actualView->lightQueues_.end(); ++i)
					{
						// If reusing shadowmaps, render each of them before the lit batches
						if(renderer_->GetReuseShadowMaps() && i->shadowMap_)
						{
							RHIEvent("Render Shadowmap");
							RenderShadowMap(*i);
							SetRenderTargets(command);
						}

						SetTextures(command);

						for(unsigned j = 0; j < i->volumeBatches_.size(); ++j)
						{
							YumeString eventName("Render Light Volume #");
							eventName.append(j);
							RHIEvent rhiEvent(eventName);
							SetupLightVolumeBatch(i->volumeBatches_[j]);
							i->volumeBatches_[j].Draw(this,camera_,false);
						}
					}

					graphics_->SetScissorTest(false);
					graphics_->SetStencilTest(false);
				}
				break;

			case CMD_RENDERUI:
			{
				//Todo
			}
			break;

			default:
				break;
			}

			// If current command output to the viewport, mark it modified
			if(viewportWrite)
				viewportModified = true;
		}
	}
	}

	void YumeRenderView::SetRenderTargets(RenderCommand& command)
	{
		unsigned index = 0;
		bool useColorWrite = true;
		bool useCustomDepth = false;
		bool useViewportOutput = false;

		while(index < command.outputs_.size())
		{
			if(!command.outputs_[index].first.Compare("viewport",false))
			{
				graphics_->SetRenderTarget(index,currentRenderTarget_);
				useViewportOutput = true;
			}
			else
			{
				YumeTexture* texture = FindNamedTexture(command.outputs_[index].first,true,false);

				// Check for depth only rendering (by specifying a depth texture as the sole output)
				if(!index && command.outputs_.size() == 1 && texture && (texture->GetFormat() == graphics_->GetReadableDepthFormatNs() ||
					texture->GetFormat() == graphics_->GetDepthStencilFormatNs()))
				{
					useColorWrite = false;
					useCustomDepth = true;

					graphics_->SetRenderTarget(0,GetRenderSurfaceFromTexture(depthOnlyDummyTexture_));
					graphics_->SetDepthStencil(GetRenderSurfaceFromTexture(texture));
				}
				else
					graphics_->SetRenderTarget(index,GetRenderSurfaceFromTexture(texture,command.outputs_[index].second));
			}

			++index;
		}

		while(index < MAX_RENDERTARGETS)
		{
			graphics_->SetRenderTarget(index,(YumeRenderable*)0);
			++index;
		}

		if(command.depthStencilName_.length())
		{
			YumeTexture* depthTexture = FindNamedTexture(command.depthStencilName_,true,false);
			if(depthTexture)
			{
				useCustomDepth = true;
				graphics_->SetDepthStencil(GetRenderSurfaceFromTexture(depthTexture));
			}
		}

		// When rendering to the final destination rendertarget, use the actual viewport. Otherwise texture rendertargets should use 
		// their full size as the viewport
		IntVector2 rtSizeNow = graphics_->GetRenderTargetDimensions();
		IntRect viewport = (useViewportOutput && currentRenderTarget_ == renderTarget_) ? viewRect_ : IntRect(0,0,rtSizeNow.x_,
			rtSizeNow.y_);

		if(!useCustomDepth)
			graphics_->SetDepthStencil(GetDepthStencil(graphics_->GetRenderTarget(0)));
		graphics_->SetViewport(viewport);
		graphics_->SetColorWrite(useColorWrite);
	}

	bool YumeRenderView::SetTextures(RenderCommand& command)
	{
		bool allowDepthWrite = true;

		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			if(command.textureNames_[i].empty())
				continue;

			// Bind the rendered output
			if(!command.textureNames_[i].Compare("viewport",false))
			{
				graphics_->SetTexture(i,currentViewportTexture_);
				continue;
			}
			YumeTexture* texture = FindNamedTexture(command.textureNames_[i],false,i == TU_VOLUMEMAP);

			if(texture)
			{
				graphics_->SetTexture(i,texture);
				// Check if the current depth stencil is being sampled
				if(graphics_->GetDepthStencil() && texture == graphics_->GetDepthStencil()->GetParentTexture())
					allowDepthWrite = false;
			}
			else
			{
				// If requesting a texture fails, clear the texture name to prevent redundant attempts
				command.textureNames_[i] = EmptyString;
			}
		}

		return allowDepthWrite;
	}


	void YumeRenderView::RenderQuad(RenderCommand& command)
	{
		if(command.vertexShaderName_.empty() || command.pixelShaderName_.empty())
			return;

		// If shader can not be found, clear it from the command to prevent redundant attempts
		YumeShaderVariation* vs = graphics_->GetShader(VS,command.vertexShaderName_,command.vertexShaderDefines_);
		if(!vs)
			command.vertexShaderName_ = EmptyString;
		YumeShaderVariation* ps = graphics_->GetShader(PS,command.pixelShaderName_,command.pixelShaderDefines_);
		if(!ps)
			command.pixelShaderName_ = EmptyString;

		// Set shaders & shader parameters and textures
		graphics_->SetShaders(vs,ps);

		const YumeMap<YumeHash,Variant>::type& parameters = command.shaderParameters_;
		for(YumeMap<YumeHash,Variant>::const_iterator k = parameters.begin(); k != parameters.end(); ++k)
			graphics_->SetShaderParameter(k->first,k->second);

		SetGlobalShaderParameters();
		SetCameraShaderParameters(camera_,false);

		// During renderpath commands the G-Buffer or viewport texture is assumed to always be viewport-sized
		IntRect viewport = graphics_->GetViewport();
		IntVector2 viewSize = IntVector2(viewport.Width(),viewport.Height());
		SetGBufferShaderParameters(viewSize,IntRect(0,0,viewSize.x_,viewSize.y_));

		// Set per-rendertarget inverse size / offset shader parameters as necessary
		for(unsigned i = 0; i < renderPath_->renderTargets_.size(); ++i)
		{
			const RenderTargetInfo& rtInfo = renderPath_->renderTargets_[i];
			if(!rtInfo.enabled_)
				continue;

			YumeHash nameHash(rtInfo.name_);
			if(renderTargets_.find(nameHash) == renderTargets_.end())
				continue;

			YumeString invSizeName = rtInfo.name_ + "InvSize";
			YumeString offsetsName = rtInfo.name_ + "Offsets";
			float width = (float)renderTargets_[nameHash]->GetWidth();
			float height = (float)renderTargets_[nameHash]->GetHeight();

			const Vector2& pixelUVOffset = graphics_->GetPixelUVOffset();
			graphics_->SetShaderParameter(invSizeName,Vector2(1.0f / width,1.0f / height));
			graphics_->SetShaderParameter(offsetsName,Vector2(pixelUVOffset.x_ / width,pixelUVOffset.y_ / height));
		}

		graphics_->SetBlendMode(command.blendMode_);
		graphics_->SetDepthTest(CMP_ALWAYS);
		graphics_->SetDepthWrite(false);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetClipPlane(false);
		graphics_->SetScissorTest(false);
		graphics_->SetStencilTest(false);



		bool ssaoPass = false;
		if(vs->GetName() == "LinearDepthSSAO")
		{
			ssaoPass = true;
			graphics_->SetTexture(TU_CUSTOM1,gYume->pRenderer->GetRandomVectorMap());

			Matrix4 p = camera_->GetProjection();

			Vector4 projInfo = Vector4(2.0f / p.m00_,
				-2.0f / p.m11_,
				-(1.0f + p.m02_ + 1.0f / gYume->pRHI->GetWidth()) / p.m00_,
				(1.0f - p.m12_ + 1.0f / gYume->pRHI->GetHeight()) / p.m11_);

			float viewSize = 2.0f * camera_->GetHalfViewSize();

			
			renderPath_->SetShaderParameter("ProjInfo",projInfo);
			renderPath_->SetShaderParameter("ProjScale",(gYume->pRHI->GetHeight()/ viewSize));
		}
		DrawFullscreenQuad(false,ssaoPass);
	}

	bool YumeRenderView::IsNecessary(const RenderCommand& command)
	{
		return command.enabled_ && command.outputs_.size() &&
			(command.type_ != CMD_SCENEPASS || !batchQueues_[command.passIndex_].IsEmpty());
	}

	bool YumeRenderView::CheckViewportRead(const RenderCommand& command)
	{
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
		{
			if(!command.textureNames_[i].empty() && !command.textureNames_[i].Compare("viewport",false))
				return true;
		}

		return false;
	}

	bool YumeRenderView::CheckViewportWrite(const RenderCommand& command)
	{
		for(unsigned i = 0; i < command.outputs_.size(); ++i)
		{
			if(!command.outputs_[i].first.Compare("viewport",false))
				return true;
		}

		return false;
	}


	bool YumeRenderView::CheckPingpong(unsigned index)
	{
		// Current command must be a viewport-reading & writing quad to begin the pingpong chain
		RenderCommand& current = renderPath_->commands_[index];
		if(current.type_ != CMD_QUAD || !CheckViewportRead(current) || !CheckViewportWrite(current))
			return false;

		// If there are commands other than quads that target the viewport, we must keep rendering to the final target and resolving
		// to a viewport texture when necessary instead of pingponging, as a scene pass is not guaranteed to fill the entire viewport
		for(unsigned i = index + 1; i < renderPath_->commands_.size(); ++i)
		{
			RenderCommand& command = renderPath_->commands_[i];
			if(!IsNecessary(command))
				continue;
			if(CheckViewportWrite(command))
			{
				if(command.type_ != CMD_QUAD)
					return false;
			}
		}

		return true;
	}

	void YumeRenderView::AllocateScreenBuffers()
	{
		YumeRenderView* actualView = sourceView_ ? sourceView_ : this;

		bool hasScenePassToRTs = false;
		bool hasCustomDepth = false;
		bool hasViewportRead = false;
		bool hasPingpong = false;
		bool needSubstitute = false;
		unsigned numViewportTextures = 0;
		depthOnlyDummyTexture_ = 0;

		// Check for commands with special meaning: has custom depth, renders a scene pass to other than the destination viewport,
		// read the viewport, or pingpong between viewport textures. These may trigger the need to substitute the destination RT
		for(unsigned i = 0; i < renderPath_->commands_.size(); ++i)
		{
			const RenderCommand& command = renderPath_->commands_[i];
			if(!actualView->IsNecessary(command))
				continue;
			if(!hasViewportRead && CheckViewportRead(command))
				hasViewportRead = true;
			if(!hasPingpong && CheckPingpong(i))
				hasPingpong = true;
			if(command.depthStencilName_.length())
				hasCustomDepth = true;
			if(!hasScenePassToRTs && command.type_ == CMD_SCENEPASS)
			{
				for(unsigned j = 0; j < command.outputs_.size(); ++j)
				{
					if(command.outputs_[j].first.Compare("viewport",false))
					{
						hasScenePassToRTs = true;
						break;
					}
				}
			}
		}
		if(renderer_->GetName() == "OpenGL")
		{
			// Due to FBO limitations, in OpenGL deferred modes need to render to texture first and then blit to the backbuffer
			// Also, if rendering to a texture with full deferred rendering, it must be RGBA to comply with the rest of the buffers,
			// unless using OpenGL 3
			if(((deferred_ || hasScenePassToRTs) && !renderTarget_) || (!graphics_->GetGL3SupportNs() && deferredAmbient_ && renderTarget_
				&& renderTarget_->GetParentTexture()->GetFormat() != graphics_->GetRGBAFormatNs()))
				needSubstitute = true;
			// Also need substitute if rendering to backbuffer using a custom (readable) depth buffer
			if(!renderTarget_ && hasCustomDepth)
				needSubstitute = true;
		}
		// If backbuffer is antialiased when using deferred rendering, need to reserve a buffer
		if(deferred_ && !renderTarget_ && graphics_->GetMultiSample() > 1)
			needSubstitute = true;
		// If viewport is smaller than whole texture/backbuffer in deferred rendering, need to reserve a buffer, as the G-buffer
		// textures will be sized equal to the viewport
		if(viewSize_.x_ < rtSize_.x_ || viewSize_.y_ < rtSize_.y_)
		{
			if(deferred_ || hasScenePassToRTs || hasCustomDepth)
				needSubstitute = true;
		}

		// Follow final rendertarget format, or use RGB to match the backbuffer format
		unsigned format = renderTarget_ ? renderTarget_->GetParentTexture()->GetFormat() : graphics_->GetRGBFormatNs();

		// If HDR rendering is enabled use RGBA16f and reserve a buffer
		if(renderer_->GetHDRRendering())
		{
			format = graphics_->GetRGBAFloat16FormatNs();
			needSubstitute = true;
		}

		if(renderer_->GetName() == "OpenGL")
		{
			// On OpenGL 2 ensure that all MRT buffers are RGBA in deferred rendering
			if(deferred_ && !renderer_->GetHDRRendering() && !graphics_->GetGL3SupportNs())
				format = graphics_->GetRGBAFormatNs();
		}

		if(hasViewportRead)
		{
			++numViewportTextures;

			// If we have viewport read and target is a cube map, must allocate a substitute target instead as BlitFramebuffer()
			// does not support reading a cube map
			if(renderTarget_ && renderTarget_->GetParentTexture()->GetType() == YumeTextureCube::GetTypeStatic())
				needSubstitute = true;

			// If rendering to a texture, but the viewport is less than the whole texture, use a substitute to ensure
			// postprocessing shaders will never read outside the viewport
			if(renderTarget_ && (viewSize_.x_ < renderTarget_->GetWidth() || viewSize_.y_ < renderTarget_->GetHeight()))
				needSubstitute = true;

			if(hasPingpong && !needSubstitute)
				++numViewportTextures;
		}

		// Allocate screen buffers with filtering active in case the quad commands need that
		// Follow the sRGB mode of the destination render target
		bool sRGB = renderTarget_ ? renderTarget_->GetParentTexture()->GetSRGB() : graphics_->GetSRGB();
		substituteRenderTarget_ = needSubstitute ? GetRenderSurfaceFromTexture(renderer_->GetScreenBuffer(viewSize_.x_,viewSize_.y_,
			format,false,true,sRGB)) : (YumeRenderable*)0;
		for(unsigned i = 0; i < MAX_VIEWPORT_TEXTURES; ++i)
		{
			viewportTextures_[i] =
				i < numViewportTextures ? renderer_->GetScreenBuffer(viewSize_.x_,viewSize_.y_,format,false,true,sRGB) :
				(YumeTexture*)0;
		}
		// If using a substitute render target and pingponging, the substitute can act as the second viewport texture
		if(numViewportTextures == 1 && substituteRenderTarget_)
			viewportTextures_[1] = substituteRenderTarget_->GetParentTexture();

		// Allocate extra render targets defined by the rendering path
		for(unsigned i = 0; i < renderPath_->renderTargets_.size(); ++i)
		{
			const RenderTargetInfo& rtInfo = renderPath_->renderTargets_[i];
			if(!rtInfo.enabled_)
				continue;

			float width = rtInfo.size_.x_;
			float height = rtInfo.size_.y_;



			if(rtInfo.sizeMode_ == SIZE_VIEWPORTDIVISOR)
			{
				width = (float)viewSize_.x_ / Max(width,M_EPSILON);
				height = (float)viewSize_.y_ / Max(height,M_EPSILON);
			}
			else if(rtInfo.sizeMode_ == SIZE_VIEWPORTMULTIPLIER)
			{
				width = (float)viewSize_.x_ * width;
				height = (float)viewSize_.y_ * height;
			}

			int intWidth = (int)(width + 0.5f);
			int intHeight = (int)(height + 0.5f);

			// If the rendertarget is persistent, key it with a hash derived from the RT name and the view's pointer
			renderTargets_[rtInfo.name_] =
				renderer_->GetScreenBuffer(intWidth,intHeight,rtInfo.format_,rtInfo.cubemap_,rtInfo.filtered_,rtInfo.sRGB_,
				rtInfo.persistent_ ? YumeHash(rtInfo.name_).Value() + (unsigned)(size_t)this : 0);
		}


	}

	void YumeRenderView::BlitFramebuffer(YumeTexture* source,YumeRenderable* destination,bool depthWrite)
	{
		if(!source)
			return;




		// If blitting to the destination rendertarget, use the actual viewport. Intermediate textures on the other hand
		// are always viewport-sized
		IntVector2 srcSize(source->GetWidth(),source->GetHeight());
		IntVector2 destSize = destination ? IntVector2(destination->GetWidth(),destination->GetHeight()) : IntVector2(
			graphics_->GetWidth(),graphics_->GetHeight());

		IntRect srcRect = (GetRenderSurfaceFromTexture(source) == renderTarget_) ? viewRect_ : IntRect(0,0,srcSize.x_,srcSize.y_);
		IntRect destRect = (destination == renderTarget_) ? viewRect_ : IntRect(0,0,destSize.x_,destSize.y_);

		graphics_->SetBlendMode(BLEND_REPLACE);
		graphics_->SetDepthTest(CMP_ALWAYS);
		graphics_->SetDepthWrite(depthWrite);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetClipPlane(false);
		graphics_->SetScissorTest(false);
		graphics_->SetStencilTest(false);
		graphics_->SetRenderTarget(0,destination);
		for(unsigned i = 1; i < MAX_RENDERTARGETS; ++i)
			graphics_->SetRenderTarget(i,(YumeRenderable*)0);
		graphics_->SetDepthStencil(GetDepthStencil(destination));
		graphics_->SetViewport(destRect);

		static const YumeString shaderName("CopyFramebuffer");
		graphics_->SetShaders(graphics_->GetShader(VS,shaderName),graphics_->GetShader(PS,shaderName));

		SetGBufferShaderParameters(srcSize,srcRect);

		graphics_->SetTexture(TU_DIFFUSE,source);
		DrawFullscreenQuad(false);
	}



	void YumeRenderView::DrawFullscreenQuad(bool nearQuad,bool ssaoPass)
	{
		YumeGeometry* geometry = renderer_->GetQuadGeometry();

		Matrix3x4 model = Matrix3x4::IDENTITY;
		Matrix4 projection = Matrix4::IDENTITY;

		if(renderer_->GetName() == "OpenGL")
		{
			if(camera_ && camera_->GetFlipVertical())
				projection.m11_ = -1.0f;
			model.m23_ = nearQuad ? -1.0f : 1.0f;
		}
		else
		{
			model.m23_ = nearQuad ? 0.0f : 1.0f;
		}




		graphics_->SetCullMode(CULL_NONE);
		graphics_->SetShaderParameter(VSP_MODEL,model);
		graphics_->SetShaderParameter(VSP_VIEWPROJ,projection);



		//Bind vectors necessary for SSAO
		if(ssaoPass)
		{

		}

		graphics_->ClearTransformSources();

		geometry->Draw(graphics_);
	}

	void YumeRenderView::UpdateOccluders(YumePodVector<YumeDrawable*>::type& occluders,YumeCamera* camera)
	{
		float occluderSizeThreshold_ = renderer_->GetOccluderSizeThreshold();
		float halfViewSize = camera->GetHalfViewSize();
		float invOrthoSize = 1.0f / camera->GetOrthoSize();

		for(YumeVector<YumeDrawable*>::iterator i = occluders.begin(); i != occluders.end();)
		{
			YumeDrawable* occluder = *i;
			bool erase = false;

			if(!occluder->IsInView(frame_,true))
				occluder->UpdateBatches(frame_);

			// Check occluder's draw distance (in main camera view)
			float maxDistance = occluder->GetDrawDistance();
			if(maxDistance <= 0.0f || occluder->GetDistance() <= maxDistance)
			{
				// Check that occluder is big enough on the screen
				const BoundingBox& box = occluder->GetWorldBoundingBox();
				float diagonal = box.Size().Length();
				float compare;
				if(!camera->IsOrthographic())
					compare = diagonal * halfViewSize / occluder->GetDistance();
				else
					compare = diagonal * invOrthoSize;

				if(compare < occluderSizeThreshold_)
					erase = true;
				else
				{
					// Store amount of triangles divided by screen size as a sorting key
					// (best occluders are big and have few triangles)
					occluder->SetSortValue((float)occluder->GetNumOccluderTriangles() / compare);
				}
			}
			else
				erase = true;

			if(erase)
				i = occluders.erase(i);
			else
				++i;
		}

		// Sort occluders so that if triangle budget is exceeded, best occluders have been drawn
		if(occluders.size())
			Sort(occluders.begin(),occluders.end(),CompareDrawables);
	}

	void YumeRenderView::DrawOccluders(OcclusionBuffer* buffer,const YumePodVector<YumeDrawable*>::type& occluders)
	{
		buffer->SetMaxTriangles((unsigned)maxOccluderTriangles_);
		buffer->Clear();

		if(!buffer->IsThreaded())
		{
			// If not threaded, draw occluders one by one and test the next occluder against already rasterized depth
			for(unsigned i = 0; i < occluders.size(); ++i)
			{
				YumeDrawable* occluder = occluders[i];
				if(i > 0)
				{
					// For subsequent occluders, do a test against the pixel-level occlusion buffer to see if rendering is necessary
					if(!buffer->IsVisible(occluder->GetWorldBoundingBox()))
						continue;
				}

				// Check for running out of triangles
				++activeOccluders_;
				bool success = occluder->DrawOcclusion(buffer);
				// Draw triangles submitted by this occluder
				buffer->DrawTriangles();
				if(!success)
					break;
			}
		}
		else
		{
			// In threaded mode submit all triangles first, then render (cannot test in this case)
			for(unsigned i = 0; i < occluders.size(); ++i)
			{
				// Check for running out of triangles
				++activeOccluders_;
				if(!occluders[i]->DrawOcclusion(buffer))
					break;
			}

			buffer->DrawTriangles();
		}

		// Finally build the depth mip levels
		buffer->BuildDepthHierarchy();
	}

	void YumeRenderView::ProcessLight(LightQueryResult& query,unsigned threadIndex)
	{
		YumeLight* light = query.light_;
		LightType type = light->GetLightType();
		const Frustum& frustum = cullCamera_->GetFrustum();

		// Check if light should be shadowed
		bool isShadowed = drawShadows_ && light->GetCastShadows() && !light->GetPerVertex() && light->GetShadowIntensity() < 1.0f;
		// If shadow distance non-zero, check it
		if(isShadowed && light->GetShadowDistance() > 0.0f && light->GetDistance() > light->GetShadowDistance())
			isShadowed = false;
		// OpenGL ES can not support point light shadows
#ifdef GL_ES_VERSION_2_0
		if(isShadowed && type == LIGHT_POINT)
			isShadowed = false;
#endif
		// Get lit geometries. They must match the light mask and be inside the main camera frustum to be considered
		YumePodVector<YumeDrawable*>::type& tempDrawables = tempDrawables_[threadIndex];
		query.litGeometries_.clear();

		switch(type)
		{
		case LIGHT_DIRECTIONAL:
			for(unsigned i = 0; i < geometries_.size(); ++i)
			{
				if(GetLightMask(geometries_[i]) & light->GetLightMask())
					query.litGeometries_.push_back(geometries_[i]);
			}
			break;

		case LIGHT_SPOT:
		{
			FrustumOctreeQuery octreeQuery(tempDrawables,light->GetFrustum(),DRAWABLE_GEOMETRY,
				cullCamera_->GetViewMask());
			octree_->GetDrawables(octreeQuery);
			for(unsigned i = 0; i < tempDrawables.size(); ++i)
			{
				if(tempDrawables[i]->IsInView(frame_) && (GetLightMask(tempDrawables[i]) & light->GetLightMask()))
					query.litGeometries_.push_back(tempDrawables[i]);
			}
		}
		break;

		case LIGHT_POINT:
		{
			SphereOctreeQuery octreeQuery(tempDrawables,Sphere(light->GetNode()->GetWorldPosition(),light->GetRange()),
				DRAWABLE_GEOMETRY,cullCamera_->GetViewMask());
			octree_->GetDrawables(octreeQuery);
			for(unsigned i = 0; i < tempDrawables.size(); ++i)
			{
				if(tempDrawables[i]->IsInView(frame_) && (GetLightMask(tempDrawables[i]) & light->GetLightMask()))
					query.litGeometries_.push_back(tempDrawables[i]);
			}
		}
		break;
		}

		// If no lit geometries or not shadowed, no need to process shadow cameras
		if(query.litGeometries_.empty() || !isShadowed)
		{
			query.numSplits_ = 0;
			return;
		}

		// Determine number of shadow cameras and setup their initial positions
		SetupShadowCameras(query);

		// Process each split for shadow casters
		query.shadowCasters_.clear();
		for(unsigned i = 0; i < query.numSplits_; ++i)
		{
			YumeCamera* shadowCamera = query.shadowCameras_[i];
			const Frustum& shadowCameraFrustum = shadowCamera->GetFrustum();
			query.shadowCasterBegin_[i] = query.shadowCasterEnd_[i] = query.shadowCasters_.size();

			// For point light check that the face is visible: if not, can skip the split
			if(type == LIGHT_POINT && frustum.IsInsideFast(BoundingBox(shadowCameraFrustum)) == OUTSIDE)
				continue;

			// For directional light check that the split is inside the visible scene: if not, can skip the split
			if(type == LIGHT_DIRECTIONAL)
			{
				if(minZ_ > query.shadowFarSplits_[i])
					continue;
				if(maxZ_ < query.shadowNearSplits_[i])
					continue;

				// Reuse lit geometry query for all except directional lights
				ShadowCasterOctreeQuery query(tempDrawables,shadowCameraFrustum,DRAWABLE_GEOMETRY,cullCamera_->GetViewMask());
				octree_->GetDrawables(query);
			}

			// Check which shadow casters actually contribute to the shadowing
			ProcessShadowCasters(query,tempDrawables,i);
		}

		// If no shadow casters, the light can be rendered unshadowed. At this point we have not allocated a shadow map yet, so the
		// only cost has been the shadow camera setup & queries
		if(query.shadowCasters_.empty())
			query.numSplits_ = 0;
	}

	void YumeRenderView::ProcessShadowCasters(LightQueryResult& query,const YumePodVector<YumeDrawable*>::type& drawables,unsigned splitIndex)
	{
		YumeLight* light = query.light_;

		YumeCamera* shadowCamera = query.shadowCameras_[splitIndex];
		const Frustum& shadowCameraFrustum = shadowCamera->GetFrustum();
		const Matrix3x4& lightView = shadowCamera->GetView();
		const Matrix4& lightProj = shadowCamera->GetProjection();
		LightType type = light->GetLightType();

		query.shadowCasterBox_[splitIndex].Clear();

		// Transform scene frustum into shadow camera's view space for shadow caster visibility check. For point & spot lights,
		// we can use the whole scene frustum. For directional lights, use the intersection of the scene frustum and the split
		// frustum, so that shadow casters do not get rendered into unnecessary splits
		Frustum lightViewFrustum;
		if(type != LIGHT_DIRECTIONAL)
			lightViewFrustum = cullCamera_->GetSplitFrustum(minZ_,maxZ_).Transformed(lightView);
		else
			lightViewFrustum = cullCamera_->GetSplitFrustum(Max(minZ_,query.shadowNearSplits_[splitIndex]),
			Min(maxZ_,query.shadowFarSplits_[splitIndex])).Transformed(lightView);

		BoundingBox lightViewFrustumBox(lightViewFrustum);

		// Check for degenerate split frustum: in that case there is no need to get shadow casters
		if(lightViewFrustum.vertices_[0] == lightViewFrustum.vertices_[4])
			return;

		BoundingBox lightViewBox;
		BoundingBox lightProjBox;

		for(YumeVector<YumeDrawable*>::const_iterator i = drawables.begin(); i != drawables.end(); ++i)
		{
			YumeDrawable* drawable = *i;
			// In case this is a point or spot light query result reused for optimization, we may have non-shadowcasters included.
			// Check for that first
			if(!drawable->GetCastShadows())
				continue;
			// Check shadow mask
			if(!(GetShadowMask(drawable) & light->GetLightMask()))
				continue;
			// For point light, check that this drawable is inside the split shadow camera frustum
			if(type == LIGHT_POINT && shadowCameraFrustum.IsInsideFast(drawable->GetWorldBoundingBox()) == OUTSIDE)
				continue;

			// Check shadow distance
			// Note: as lights are processed threaded, it is possible a drawable's UpdateBatches() function is called several
			// times. However, this should not cause problems as no scene modification happens at this point.
			if(!drawable->IsInView(frame_,true))
				drawable->UpdateBatches(frame_);
			float maxShadowDistance = drawable->GetShadowDistance();
			float drawDistance = drawable->GetDrawDistance();
			if(drawDistance > 0.0f && (maxShadowDistance <= 0.0f || drawDistance < maxShadowDistance))
				maxShadowDistance = drawDistance;
			if(maxShadowDistance > 0.0f && drawable->GetDistance() > maxShadowDistance)
				continue;

			// Project shadow caster bounding box to light view space for visibility check
			lightViewBox = drawable->GetWorldBoundingBox().Transformed(lightView);

			if(IsShadowCasterVisible(drawable,lightViewBox,shadowCamera,lightView,lightViewFrustum,lightViewFrustumBox))
			{
				// Merge to shadow caster bounding box (only needed for focused spot lights) and add to the list
				if(type == LIGHT_SPOT && light->GetShadowFocus().focus_)
				{
					lightProjBox = lightViewBox.Projected(lightProj);
					query.shadowCasterBox_[splitIndex].Merge(lightProjBox);
				}
				query.shadowCasters_.push_back(drawable);
			}
		}

		query.shadowCasterEnd_[splitIndex] = query.shadowCasters_.size();
	}

	bool YumeRenderView::IsShadowCasterVisible(YumeDrawable* drawable,BoundingBox lightViewBox,YumeCamera* shadowCamera,const Matrix3x4& lightView,
		const Frustum& lightViewFrustum,const BoundingBox& lightViewFrustumBox)
	{
		if(shadowCamera->IsOrthographic())
		{
			// Extrude the light space bounding box up to the far edge of the frustum's light space bounding box
			lightViewBox.max_.z_ = Max(lightViewBox.max_.z_,lightViewFrustumBox.max_.z_);
			return lightViewFrustum.IsInsideFast(lightViewBox) != OUTSIDE;
		}
		else
		{
			// If light is not directional, can do a simple check: if object is visible, its shadow is too
			if(drawable->IsInView(frame_))
				return true;

			// For perspective lights, extrusion direction depends on the position of the shadow caster
			Vector3 center = lightViewBox.Center();
			Ray extrusionRay(center,center);

			float extrusionDistance = shadowCamera->GetFarClip();
			float originalDistance = Clamp(center.Length(),M_EPSILON,extrusionDistance);

			// Because of the perspective, the bounding box must also grow when it is extruded to the distance
			float sizeFactor = extrusionDistance / originalDistance;

			// Calculate the endpoint box and merge it to the original. Because it's axis-aligned, it will be larger
			// than necessary, so the test will be conservative
			Vector3 newCenter = extrusionDistance * extrusionRay.direction_;
			Vector3 newHalfSize = lightViewBox.Size() * sizeFactor * 0.5f;
			BoundingBox extrudedBox(newCenter - newHalfSize,newCenter + newHalfSize);
			lightViewBox.Merge(extrudedBox);

			return lightViewFrustum.IsInsideFast(lightViewBox) != OUTSIDE;
		}
	}

	IntRect YumeRenderView::GetShadowMapViewport(YumeLight* light,unsigned splitIndex,YumeTexture2D* shadowMap)
	{
		unsigned width = (unsigned)shadowMap->GetWidth();
		unsigned height = (unsigned)shadowMap->GetHeight();

		switch(light->GetLightType())
		{
		case LIGHT_DIRECTIONAL:
		{
			int numSplits = light->GetNumShadowSplits();
			if(numSplits == 1)
				return IntRect(0,0,width,height);
			else if(numSplits == 2)
				return IntRect(splitIndex * width / 2,0,(splitIndex + 1) * width / 2,height);
			else
				return IntRect((splitIndex & 1) * width / 2,(splitIndex / 2) * height / 2,((splitIndex & 1) + 1) * width / 2,
				(splitIndex / 2 + 1) * height / 2);
		}

		case LIGHT_SPOT:
			return IntRect(0,0,width,height);

		case LIGHT_POINT:
			return IntRect((splitIndex & 1) * width / 2,(splitIndex / 2) * height / 3,((splitIndex & 1) + 1) * width / 2,
				(splitIndex / 2 + 1) * height / 3);
		}

		return IntRect();
	}

	void YumeRenderView::SetupShadowCameras(LightQueryResult& query)
	{
		YumeLight* light = query.light_;

		int splits = 0;

		switch(light->GetLightType())
		{
		case LIGHT_DIRECTIONAL:
		{
			const CascadeParameters& cascade = light->GetShadowCascade();

			float nearSplit = cullCamera_->GetNearClip();
			float farSplit;
			int numSplits = light->GetNumShadowSplits();

			while(splits < numSplits)
			{
				// If split is completely beyond camera far clip, we are done
				if(nearSplit > cullCamera_->GetFarClip())
					break;

				farSplit = Min(cullCamera_->GetFarClip(),cascade.splits_[splits]);
				if(farSplit <= nearSplit)
					break;

				// Setup the shadow camera for the split
				YumeCamera* shadowCamera = renderer_->GetShadowCamera();
				query.shadowCameras_[splits] = shadowCamera;
				query.shadowNearSplits_[splits] = nearSplit;
				query.shadowFarSplits_[splits] = farSplit;
				SetupDirLightShadowCamera(shadowCamera,light,nearSplit,farSplit);

				nearSplit = farSplit;
				++splits;
			}
		}
		break;

		case LIGHT_SPOT:
		{
			YumeCamera* shadowCamera = renderer_->GetShadowCamera();
			query.shadowCameras_[0] = shadowCamera;
			YumeSceneNode* cameraNode = shadowCamera->GetNode();
			YumeSceneNode* lightNode = light->GetNode();

			cameraNode->SetTransform(lightNode->GetWorldPosition(),lightNode->GetWorldRotation());
			shadowCamera->SetNearClip(light->GetShadowNearFarRatio() * light->GetRange());
			shadowCamera->SetFarClip(light->GetRange());
			shadowCamera->SetFov(light->GetFov());
			shadowCamera->SetAspectRatio(light->GetAspectRatio());

			splits = 1;
		}
		break;

		case LIGHT_POINT:
		{
			for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			{
				YumeCamera* shadowCamera = renderer_->GetShadowCamera();
				query.shadowCameras_[i] = shadowCamera;
				YumeSceneNode* cameraNode = shadowCamera->GetNode();

				// When making a shadowed point light, align the splits along X, Y and Z axes regardless of light rotation
				cameraNode->SetPosition(light->GetNode()->GetWorldPosition());
				cameraNode->SetDirection(*directions[i]);
				shadowCamera->SetNearClip(light->GetShadowNearFarRatio() * light->GetRange());
				shadowCamera->SetFarClip(light->GetRange());
				shadowCamera->SetFov(90.0f);
				shadowCamera->SetAspectRatio(1.0f);
			}

			splits = MAX_CUBEMAP_FACES;
		}
		break;
		}

		query.numSplits_ = (unsigned)splits;
	}

	void YumeRenderView::SetupDirLightShadowCamera(YumeCamera* shadowCamera,YumeLight* light,float nearSplit,float farSplit)
	{
		YumeSceneNode* shadowCameraNode = shadowCamera->GetNode();
		YumeSceneNode* lightNode = light->GetNode();
		float extrusionDistance = cullCamera_->GetFarClip();
		const FocusParameters& parameters = light->GetShadowFocus();

		// Calculate initial position & rotation
		Vector3 pos = cullCamera_->GetNode()->GetWorldPosition() - extrusionDistance * lightNode->GetWorldDirection();
		shadowCameraNode->SetTransform(pos,lightNode->GetWorldRotation());

		// Calculate main camera shadowed frustum in light's view space
		farSplit = Min(farSplit,cullCamera_->GetFarClip());
		// Use the scene Z bounds to limit frustum size if applicable
		if(parameters.focus_)
		{
			nearSplit = Max(minZ_,nearSplit);
			farSplit = Min(maxZ_,farSplit);
		}

		Frustum splitFrustum = cullCamera_->GetSplitFrustum(nearSplit,farSplit);
		Polyhedron frustumVolume;
		frustumVolume.Define(splitFrustum);
		// If focusing enabled, clip the frustum volume by the combined bounding box of the lit geometries within the frustum
		if(parameters.focus_)
		{
			BoundingBox litGeometriesBox;
			for(unsigned i = 0; i < geometries_.size(); ++i)
			{
				YumeDrawable* drawable = geometries_[i];
				if(drawable->GetMinZ() <= farSplit && drawable->GetMaxZ() >= nearSplit &&
					(GetLightMask(drawable) & light->GetLightMask()))
					litGeometriesBox.Merge(drawable->GetWorldBoundingBox());
			}
			if(litGeometriesBox.Defined())
			{
				frustumVolume.Clip(litGeometriesBox);
				// If volume became empty, restore it to avoid zero size
				if(frustumVolume.Empty())
					frustumVolume.Define(splitFrustum);
			}
		}

		// Transform frustum volume to light space
		const Matrix3x4& lightView = shadowCamera->GetView();
		frustumVolume.Transform(lightView);

		// Fit the frustum volume inside a bounding box. If uniform size, use a sphere instead
		BoundingBox shadowBox;
		if(!parameters.nonUniform_)
			shadowBox.Define(Sphere(frustumVolume));
		else
			shadowBox.Define(frustumVolume);

		shadowCamera->SetOrthographic(true);
		shadowCamera->SetAspectRatio(1.0f);
		shadowCamera->SetNearClip(0.0f);
		shadowCamera->SetFarClip(shadowBox.max_.z_);

		// Center shadow camera on the bounding box. Can not snap to texels yet as the shadow map viewport is unknown
		QuantizeDirLightShadowCamera(shadowCamera,light,IntRect(0,0,0,0),shadowBox);
	}

	void YumeRenderView::FinalizeShadowCamera(YumeCamera* shadowCamera,YumeLight* light,const IntRect& shadowViewport,
		const BoundingBox& shadowCasterBox)
	{
		const FocusParameters& parameters = light->GetShadowFocus();
		float shadowMapWidth = (float)(shadowViewport.Width());
		LightType type = light->GetLightType();

		if(type == LIGHT_DIRECTIONAL)
		{
			BoundingBox shadowBox;
			shadowBox.max_.y_ = shadowCamera->GetOrthoSize() * 0.5f;
			shadowBox.max_.x_ = shadowCamera->GetAspectRatio() * shadowBox.max_.y_;
			shadowBox.min_.y_ = -shadowBox.max_.y_;
			shadowBox.min_.x_ = -shadowBox.max_.x_;

			// Requantize and snap to shadow map texels
			QuantizeDirLightShadowCamera(shadowCamera,light,shadowViewport,shadowBox);
		}

		if(type == LIGHT_SPOT && parameters.focus_)
		{
			float viewSizeX = Max(Abs(shadowCasterBox.min_.x_),Abs(shadowCasterBox.max_.x_));
			float viewSizeY = Max(Abs(shadowCasterBox.min_.y_),Abs(shadowCasterBox.max_.y_));
			float viewSize = Max(viewSizeX,viewSizeY);
			// Scale the quantization parameters, because view size is in projection space (-1.0 - 1.0)
			float invOrthoSize = 1.0f / shadowCamera->GetOrthoSize();
			float quantize = parameters.quantize_ * invOrthoSize;
			float minView = parameters.minView_ * invOrthoSize;

			viewSize = Max(ceilf(viewSize / quantize) * quantize,minView);
			if(viewSize < 1.0f)
				shadowCamera->SetZoom(1.0f / viewSize);
		}

		// Perform a finalization step for all lights: ensure zoom out of 2 pixels to eliminate border filtering issues
		// For point lights use 4 pixels, as they must not cross sides of the virtual cube map (maximum 3x3 PCF)
		if(shadowCamera->GetZoom() >= 1.0f)
		{
			if(light->GetLightType() != LIGHT_POINT)
				shadowCamera->SetZoom(shadowCamera->GetZoom() * ((shadowMapWidth - 2.0f) / shadowMapWidth));
			else
			{
				if(renderer_->GetName() == "OpenGL")
				{
					shadowCamera->SetZoom(shadowCamera->GetZoom() * ((shadowMapWidth - 3.0f) / shadowMapWidth));
				}
				else
				{
					shadowCamera->SetZoom(shadowCamera->GetZoom() * ((shadowMapWidth - 4.0f) / shadowMapWidth));
				}

			}
		}
	}

	void YumeRenderView::QuantizeDirLightShadowCamera(YumeCamera* shadowCamera,YumeLight* light,const IntRect& shadowViewport,
		const BoundingBox& viewBox)
	{
		YumeSceneNode* shadowCameraNode = shadowCamera->GetNode();
		const FocusParameters& parameters = light->GetShadowFocus();
		float shadowMapWidth = (float)(shadowViewport.Width());

		float minX = viewBox.min_.x_;
		float minY = viewBox.min_.y_;
		float maxX = viewBox.max_.x_;
		float maxY = viewBox.max_.y_;

		Vector2 center((minX + maxX) * 0.5f,(minY + maxY) * 0.5f);
		Vector2 viewSize(maxX - minX,maxY - minY);

		// Quantize size to reduce swimming
		// Note: if size is uniform and there is no focusing, quantization is unnecessary
		if(parameters.nonUniform_)
		{
			viewSize.x_ = ceilf(sqrtf(viewSize.x_ / parameters.quantize_));
			viewSize.y_ = ceilf(sqrtf(viewSize.y_ / parameters.quantize_));
			viewSize.x_ = Max(viewSize.x_ * viewSize.x_ * parameters.quantize_,parameters.minView_);
			viewSize.y_ = Max(viewSize.y_ * viewSize.y_ * parameters.quantize_,parameters.minView_);
		}
		else if(parameters.focus_)
		{
			viewSize.x_ = Max(viewSize.x_,viewSize.y_);
			viewSize.x_ = ceilf(sqrtf(viewSize.x_ / parameters.quantize_));
			viewSize.x_ = Max(viewSize.x_ * viewSize.x_ * parameters.quantize_,parameters.minView_);
			viewSize.y_ = viewSize.x_;
		}

		shadowCamera->SetOrthoSize(viewSize);

		// Center shadow camera to the view space bounding box
		Quaternion rot(shadowCameraNode->GetWorldRotation());
		Vector3 adjust(center.x_,center.y_,0.0f);
		shadowCameraNode->Translate(rot * adjust,TS_WORLD);

		// If the shadow map viewport is known, snap to whole texels
		if(shadowMapWidth > 0.0f)
		{
			Vector3 viewPos(rot.Inverse() * shadowCameraNode->GetWorldPosition());
			// Take into account that shadow map border will not be used
			float invActualSize = 1.0f / (shadowMapWidth - 2.0f);
			Vector2 texelSize(viewSize.x_ * invActualSize,viewSize.y_ * invActualSize);
			Vector3 snap(-fmodf(viewPos.x_,texelSize.x_),-fmodf(viewPos.y_,texelSize.y_),0.0f);
			shadowCameraNode->Translate(rot * snap,TS_WORLD);
		}
	}

	void YumeRenderView::FindZone(YumeDrawable* drawable)
	{
		Vector3 center = drawable->GetWorldBoundingBox().Center();
		int bestPriority = M_MIN_INT;
		YumeRendererEnvironment* newZone = 0;

		// If bounding box center is in view, the zone assignment is conclusive also for next frames. Otherwise it is temporary
		// (possibly incorrect) and must be re-evaluated on the next frame
		bool temporary = !cullCamera_->GetFrustum().IsInside(center);

		// First check if the current zone remains a conclusive result
		YumeRendererEnvironment* lastZone = drawable->GetZone();

		if(lastZone && (lastZone->GetViewMask() & cullCamera_->GetViewMask()) && lastZone->GetPriority() >= highestZonePriority_ &&
			(drawable->GetZoneMask() & lastZone->GetZoneMask()) && lastZone->IsInside(center))
			newZone = lastZone;
		else
		{
			for(YumeVector<YumeRendererEnvironment*>::iterator i = zones_.begin(); i != zones_.end(); ++i)
			{
				YumeRendererEnvironment* zone = *i;
				int priority = zone->GetPriority();
				if(priority > bestPriority && (drawable->GetZoneMask() & zone->GetZoneMask()) && zone->IsInside(center))
				{
					newZone = zone;
					bestPriority = priority;
				}
			}
		}

		drawable->SetZone(newZone,temporary);
	}

	YumeRenderTechnique* YumeRenderView::GetTechnique(YumeDrawable* drawable,YumeMaterial* material)
	{
		if(!material)
			return renderer_->GetDefaultMaterial()->GetTechniques()[0].technique_;

		const YumeVector<TechniqueEntry>::type& techniques = material->GetTechniques();
		// If only one technique, no choice
		if(techniques.size() == 1)
			return techniques[0].technique_;
		else
		{
			float lodDistance = drawable->GetLodDistance();

			// Check for suitable technique. Techniques should be ordered like this:
			// Most distant & highest quality
			// Most distant & lowest quality
			// Second most distant & highest quality
			// ...
			for(unsigned i = 0; i < techniques.size(); ++i)
			{
				const TechniqueEntry& entry = techniques[i];
				YumeRenderTechnique* tech = entry.technique_;

				if(!tech || (!tech->IsSupported()) || materialQuality_ < entry.qualityLevel_)
					continue;
				if(lodDistance >= entry.lodDistance_)
					return tech;
			}

			// If no suitable technique found, fallback to the last
			return techniques.size() ? techniques.back().technique_ : (YumeRenderTechnique*)0;
		}
	}

	void YumeRenderView::CheckMaterialForAuxView(YumeMaterial* material)
	{
		const YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type& textures = material->GetTextures();

		for(YumeMap<TextureUnit,SharedPtr<YumeTexture> >::const_iterator i = textures.begin(); i != textures.end(); ++i)
		{
			YumeTexture* texture = i->second;
			if(texture && texture->GetUsage() == TEXTURE_RENDERTARGET)
			{
				// Have to check cube & 2D textures separately
				if(texture->GetType() == YumeTexture2D::GetTypeStatic())
				{
					YumeTexture2D* tex2D = static_cast<YumeTexture2D*>(texture);
					YumeRenderable* target = tex2D->GetRenderSurface();
					if(target && target->GetUpdateMode() == SURFACE_UPDATEVISIBLE)
						target->QueueUpdate();
				}
				else if(texture->GetType() == YumeTextureCube::GetTypeStatic())
				{
					YumeTextureCube* texCube = static_cast<YumeTextureCube*>(texture);
					for(unsigned j = 0; j < MAX_CUBEMAP_FACES; ++j)
					{
						YumeRenderable* target = texCube->GetRenderSurface((CubeMapFace)j);
						if(target && target->GetUpdateMode() == SURFACE_UPDATEVISIBLE)
							target->QueueUpdate();
					}
				}
			}
		}

		// Flag as processed so we can early-out next time we come across this material on the same frame
		material->MarkForAuxView(frame_.frameNumber_);
	}

	void YumeRenderView::AddBatchToQueue(BatchQueue& batchQueue,Batch& batch,YumeRenderTechnique* tech,bool allowInstancing,bool allowShadows)
	{
		if(!batch.material_)
			batch.material_ = renderer_->GetDefaultMaterial();

		// Convert to instanced if possible
		if(allowInstancing && batch.geometryType_ == GEOM_STATIC && batch.geometry_->GetIndexBuffer())
			batch.geometryType_ = GEOM_INSTANCED;

		if(batch.geometryType_ == GEOM_INSTANCED)
		{
			BatchGroupKey key(batch);

			YumeMap<BatchGroupKey,BatchGroup>::iterator i = batchQueue.batchGroups_.find(key);
			if(i == batchQueue.batchGroups_.end())
			{
				// Create a new group based on the batch
				// In case the group remains below the instancing limit, do not enable instancing shaders yet
				BatchGroup newGroup(batch);
				newGroup.geometryType_ = GEOM_STATIC;
				renderer_->SetBatchShaders(newGroup,tech,allowShadows);
				newGroup.CalculateSortKey();
				i = batchQueue.batchGroups_.insert(MakePair(key,newGroup));
			}

			int oldSize = i->second.instances_.size();
			i->second.AddTransforms(batch);
			// Convert to using instancing shaders when the instancing limit is reached
			if(oldSize < minInstances_ && (int)i->second.instances_.size() >= minInstances_)
			{
				i->second.geometryType_ = GEOM_INSTANCED;
				renderer_->SetBatchShaders(i->second,tech,allowShadows);
				i->second.CalculateSortKey();
			}
		}
		else
		{
			renderer_->SetBatchShaders(batch,tech,allowShadows);
			batch.CalculateSortKey();

			// If batch is static with multiple world transforms and cannot instance, we must push copies of the batch individually
			if(batch.geometryType_ == GEOM_STATIC && batch.numWorldTransforms_ > 1)
			{
				unsigned numTransforms = batch.numWorldTransforms_;
				batch.numWorldTransforms_ = 1;
				for(unsigned i = 0; i < numTransforms; ++i)
				{
					// Move the transform pointer to generate copies of the batch which only refer to 1 world transform
					batchQueue.batches_.push_back(batch);
					++batch.worldTransform_;
				}
			}
			else
				batchQueue.batches_.push_back(batch);
		}
	}

	void YumeRenderView::PrepareInstancingBuffer()
	{
		// Prepare instancing buffer from the source view
		/// \todo If rendering the same view several times back-to-back, would not need to refill the buffer
		if(sourceView_)
		{
			sourceView_->PrepareInstancingBuffer();
			return;
		}

		unsigned totalInstances = 0;

		for(YumeMap<unsigned,BatchQueue>::iterator i = batchQueues_.begin(); i != batchQueues_.end(); ++i)
			totalInstances += i->second.GetNumInstances();

		for(YumeVector<LightBatchQueue>::iterator i = lightQueues_.begin(); i != lightQueues_.end(); ++i)
		{
			for(unsigned j = 0; j < i->shadowSplits_.size(); ++j)
				totalInstances += i->shadowSplits_[j].shadowBatches_.GetNumInstances();
			totalInstances += i->litBaseBatches_.GetNumInstances();
			totalInstances += i->litBatches_.GetNumInstances();
		}

		if(!totalInstances || !renderer_->ResizeInstancingBuffer(totalInstances))
			return;

		YumeVertexBuffer* instancingBuffer = renderer_->GetInstancingBuffer();
		unsigned freeIndex = 0;
		void* dest = instancingBuffer->Lock(0,totalInstances,true);
		if(!dest)
			return;

		for(YumeMap<unsigned,BatchQueue>::iterator i = batchQueues_.begin(); i != batchQueues_.end(); ++i)
			i->second.SetTransforms(dest,freeIndex);

		for(YumeVector<LightBatchQueue>::iterator i = lightQueues_.begin(); i != lightQueues_.end(); ++i)
		{
			for(unsigned j = 0; j < i->shadowSplits_.size(); ++j)
				i->shadowSplits_[j].shadowBatches_.SetTransforms(dest,freeIndex);
			i->litBaseBatches_.SetTransforms(dest,freeIndex);
			i->litBatches_.SetTransforms(dest,freeIndex);
		}

		instancingBuffer->Unlock();
	}

	void YumeRenderView::SetupLightVolumeBatch(Batch& batch)
	{
		YumeLight* light = batch.lightQueue_->light_;
		LightType type = light->GetLightType();
		Vector3 cameraPos = camera_->GetNode()->GetWorldPosition();
		float lightDist;

		graphics_->SetBlendMode(light->IsNegative() ? BLEND_SUBTRACT : BLEND_ADD);
		graphics_->SetDepthBias(0.0f,0.0f);
		graphics_->SetDepthWrite(false);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetClipPlane(false);

		if(type != LIGHT_DIRECTIONAL)
		{
			if(type == LIGHT_POINT)
				lightDist = Sphere(light->GetNode()->GetWorldPosition(),light->GetRange() * 1.25f).Distance(cameraPos);
			else
				lightDist = light->GetFrustum().Distance(cameraPos);

			// Draw front faces if not inside light volume
			if(lightDist < camera_->GetNearClip() * 2.0f)
			{
				renderer_->SetCullMode(CULL_CW,camera_);
				graphics_->SetDepthTest(CMP_GREATER);
			}
			else
			{
				renderer_->SetCullMode(CULL_CCW,camera_);
				graphics_->SetDepthTest(CMP_LESSEQUAL);
			}
		}
		else
		{
			// In case the same camera is used for multiple views with differing aspect ratios (not recommended)
			// refresh the directional light's model transform before rendering
			light->GetVolumeTransform(camera_);
			graphics_->SetCullMode(CULL_NONE);
			graphics_->SetDepthTest(CMP_ALWAYS);
		}

		graphics_->SetScissorTest(false);
		if(!noStencil_)
			graphics_->SetStencilTest(true,CMP_NOTEQUAL,OP_KEEP,OP_KEEP,OP_KEEP,OP_KEEP,0,light->GetLightMask());
		else
			graphics_->SetStencilTest(false);
	}

	void YumeRenderView::RenderShadowMap(const LightBatchQueue& queue)
	{
		YumeTexture2D* shadowMap = queue.shadowMap_;
		graphics_->SetTexture(TU_SHADOWMAP,0);

		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetClipPlane(false);
		graphics_->SetStencilTest(false);

		// Set shadow depth bias
		BiasParameters parameters = queue.light_->GetShadowBias();

		// the shadow map is a depth stencil map
		if(shadowMap->GetUsage() == TEXTURE_DEPTHSTENCIL)
		{
			graphics_->SetColorWrite(false);
			graphics_->SetDepthStencil(shadowMap);
			graphics_->SetRenderTarget(0,shadowMap->GetRenderSurface()->GetLinkedRenderTarget());
			// disable other render targets
			for(unsigned i = 1; i < MAX_RENDERTARGETS; ++i)
				graphics_->SetRenderTarget(i,(YumeRenderable*)0);
			graphics_->SetViewport(IntRect(0,0,shadowMap->GetWidth(),shadowMap->GetHeight()));
			graphics_->Clear(CLEAR_DEPTH);
		}
		else // if the shadow map is a render texture
		{
			graphics_->SetColorWrite(true);
			graphics_->SetRenderTarget(0,shadowMap);
			// disable other render targets
			for(unsigned i = 1; i < MAX_RENDERTARGETS; ++i)
				graphics_->SetRenderTarget(i,(YumeRenderable*)0);
			graphics_->SetDepthStencil(renderer_->GetDepthStencil(shadowMap->GetWidth(),shadowMap->GetHeight()));
			graphics_->SetViewport(IntRect(0,0,shadowMap->GetWidth(),shadowMap->GetHeight()));
			graphics_->Clear(CLEAR_DEPTH | CLEAR_COLOR,YumeColor::WHITE);

			parameters = BiasParameters(0.0f,0.0f);
		}


		for(unsigned i = 0; i < queue.shadowSplits_.size(); ++i)
		{
			const ShadowBatchQueue& shadowQueue = queue.shadowSplits_[i];

			float multiplier = 1.0f;
			// For directional light cascade splits, adjust depth bias according to the far clip ratio of the splits
			if(i > 0 && queue.light_->GetLightType() == LIGHT_DIRECTIONAL)
			{
				multiplier =
					Max(shadowQueue.shadowCamera_->GetFarClip() / queue.shadowSplits_[0].shadowCamera_->GetFarClip(),1.0f);
				multiplier = 1.0f + (multiplier - 1.0f) * queue.light_->GetShadowCascade().biasAutoAdjust_;
				// Quantize multiplier to prevent creation of too many rasterizer states on D3D11
				multiplier = (int)(multiplier * 10.0f) / 10.0f;
			}

			// Perform further modification of depth bias on OpenGL ES, as shadow calculations' precision is limited
			float addition = 0.0f;

			graphics_->SetDepthBias(multiplier * parameters.constantBias_ + addition,multiplier * parameters.slopeScaledBias_);

			if(!shadowQueue.shadowBatches_.IsEmpty())
			{
				graphics_->SetViewport(shadowQueue.shadowViewport_);
				shadowQueue.shadowBatches_.Draw(this,shadowQueue.shadowCamera_,false,false,true);
			}
		}

		renderer_->ApplyShadowMapFilter(this,shadowMap);


		// reset some parameters
		graphics_->SetColorWrite(true);
		graphics_->SetDepthBias(0.0f,0.0f);
	}

	YumeRenderable* YumeRenderView::GetDepthStencil(YumeRenderable* renderTarget)
	{
		// If using the backbuffer, return the backbuffer depth-stencil
		if(!renderTarget)
			return 0;
		// Then check for linked depth-stencil
		YumeRenderable* depthStencil = renderTarget->GetLinkedDepthStencil();
		// Finally get one from Renderer
		if(!depthStencil)
			depthStencil = renderer_->GetDepthStencil(renderTarget->GetWidth(),renderTarget->GetHeight());
		return depthStencil;
	}

	YumeRenderable* YumeRenderView::GetRenderSurfaceFromTexture(YumeTexture* texture,CubeMapFace face)
	{
		if(!texture)
			return 0;


		if(texture->GetType() == YumeTexture2D::GetTypeStatic())
			return static_cast<YumeTexture2D*>(texture)->GetRenderSurface();
		else if(texture->GetType() == YumeTextureCube::GetTypeStatic())
			return static_cast<YumeTextureCube*>(texture)->GetRenderSurface(face);
		else
			return 0;
	}

	YumeTexture* YumeRenderView::FindNamedTexture(const YumeString& name,bool isRenderTarget,bool isVolumeMap)
	{
		// Check rendertargets first
		YumeHash nameHash(name);
		if(renderTargets_.find(nameHash) != renderTargets_.end())
			return renderTargets_[nameHash];

		YumeResourceManager* rm_= gYume->pResourceManager;

		YumeTexture* texture = rm_->RetrieveResource<YumeTexture2D>(name);
		if(!texture)
			texture = rm_->RetrieveResource<YumeTextureCube>(name);
		if(!texture)
			texture = rm_->RetrieveResource<YumeTexture3D>(name);
		if(texture)
			return texture;

		if(!isRenderTarget)
		{
			if(GetExtension(name) == ".xml")
			{
				if(isVolumeMap)
					return rm_->PrepareResource<YumeTexture3D>(name);
				else
					return rm_->PrepareResource<YumeTextureCube>(name);
			}
			else
				return rm_->PrepareResource<YumeTexture2D>(name);
		}

		return 0;
	}
}

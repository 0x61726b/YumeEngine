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
#include "YumeBatch.h"

#include "YumeCamera.h"
#include "YumeGeometry.h"
#include "YumeRHI.h"
#include "YumeMaterial.h"
#include "YumeRenderer.h"
#include "YumeShaderVariation.h"
#include "YumeRenderPass.h"
#include "YumeVertexBuffer.h"
#include "YumeRenderView.h"
#include "YumeTexture2D.h"
#include "Scene/YumeSceneNode.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeSortAlgorithms.h"
namespace YumeEngine
{

	inline bool CompareBatchesState(Batch* lhs,Batch* rhs)
	{
		if(lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if(lhs->sortKey_ != rhs->sortKey_)
			return lhs->sortKey_ < rhs->sortKey_;
		else
			return lhs->distance_ < rhs->distance_;
	}

	inline bool CompareBatchesFrontToBack(Batch* lhs,Batch* rhs)
	{
		if(lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if(lhs->distance_ != rhs->distance_)
			return lhs->distance_ < rhs->distance_;
		else
			return lhs->sortKey_ < rhs->sortKey_;
	}

	inline bool CompareBatchesBackToFront(Batch* lhs,Batch* rhs)
	{
		if(lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if(lhs->distance_ != rhs->distance_)
			return lhs->distance_ > rhs->distance_;
		else
			return lhs->sortKey_ < rhs->sortKey_;
	}

	inline bool CompareInstancesFrontToBack(const InstanceData& lhs,const InstanceData& rhs)
	{
		return lhs.distance_ < rhs.distance_;
	}

	inline bool CompareBatchGroupOrder(BatchGroup* lhs,BatchGroup* rhs)
	{
		return lhs->renderOrder_ < rhs->renderOrder_;
	}

	void CalculateShadowMatrix(Matrix4& dest,LightBatchQueue* queue,unsigned split,YumeRenderer* renderer,const Vector3& translation)
	{
		YumeCamera* shadowCamera = queue->shadowSplits_[split].shadowCamera_;
		const IntRect& viewport = queue->shadowSplits_[split].shadowViewport_;

		Matrix3x4 posAdjust(translation,Quaternion::IDENTITY,1.0f);
		Matrix3x4 shadowView(shadowCamera->GetView());
		Matrix4 shadowProj(shadowCamera->GetProjection());
		Matrix4 texAdjust(Matrix4::IDENTITY);

		YumeTexture2D* shadowMap = queue->shadowMap_;
		if(!shadowMap)
			return;

		float width = (float)shadowMap->GetWidth();
		float height = (float)shadowMap->GetHeight();

		Vector3 offset(
			(float)viewport.left_ / width,
			(float)viewport.top_ / height,
			0.0f
			);

		Vector3 scale(
			0.5f * (float)viewport.Width() / width,
			0.5f * (float)viewport.Height() / height,
			1.0f
			);

		YumeRHI* rhi_ = gYume->pRHI;
		// Add pixel-perfect offset if needed by the graphics API
		const Vector2& pixelUVOffset = rhi_->GetPixelUVOffset();
		offset.x_ += scale.x_ + pixelUVOffset.x_ / width;
		offset.y_ += scale.y_ + pixelUVOffset.y_ / height;

		if(gYume->pEngine->GetRendererName() == "OpenGL")
		{
			offset.z_ = 0.5f;
			scale.z_ = 0.5f;
			offset.y_ = 1.0f - offset.y_;
		}
		else
		{
			scale.y_ = -scale.y_;
		}

		// If using 4 shadow samples, offset the position diagonally by half pixel
		if(renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
		{
			offset.x_ -= 0.5f / width;
			offset.y_ -= 0.5f / height;
		}
		texAdjust.SetTranslation(offset);
		texAdjust.SetScale(scale);

		dest = texAdjust * shadowProj * shadowView * posAdjust;
	}

	void CalculateSpotMatrix(Matrix4& dest,YumeLight* light,const Vector3& translation)
	{
		YumeSceneNode* lightNode = light->GetNode();
		Matrix3x4 posAdjust(translation,Quaternion::IDENTITY,1.0f);
		Matrix3x4 spotView = Matrix3x4(lightNode->GetWorldPosition(),lightNode->GetWorldRotation(),1.0f).Inverse();
		Matrix4 spotProj(Matrix4::ZERO);
		Matrix4 texAdjust(Matrix4::IDENTITY);

		// Make the projected light slightly smaller than the shadow map to prevent light spill
		float h = 1.005f / tanf(light->GetFov() * M_DEGTORAD * 0.5f);
		float w = h / light->GetAspectRatio();
		spotProj.m00_ = w;
		spotProj.m11_ = h;
		spotProj.m22_ = 1.0f / Max(light->GetRange(),M_EPSILON);
		spotProj.m32_ = 1.0f;

		if(gYume->pEngine->GetRendererName() == "OpenGL")
		{
			texAdjust.SetTranslation(Vector3(0.5f,0.5f,0.5f));
			texAdjust.SetScale(Vector3(0.5f,-0.5f,0.5f));
		}
		else
		{
			texAdjust.SetTranslation(Vector3(0.5f,0.5f,0.0f));
			texAdjust.SetScale(Vector3(0.5f,-0.5f,1.0f));
		}

		dest = texAdjust * spotProj * spotView * posAdjust;
	}

	void Batch::CalculateSortKey()
	{
		unsigned shaderID = (unsigned)(
			((*((unsigned*)&vertexShader_) / sizeof(YumeShaderVariation)) + (*((unsigned*)&geometryShader_) / sizeof(YumeShaderVariation)) + (*((unsigned*)&pixelShader_) / sizeof(YumeShaderVariation))) &
			0x3fff);
		if(!isBase_)
			shaderID |= 0x8000;
		if(pass_ && pass_->GetAlphaMask())
			shaderID |= 0x4000;

		unsigned lightQueueID = (unsigned)((*((unsigned*)&lightQueue_) / sizeof(LightBatchQueue)) & 0xffff);
		unsigned materialID = (unsigned)((*((unsigned*)&material_) / sizeof(YumeMaterial)) & 0xffff);
		unsigned geometryID = (unsigned)((*((unsigned*)&geometry_) / sizeof(YumeGeometry)) & 0xffff);

		sortKey_ = (((unsigned long long)shaderID) << 48) | (((unsigned long long)lightQueueID) << 32) |
			(((unsigned long long)materialID) << 16) | geometryID;
	}

	void Batch::Prepare(YumeRenderView* view,YumeCamera* camera,bool setModelTransform,bool allowDepthWrite) const
	{
		if(!vertexShader_ || !pixelShader_)
			return;

		YumeRHI* graphics = view->GetRHI();
		YumeRenderer* renderer = view->GetRenderer();
		YumeSceneNode* cameraNode = camera ? camera->GetNode() : 0;
		YumeLight* light = lightQueue_ ? lightQueue_->light_ : 0;
		YumeTexture2D* shadowMap = lightQueue_ ? lightQueue_->shadowMap_ : 0;

		// Set shaders first. The available shader parameters and their register/uniform positions depend on the currently set shaders
		graphics->SetShaders(vertexShader_,pixelShader_,geometryShader_);

		// Set pass / material-specific renderstates
		if(pass_ && material_)
		{
			BlendMode blend = pass_->GetBlendMode();
			// Turn additive blending into subtract if the light is negative
			if(light && light->IsNegative())
			{
				if(blend == BLEND_ADD)
					blend = BLEND_SUBTRACT;
				else if(blend == BLEND_ADDALPHA)
					blend = BLEND_SUBTRACTALPHA;
			}
			graphics->SetBlendMode(blend);

			bool isShadowPass = pass_->GetIndex() == YumeRenderTechnique::shadowPassIndex;
			renderer->SetCullMode(isShadowPass ? material_->GetShadowCullMode() : material_->GetCullMode(),camera);
			if(!isShadowPass)
			{
				const BiasParameters& depthBias = material_->GetDepthBias();
				graphics->SetDepthBias(depthBias.constantBias_,depthBias.slopeScaledBias_);
			}

			// Use the "least filled" fill mode combined from camera & material
			graphics->SetFillMode((FillMode)(Max(camera->GetFillMode(),material_->GetFillMode())));
			graphics->SetDepthTest(pass_->GetDepthTestMode());
			graphics->SetDepthWrite(pass_->GetDepthWrite() && allowDepthWrite);
		}

		// Set global (per-frame) shader parameters
		if(graphics->NeedParameterUpdate(SP_FRAME,(void*)0))
			view->SetGlobalShaderParameters();

		// Set camera & viewport shader parameters
		unsigned cameraHash = (unsigned)(size_t)camera;
		IntRect viewport = graphics->GetViewport();
		IntVector2 viewSize = IntVector2(viewport.Width(),viewport.Height());
		unsigned viewportHash = (unsigned)(viewSize.x_ | (viewSize.y_ << 16));
		if(graphics->NeedParameterUpdate(SP_CAMERA,reinterpret_cast<const void*>(cameraHash + viewportHash)))
		{
			view->SetCameraShaderParameters(camera,true);
			// During renderpath commands the G-Buffer or viewport texture is assumed to always be viewport-sized
			view->SetGBufferShaderParameters(viewSize,IntRect(0,0,viewSize.x_,viewSize.y_));
		}

		// Set model or skinning transforms
		if(setModelTransform && graphics->NeedParameterUpdate(SP_OBJECT,worldTransform_))
		{
			if(geometryType_ == GEOM_SKINNED)
			{
				graphics->SetShaderParameter(VSP_SKINMATRICES,reinterpret_cast<const float*>(worldTransform_),
					12 * numWorldTransforms_);
			}
			else
			{
				graphics->SetShaderParameter(VSP_MODEL,*worldTransform_);
				graphics->SetShaderParameter(VSP_WORLDVIEW,*worldTransform_ * camera->GetView());
			}

			// Set the orientation for billboards, either from the object itself or from the camera
			if(geometryType_ == GEOM_BILLBOARD)
			{
				if(numWorldTransforms_ > 1)
					graphics->SetShaderParameter(VSP_BILLBOARDROT,worldTransform_[1].RotationMatrix());
				else
					graphics->SetShaderParameter(VSP_BILLBOARDROT,cameraNode->GetWorldRotation().RotationMatrix());
			}
		}

		// Set zone-related shader parameters
		BlendMode blend = graphics->GetBlendMode();
		// If the pass is additive, override fog color to black so that shaders do not need a separate additive path
		bool overrideFogColorToBlack = blend == BLEND_ADD || blend == BLEND_ADDALPHA;
		unsigned zoneHash = (unsigned)(size_t)zone_;
		if(overrideFogColorToBlack)
			zoneHash += 0x80000000;
		if(zone_ && graphics->NeedParameterUpdate(SP_ZONE,reinterpret_cast<const void*>(zoneHash)))
		{
			graphics->SetShaderParameter(VSP_AMBIENTSTARTCOLOR,zone_->GetAmbientStartColor());
			graphics->SetShaderParameter(VSP_AMBIENTENDCOLOR,
				zone_->GetAmbientEndColor().ToVector4() - zone_->GetAmbientStartColor().ToVector4());

			const BoundingBox& box = zone_->GetBoundingBox();
			Vector3 boxSize = box.Size();
			Matrix3x4 adjust(Matrix3x4::IDENTITY);
			adjust.SetScale(Vector3(1.0f / boxSize.x_,1.0f / boxSize.y_,1.0f / boxSize.z_));
			adjust.SetTranslation(Vector3(0.5f,0.5f,0.5f));
			Matrix3x4 zoneTransform = adjust * zone_->GetInverseWorldTransform();
			graphics->SetShaderParameter(VSP_ZONE,zoneTransform);

			graphics->SetShaderParameter(PSP_AMBIENTCOLOR,zone_->GetAmbientColor());
			graphics->SetShaderParameter(PSP_FOGCOLOR,overrideFogColorToBlack ? YumeColor::BLACK : zone_->GetFogColor());

			float farClip = camera->GetFarClip();
			float fogStart = Min(zone_->GetFogStart(),farClip);
			float fogEnd = Min(zone_->GetFogEnd(),farClip);
			if(fogStart >= fogEnd * (1.0f - M_LARGE_EPSILON))
				fogStart = fogEnd * (1.0f - M_LARGE_EPSILON);
			float fogRange = Max(fogEnd - fogStart,M_EPSILON);
			Vector4 fogParams(fogEnd / farClip,farClip / fogRange,0.0f,0.0f);

			YumeSceneNode* zoneNode = zone_->GetNode();
			if(zone_->GetHeightFog() && zoneNode)
			{
				Vector3 worldFogHeightVec = zoneNode->GetWorldTransform() * Vector3(0.0f,zone_->GetFogHeight(),0.0f);
				fogParams.z_ = worldFogHeightVec.y_;
				fogParams.w_ = zone_->GetFogHeightScale() / Max(zoneNode->GetWorldScale().y_,M_EPSILON);
			}

			graphics->SetShaderParameter(PSP_FOGPARAMS,fogParams);
		}

		// Set light-related shader parameters
		if(lightQueue_)
		{
			if(light && graphics->NeedParameterUpdate(SP_LIGHT,lightQueue_))
			{
				// Deferred light volume batches operate in a camera-centered space. Detect from material, zone & pass all being null
				bool isLightVolume = !material_ && !pass_ && !zone_;

				Matrix3x4 cameraEffectiveTransform = camera->GetEffectiveWorldTransform();
				Vector3 cameraEffectivePos = cameraEffectiveTransform.Translation();

				YumeSceneNode* lightNode = light->GetNode();
				Matrix3 lightWorldRotation = lightNode->GetWorldRotation().RotationMatrix();

				graphics->SetShaderParameter(VSP_LIGHTDIR,lightWorldRotation * Vector3::BACK);

				float atten = 1.0f / Max(light->GetRange(),M_EPSILON);
				graphics->SetShaderParameter(VSP_LIGHTPOS,Vector4(lightNode->GetWorldPosition(),atten));

				if(graphics->HasShaderParameter(VSP_LIGHTMATRICES))
				{
					switch(light->GetLightType())
					{
					case LIGHT_DIRECTIONAL:
					{
						Matrix4 shadowMatrices[MAX_CASCADE_SPLITS];
						unsigned numSplits = Min(MAX_CASCADE_SPLITS,lightQueue_->shadowSplits_.size());

						for(unsigned i = 0; i < numSplits; ++i)
							CalculateShadowMatrix(shadowMatrices[i],lightQueue_,i,renderer,Vector3::ZERO);

						graphics->SetShaderParameter(VSP_LIGHTMATRICES,shadowMatrices[0].Data(),16 * numSplits);
					}
					break;

					case LIGHT_SPOT:
					{
						Matrix4 shadowMatrices[2];

						CalculateSpotMatrix(shadowMatrices[0],light,Vector3::ZERO);
						bool isShadowed = shadowMap && graphics->HasTextureUnit(TU_SHADOWMAP);
						if(isShadowed)
							CalculateShadowMatrix(shadowMatrices[1],lightQueue_,0,renderer,Vector3::ZERO);

						graphics->SetShaderParameter(VSP_LIGHTMATRICES,shadowMatrices[0].Data(),isShadowed ? 32 : 16);
					}
					break;

					case LIGHT_POINT:
					{
						Matrix4 lightVecRot(lightNode->GetWorldRotation().RotationMatrix());

						if(gYume->pEngine->GetRendererName() == "OpenGL")
						{
							graphics->SetShaderParameter(VSP_LIGHTMATRICES,lightVecRot.Data(),16);
						}
						else
						{
							graphics->SetShaderParameter(VSP_LIGHTMATRICES,lightVecRot.Data(),12);
						}
					}
					break;
					}
				}

				float fade = 1.0f;
				float fadeEnd = light->GetDrawDistance();
				float fadeStart = light->GetFadeDistance();

				// Do fade calculation for light if both fade & draw distance defined
				if(light->GetLightType() != LIGHT_DIRECTIONAL && fadeEnd > 0.0f && fadeStart > 0.0f && fadeStart < fadeEnd)
					fade = Min(1.0f - (light->GetDistance() - fadeStart) / (fadeEnd - fadeStart),1.0f);

				// Negative lights will use subtract blending, so write absolute RGB values to the shader parameter
				graphics->SetShaderParameter(PSP_LIGHTCOLOR,YumeColor(light->GetEffectiveColor().Abs(),
					light->GetEffectiveSpecularIntensity()) * fade);
				graphics->SetShaderParameter(PSP_LIGHTDIR,lightWorldRotation * Vector3::BACK);
				graphics->SetShaderParameter(PSP_LIGHTPOS,
					Vector4((isLightVolume ? (lightNode->GetWorldPosition() - cameraEffectivePos) : lightNode->GetWorldPosition()),
					atten));
				Vector4 lightScreen = Vector4((isLightVolume ? (lightNode->GetWorldPosition() - cameraEffectivePos) : lightNode->GetWorldPosition()),
					atten);
				Matrix4 viewproj = (camera->GetProjection() * camera->GetView());
				lightScreen = viewproj * lightScreen;


				graphics->SetShaderParameter(PSP_LIGHTPOSSCREEN,viewproj);
				if(graphics->HasShaderParameter(PSP_LIGHTMATRICES))
				{
					switch(light->GetLightType())
					{
					case LIGHT_DIRECTIONAL:
					{
						Matrix4 shadowMatrices[MAX_CASCADE_SPLITS];
						unsigned numSplits = Min(MAX_CASCADE_SPLITS,lightQueue_->shadowSplits_.size());

						for(unsigned i = 0; i < numSplits; ++i)
						{
							CalculateShadowMatrix(shadowMatrices[i],lightQueue_,i,renderer,isLightVolume ? cameraEffectivePos :
								Vector3::ZERO);
						}
						graphics->SetShaderParameter(PSP_LIGHTMATRICES,shadowMatrices[0].Data(),16 * numSplits);
					}
					break;

					case LIGHT_SPOT:
					{
						Matrix4 shadowMatrices[2];

						CalculateSpotMatrix(shadowMatrices[0],light,cameraEffectivePos);
						bool isShadowed = lightQueue_->shadowMap_ != 0;
						if(isShadowed)
						{
							CalculateShadowMatrix(shadowMatrices[1],lightQueue_,0,renderer,isLightVolume ? cameraEffectivePos :
								Vector3::ZERO);
						}

						graphics->SetShaderParameter(PSP_LIGHTMATRICES,shadowMatrices[0].Data(),isShadowed ? 32 : 16);
					}
					break;

					case LIGHT_POINT:
					{
						Matrix4 lightVecRot(lightNode->GetWorldRotation().RotationMatrix());
						// HLSL compiler will pack the parameters as if the matrix is only 3x4, so must be careful to not overwrite
						// the next parameter
						if(gYume->pEngine->GetRendererName() == "OpenGL")
						{
							graphics->SetShaderParameter(PSP_LIGHTMATRICES,lightVecRot.Data(),16);
						}
						else
						{
							graphics->SetShaderParameter(PSP_LIGHTMATRICES,lightVecRot.Data(),12);
						}
					}
					break;
					}
				}



				// Set shadow mapping shader parameters
				if(shadowMap)
				{
					{
						// Calculate point light shadow sampling offsets (unrolled cube map)
						unsigned faceWidth = (unsigned)(shadowMap->GetWidth() / 2);
						unsigned faceHeight = (unsigned)(shadowMap->GetHeight() / 3);
						float width = (float)shadowMap->GetWidth();
						float height = (float)shadowMap->GetHeight();

						float mulX,mulY,addX,addY;

						if(gYume->pEngine->GetRendererName() == "OpenGL")
						{
							mulX = (float)(faceWidth - 3) / width;
							mulY = (float)(faceHeight - 3) / height;
							addX = 1.5f / width;
							addY = 1.5f / height;
						}
						else
						{
							mulX = (float)(faceWidth - 4) / width;
							mulY = (float)(faceHeight - 4) / height;
							addX = 2.5f / width;
							addY = 2.5f / height;
						}
						// If using 4 shadow samples, offset the position diagonally by half pixel
						if(renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
						{
							addX -= 0.5f / width;
							addY -= 0.5f / height;
						}
						graphics->SetShaderParameter(PSP_SHADOWCUBEADJUST,Vector4(mulX,mulY,addX,addY));
					}

				{
					// Calculate shadow camera depth parameters for point light shadows and shadow fade parameters for
					//  directional light shadows, stored in the same uniform
					YumeCamera* shadowCamera = lightQueue_->shadowSplits_[0].shadowCamera_;
					float nearClip = shadowCamera->GetNearClip();
					float farClip = shadowCamera->GetFarClip();
					float q = farClip / (farClip - nearClip);
					float r = -q * nearClip;

					const CascadeParameters& parameters = light->GetShadowCascade();
					float viewFarClip = camera->GetFarClip();
					float shadowRange = parameters.GetShadowRange();
					float fadeStart = parameters.fadeStart_ * shadowRange / viewFarClip;
					float fadeEnd = shadowRange / viewFarClip;
					float fadeRange = fadeEnd - fadeStart;

					graphics->SetShaderParameter(PSP_SHADOWDEPTHFADE,Vector4(q,r,fadeStart,1.0f / fadeRange));
				}

				{
					float intensity = light->GetShadowIntensity();
					float fadeStart = light->GetShadowFadeDistance();
					float fadeEnd = light->GetShadowDistance();
					if(fadeStart > 0.0f && fadeEnd > 0.0f && fadeEnd > fadeStart)
						intensity =
						Lerp(intensity,1.0f,Clamp((light->GetDistance() - fadeStart) / (fadeEnd - fadeStart),0.0f,1.0f));
					float pcfValues = (1.0f - intensity);
					float samples = 1.0f;
					if(renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
						samples = 4.0f;
					graphics->SetShaderParameter(PSP_SHADOWINTENSITY,Vector4(pcfValues / samples,intensity,0.0f,0.0f));
				}

				float sizeX = 1.0f / (float)shadowMap->GetWidth();
				float sizeY = 1.0f / (float)shadowMap->GetHeight();
				graphics->SetShaderParameter(PSP_SHADOWMAPINVSIZE,Vector2(sizeX,sizeY));

				Vector4 lightSplits(M_LARGE_VALUE,M_LARGE_VALUE,M_LARGE_VALUE,M_LARGE_VALUE);
				if(lightQueue_->shadowSplits_.size() > 1)
					lightSplits.x_ = lightQueue_->shadowSplits_[0].farSplit_ / camera->GetFarClip();
				if(lightQueue_->shadowSplits_.size() > 2)
					lightSplits.y_ = lightQueue_->shadowSplits_[1].farSplit_ / camera->GetFarClip();
				if(lightQueue_->shadowSplits_.size() > 3)
					lightSplits.z_ = lightQueue_->shadowSplits_[2].farSplit_ / camera->GetFarClip();

				graphics->SetShaderParameter(PSP_SHADOWSPLITS,lightSplits);

				if(graphics->HasShaderParameter(PSP_VSMSHADOWPARAMS))
					graphics->SetShaderParameter(PSP_VSMSHADOWPARAMS,renderer->GetVSMShadowParameters());
				}
			}
			else if(lightQueue_->vertexLights_.size() && graphics->HasShaderParameter(VSP_VERTEXLIGHTS) &&
				graphics->NeedParameterUpdate(SP_LIGHT,lightQueue_))
			{
				Vector4 vertexLights[MAX_VERTEX_LIGHTS * 3];
				const YumePodVector<YumeLight*>::type& lights = lightQueue_->vertexLights_;

				for(unsigned i = 0; i < lights.size(); ++i)
				{
					YumeLight* vertexLight = lights[i];
					YumeSceneNode* vertexLightNode = vertexLight->GetNode();
					LightType type = vertexLight->GetLightType();

					// Attenuation
					float invRange,cutoff,invCutoff;
					if(type == LIGHT_DIRECTIONAL)
						invRange = 0.0f;
					else
						invRange = 1.0f / Max(vertexLight->GetRange(),M_EPSILON);
					if(type == LIGHT_SPOT)
					{
						cutoff = Cos(vertexLight->GetFov() * 0.5f);
						invCutoff = 1.0f / (1.0f - cutoff);
					}
					else
					{
						cutoff = -1.0f;
						invCutoff = 1.0f;
					}

					// Color
					float fade = 1.0f;
					float fadeEnd = vertexLight->GetDrawDistance();
					float fadeStart = vertexLight->GetFadeDistance();

					// Do fade calculation for light if both fade & draw distance defined
					if(vertexLight->GetLightType() != LIGHT_DIRECTIONAL && fadeEnd > 0.0f && fadeStart > 0.0f && fadeStart < fadeEnd)
						fade = Min(1.0f - (vertexLight->GetDistance() - fadeStart) / (fadeEnd - fadeStart),1.0f);

					YumeColor color = vertexLight->GetEffectiveColor() * fade;
					vertexLights[i * 3] = Vector4(color.r_,color.g_,color.b_,invRange);

					// Direction
					vertexLights[i * 3 + 1] = Vector4(-(vertexLightNode->GetWorldDirection()),cutoff);

					// Position
					vertexLights[i * 3 + 2] = Vector4(vertexLightNode->GetWorldPosition(),invCutoff);
				}

				graphics->SetShaderParameter(VSP_VERTEXLIGHTS,vertexLights[0].Data(),lights.size() * 3 * 4);
			}
		}

		// Set material-specific shader parameters and textures
		if(material_)
		{
			if(graphics->NeedParameterUpdate(SP_MATERIAL,reinterpret_cast<const void*>(material_->GetShaderParameterHash())))
			{
				const YumeMap<YumeHash,MaterialShaderParameter>::type& parameters = material_->GetShaderParameters();
				for(YumeMap<YumeHash,MaterialShaderParameter>::const_iterator i = parameters.begin(); i != parameters.end(); ++i)
				{
					YumeString n = i->second.name_;
					graphics->SetShaderParameter(i->first,i->second.value_);
				}
			}

			const YumeMap<TextureUnit,SharedPtr<YumeTexture> >::type& textures = material_->GetTextures();
			for(YumeMap<TextureUnit,SharedPtr<YumeTexture> >::const_iterator i = textures.begin(); i != textures.end(); ++i)
			{
				if(graphics->HasTextureUnit(i->first))
					graphics->SetTexture(i->first,i->second);
			}
		}

		// Set light-related textures
		if(light)
		{
			if(shadowMap && graphics->HasTextureUnit(TU_SHADOWMAP))
				graphics->SetTexture(TU_SHADOWMAP,shadowMap);
			if(graphics->HasTextureUnit(TU_LIGHTRAMP))
			{
				YumeTexture* rampTexture = light->GetRampTexture();
				if(!rampTexture)
					rampTexture = renderer->GetDefaultLightRamp();
				graphics->SetTexture(TU_LIGHTRAMP,rampTexture);
			}
			if(graphics->HasTextureUnit(TU_LIGHTSHAPE))
			{
				YumeTexture* shapeTexture = light->GetShapeTexture();
				if(!shapeTexture && light->GetLightType() == LIGHT_SPOT)
					shapeTexture = renderer->GetDefaultLightSpot();
				graphics->SetTexture(TU_LIGHTSHAPE,shapeTexture);
			}
		}
		if(zone_ && graphics->HasTextureUnit(TU_ZONE))
			graphics->SetTexture(TU_ZONE,zone_->GetZoneTexture());
	}

	void Batch::Draw(YumeRenderView* view,YumeCamera* camera,bool allowDepthWrite) const
	{
		if(!geometry_->IsEmpty())
		{
			Prepare(view,camera,true,allowDepthWrite);
			geometry_->Draw(view->GetRHI());
		}
	}

	void BatchGroup::SetTransforms(void* lockedData,unsigned& freeIndex)
	{
		// Do not use up buffer space if not going to draw as instanced
		if(geometryType_ != GEOM_INSTANCED)
			return;

		startIndex_ = freeIndex;
		Matrix3x4* dest = (Matrix3x4*)lockedData;
		dest += freeIndex;

		for(unsigned i = 0; i < instances_.size(); ++i)
			*dest++ = *instances_[i].worldTransform_;

		freeIndex += instances_.size();
	}

	void BatchGroup::Draw(YumeRenderView* view,YumeCamera* camera,bool allowDepthWrite) const
	{
		YumeRHI* graphics = view->GetRHI();
		YumeRenderer* renderer = view->GetRenderer();

		if(instances_.size() && !geometry_->IsEmpty())
		{
			// Draw as individual objects if instancing not supported or could not fill the instancing buffer
			YumeVertexBuffer* instanceBuffer = renderer->GetInstancingBuffer();
			if(!instanceBuffer || geometryType_ != GEOM_INSTANCED || startIndex_ == M_MAX_UNSIGNED)
			{
				Batch::Prepare(view,camera,false,allowDepthWrite);



				graphics->SetIndexBuffer(geometry_->GetIndexBuffer());
				graphics->SetVertexBuffers(geometry_->GetVertexBuffers(),geometry_->GetVertexElementMasks());

				for(unsigned i = 0; i < instances_.size(); ++i)
				{
					if(graphics->NeedParameterUpdate(SP_OBJECT,instances_[i].worldTransform_))
					{
						graphics->SetShaderParameter(VSP_MODEL,*instances_[i].worldTransform_);
						graphics->SetShaderParameter(VSP_WORLDVIEW,*worldTransform_ * camera->GetView());
					}

					graphics->Draw(geometry_->GetPrimitiveType(),geometry_->GetIndexStart(),geometry_->GetIndexCount(),
						geometry_->GetVertexStart(),geometry_->GetVertexCount());
				}
			}
			else
			{
				Batch::Prepare(view,camera,false,allowDepthWrite);

				// Get the geometry vertex buffers, then add the instancing stream buffer
				// Hack: use a const_cast to avoid dynamic allocation of new temp vectors
				YumeVector<SharedPtr<YumeVertexBuffer> >::type& vertexBuffers = const_cast<YumeVector<SharedPtr<YumeVertexBuffer> >::type&>(
					geometry_->GetVertexBuffers());
				YumeVector<unsigned>::type& elementMasks = const_cast<YumeVector<unsigned>::type&>(geometry_->GetVertexElementMasks());
				vertexBuffers.push_back(SharedPtr<YumeVertexBuffer>(instanceBuffer));
				elementMasks.push_back(instanceBuffer->GetElementMask());

				graphics->SetIndexBuffer(geometry_->GetIndexBuffer());
				graphics->SetVertexBuffers(vertexBuffers,elementMasks,startIndex_);
				graphics->DrawInstanced(geometry_->GetPrimitiveType(),geometry_->GetIndexStart(),geometry_->GetIndexCount(),
					geometry_->GetVertexStart(),geometry_->GetVertexCount(),instances_.size());

				// Remove the instancing buffer & element mask now
				vertexBuffers.Pop();
				elementMasks.Pop();
			}
		}
	}

	unsigned BatchGroupKey::ToHash() const
	{
		return (unsigned)((size_t)zone_ / sizeof(YumeRendererEnvironment) + (size_t)lightQueue_ / sizeof(LightBatchQueue) + (size_t)pass_ / sizeof(YumeRenderPass) +
			(size_t)material_ / sizeof(YumeMaterial) + (size_t)geometry_ / sizeof(YumeGeometry)) + renderOrder_;
	}

	void BatchQueue::Clear(int maxSortedInstances)
	{
		batches_.clear();
		sortedBatches_.clear();
		batchGroups_.clear();
		maxSortedInstances_ = (unsigned)maxSortedInstances;
	}

	void BatchQueue::SortBackToFront()
	{
		sortedBatches_.resize(batches_.size());

		for(unsigned i = 0; i < batches_.size(); ++i)
			sortedBatches_[i] = &batches_[i];

		Sort(sortedBatches_.begin(),sortedBatches_.end(),CompareBatchesBackToFront);

		sortedBatchGroups_.resize(batchGroups_.size());

		unsigned index = 0;
		for(YumeMap<BatchGroupKey,BatchGroup>::iterator i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
			sortedBatchGroups_[index++] = &i->second;

		Sort(sortedBatchGroups_.begin(),sortedBatchGroups_.end(),CompareBatchGroupOrder);
	}

	void BatchQueue::SortFrontToBack()
	{
		sortedBatches_.clear();

		for(unsigned i = 0; i < batches_.size(); ++i)
			sortedBatches_.push_back(&batches_[i]);

		SortFrontToBack2Pass(sortedBatches_);

		// Sort each group front to back
		for(YumeMap<BatchGroupKey,BatchGroup>::iterator i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
		{
			if(i->second.instances_.size() <= maxSortedInstances_)
			{
				Sort(i->second.instances_.begin(),i->second.instances_.end(),CompareInstancesFrontToBack);
				if(i->second.instances_.size())
					i->second.distance_ = i->second.instances_[0].distance_;
			}
			else
			{
				float minDistance = M_INFINITY;
				for(YumeVector<InstanceData>::const_iterator j = i->second.instances_.begin(); j != i->second.instances_.end(); ++j)
					minDistance = Min(minDistance,j->distance_);
				i->second.distance_ = minDistance;
			}
		}

		sortedBatchGroups_.resize(batchGroups_.size());

		unsigned index = 0;
		for(YumeMap<BatchGroupKey,BatchGroup>::iterator i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
			sortedBatchGroups_[index++] = &i->second;

		SortFrontToBack2Pass(reinterpret_cast<YumePodVector<Batch*>::type&>(sortedBatchGroups_));
	}

	void BatchQueue::SortFrontToBack2Pass(YumePodVector<Batch*>::type& batches)
	{
		// For desktop, first sort by distance and remap shader/material/geometry IDs in the sort key
		Sort(batches.begin(),batches.end(),CompareBatchesFrontToBack);

		unsigned freeShaderID = 0;
		unsigned short freeMaterialID = 0;
		unsigned short freeGeometryID = 0;

		for(YumeVector<Batch*>::iterator i = batches.begin(); i != batches.end(); ++i)
		{
			Batch* batch = *i;

			unsigned shaderID = (unsigned)(batch->sortKey_ >> 32);
			YumeMap<unsigned,unsigned>::const_iterator j = shaderRemapping_.find(shaderID);
			if(j != shaderRemapping_.end())
				shaderID = j->second;
			else
			{
				shaderID = shaderRemapping_[shaderID] = freeShaderID | (shaderID & 0xc0000000);
				++freeShaderID;
			}

			unsigned short materialID = (unsigned short)(batch->sortKey_ & 0xffff0000);
			YumeMap<unsigned short,unsigned short>::const_iterator k = materialRemapping_.find(materialID);
			if(k != materialRemapping_.end())
				materialID = k->second;
			else
			{
				materialID = materialRemapping_[materialID] = freeMaterialID;
				++freeMaterialID;
			}

			unsigned short geometryID = (unsigned short)(batch->sortKey_ & 0xffff);
			YumeMap<unsigned short,unsigned short>::const_iterator l = geometryRemapping_.find(geometryID);
			if(l != geometryRemapping_.end())
				geometryID = l->second;
			else
			{
				geometryID = geometryRemapping_[geometryID] = freeGeometryID;
				++freeGeometryID;
			}

			batch->sortKey_ = (((unsigned long long)shaderID) << 32) | (((unsigned long long)materialID) << 16) | geometryID;
		}

		shaderRemapping_.clear();
		materialRemapping_.clear();
		geometryRemapping_.clear();

		// Finally sort again with the rewritten ID's
		Sort(batches.begin(),batches.end(),CompareBatchesState);
	}

	void BatchQueue::SetTransforms(void* lockedData,unsigned& freeIndex)
	{
		for(YumeMap<BatchGroupKey,BatchGroup>::iterator i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
			i->second.SetTransforms(lockedData,freeIndex);
	}

	void BatchQueue::Draw(YumeRenderView* view,YumeCamera* camera,bool markToStencil,bool usingLightOptimization,bool allowDepthWrite) const
	{
		YumeRHI* graphics = view->GetRHI();
		YumeRenderer* renderer = view->GetRenderer();

		// If View has set up its own light optimizations, do not disturb the stencil/scissor test settings
		if(!usingLightOptimization)
		{
			graphics->SetScissorTest(false);

			// During G-buffer rendering, mark opaque pixels' lightmask to stencil buffer if requested
			if(!markToStencil)
				graphics->SetStencilTest(false);
		}

		// Instanced
		for(YumeVector<BatchGroup*>::const_iterator i = sortedBatchGroups_.begin(); i != sortedBatchGroups_.end(); ++i)
		{
			BatchGroup* group = *i;
			if(markToStencil)
				graphics->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,group->lightMask_);

			group->Draw(view,camera,allowDepthWrite);
		}
		// Non-instanced
		for(YumeVector<Batch*>::const_iterator i = sortedBatches_.begin(); i != sortedBatches_.end(); ++i)
		{
			Batch* batch = *i;
			if(markToStencil)
				graphics->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,batch->lightMask_);
			if(!usingLightOptimization)
			{
				// If drawing an alpha batch, we can optimize fillrate by scissor test
				if(!batch->isBase_ && batch->lightQueue_)
					renderer->OptimizeLightByScissor(batch->lightQueue_->light_,camera);
				else
					graphics->SetScissorTest(false);
			}

			batch->Draw(view,camera,allowDepthWrite);
		}
	}

	unsigned BatchQueue::GetNumInstances() const
	{
		unsigned total = 0;

		for(YumeMap<BatchGroupKey,BatchGroup>::const_iterator i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
		{
			if(i->second.geometryType_ == GEOM_INSTANCED)
				total += i->second.instances_.size();
		}

		return total;
	}

}

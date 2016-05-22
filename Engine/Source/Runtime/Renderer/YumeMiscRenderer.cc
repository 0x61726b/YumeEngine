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
#include "YumeMiscRenderer.h"

#include "LPVRendererTest.h"

#include "YumeRHI.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"

#include "YumePostProcess.h"
#include "YumeTimer.h"

#include "Engine/YumeEngine.h"

#include "RenderPass.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Light.h"
#include "Material.h"
#include "LightPropagationVolume.h"
#include "GlobalIlluminationVolume.h"
#include "StaticModel.h"

#include "YumeTextureCube.h"

#include "Logging/logging.h"
using namespace DirectX;


#define DISABLE_CALLS



#define CAMERA_MOVE_SPEED 75




namespace YumeEngine
{
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

	struct SimpleVertex
	{
		DirectX::XMFLOAT3 V;
	};

	YumeMiscRenderer::YumeMiscRenderer()
		:
		zNear(.2f),
		zFar(100000.f),
		giEnabled_(true),
		updateRsm_(true),
		rsmSize(1024),
		num_propagations_(64),
		disableFrustumCull_(true),
		updateCubemap_(true),
		currentAdaptedLuminance_(true),
		backbufferModified_(false),
		usedResolve_(false),
		currentRenderTarget_(0),
		cameraMoveSpeed_(CAMERA_MOVE_SPEED)
	{
		rhi_ = gYume->pRHI ;


		gYume->pEngine->AddListener(this);

		scene_ = YumeAPINew Scene;



		giParams_.DebugView = false;
		giParams_.LPVFlux = 4;
		giParams_.Scale = 1.0f;

		bbMin = XMFLOAT3(M_INFINITY,M_INFINITY,M_INFINITY);
		bbMax = XMFLOAT3(-M_INFINITY,-M_INFINITY,-M_INFINITY);
	}

	YumeMiscRenderer::~YumeMiscRenderer()
	{
	}

	void YumeMiscRenderer::Initialize(GISolution gi)
	{
		float width = gYume->pRHI->GetWidth();
		float height = gYume->pRHI->GetHeight();

		camera_ = YumeAPINew FirstPersonCamera(width / height,60.0f * M_DEGTORAD,zNear,zFar);
		camera_->SetLookAt(XMFLOAT3(0,7,-26),XMFLOAT3(0,9,0),XMFLOAT3(0,1,0));


		defaultPass_ = YumeAPINew RenderPass;

		

		gi_ = gi;

		if(gi_ == LPV || gi_ == Dyn_Lpv)
		{
			defaultPass_->Load("RenderCalls/ReflectiveShadowMap.xml");
			defaultPass_->Load("RenderCalls/LightPropagationVolume.xml");
			defaultPass_->Load("RenderCalls/DeferredGI.xml");


			giVolume_ = new LightPropagationVolume();

			giVolume_->Create(32);

			curr_ = 0;
			next_ = 1;



		}
		if(gi_ == SVO || gi_ == Dyn_Svo)
		{
			defaultPass_->Load("RenderCalls/ReflectiveShadowMap.xml");
			defaultPass_->Load("RenderCalls/SparseVoxelOctree.xml");
			defaultPass_->Load("RenderCalls/DeferredGISVO.xml");

			giVolume_ = new SparseVoxelOctree();

			giVolume_->Create(256);
		}
		if(gi_ == NoGI)
		{
			giVolume_ = 0;
			giEnabled_ = false;
			defaultPass_->Load("RenderCalls/Deferred.xml");
		}

		Setup();
		



		XMFLOAT3 skyVerts[8] =
		{
			XMFLOAT3(-1,1,1),
			XMFLOAT3(1,1,1),
			XMFLOAT3(1,-1,1),
			XMFLOAT3(-1,-1,1),
			XMFLOAT3(1,1,-1),
			XMFLOAT3(-1,1,-1),
			XMFLOAT3(-1,-1,-1),
			XMFLOAT3(1,-1,-1),
		};

		unsigned short skyIndices[36] =
		{
			0,1,2,2,3,0,   // Front
			1,4,7,7,2,1,   // Right
			4,5,6,6,7,4,   // Back
			5,0,3,3,6,5,   // Left
			5,4,1,1,0,5,   // Top
			3,2,7,7,6,3    // Bottom
		};



		SharedPtr<YumeVertexBuffer> skyVb(rhi_->CreateVertexBuffer());
		skyVb->SetShadowed(true);
		skyVb->SetSize(8,MASK_POSITION);
		skyVb->SetData(skyVerts);

		SharedPtr<YumeIndexBuffer> skyIb(rhi_->CreateIndexBuffer());
		skyIb->SetShadowed(true);
		skyIb->SetSize(36,false);
		skyIb->SetData(skyIndices);

		skyGeometry_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		skyGeometry_->SetIndexBuffer(skyIb);
		skyGeometry_->SetVertexBuffer(0,skyVb);
		skyGeometry_->SetDrawRange(TRIANGLE_LIST,0,skyIb->GetIndexCount());



		SharedPtr<YumeVertexBuffer> triangleVb(gYume->pRHI->CreateVertexBuffer());

		SimpleVertex v1 ={DirectX::XMFLOAT3(-1.f,-3.f,1.f)};
		SimpleVertex v2 ={DirectX::XMFLOAT3(-1.f,1.f,1.f)};
		SimpleVertex v3 ={DirectX::XMFLOAT3(3.f,1.f,1.f)};

		SimpleVertex vertices[3] ={v1,v2,v3};
		triangleVb->SetShadowed(true);
		triangleVb->SetSize(3,MASK_POSITION);
		triangleVb->SetData(vertices);

		fullscreenTriangle_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		fullscreenTriangle_->SetVertexBuffer(0,triangleVb);
		fullscreenTriangle_->SetDrawRange(TRIANGLE_LIST,0,0,0,3);

		/*pp_ = YumeAPINew YumePostProcess(this);
		pp_->Setup();*/


		screenBuffers_.clear();
		screenBufferAllocations_.clear();

		envType_ = (EnvironmentMapType)atoi((gYume->pEnv->GetVariant("DynEnv").Get<YumeString>()).c_str());
	}

	void YumeMiscRenderer::Setup()
	{
		SharedPtr<YumeVertexBuffer> plvb(gYume->pRHI->CreateVertexBuffer());
		plvb->SetShadowed(true);
		plvb->SetSize(24,MASK_POSITION);
		plvb->SetData(pointLightVertexData);

		SharedPtr<YumeIndexBuffer> plib(gYume->pRHI->CreateIndexBuffer());
		plib->SetShadowed(true);
		plib->SetSize(132,false);
		plib->SetData(pointLightIndexData);

		pointLightGeometry_ = new YumeGeometry;
		pointLightGeometry_->SetVertexBuffer(0,plvb);
		pointLightGeometry_->SetIndexBuffer(plib);
		pointLightGeometry_->SetDrawRange(TRIANGLE_LIST,0,plib->GetIndexCount());

		pointLightAttTexture_ = gYume->pResourceManager->PrepareResource<YumeTexture2D>("Textures/Ramp.png");

		lightMap = rhi_->CreateTexture2D();
		lightMap->SetSize(1600,900,rhi_->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET);


		for(int i=0; i < 2; ++i)
			adaptLuminance[i] = 0;

		backBufferSub_ = rhi_->CreateTexture2D();
		backBufferSub_->SetSize(1600,900,rhi_->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET);

		unsigned CubemapSize = 256;
		cubemapRt_ = (rhi_->CreateTextureCube());
		cubemapRt_->SetNumLevels(9);
		cubemapRt_->SetName("CubeMap");
		cubemapRt_->SetSize(CubemapSize,rhi_->GetRGBAFormatNs(),TEXTURE_RENDERTARGET);

		cubemapDsv_ = (rhi_->CreateTexture2D());
		cubemapDsv_->SetName("DSCubeMap");
		cubemapDsv_->SetSize(CubemapSize,CubemapSize,39,TEXTURE_DEPTHSTENCIL);


		CreateCubemapCameras(0,15,0);

		cameraMoveSpeed_ = (float)gYume->pEnv->GetVariant("CamMoveSpeed").Get<int>();
		if(cameraMoveSpeed_ == 0)
			cameraMoveSpeed_ = CAMERA_MOVE_SPEED;
	}

	void YumeMiscRenderer::CreateCubemapCameras(float x,float y,float z)
	{
		// Generate the cube map about the given position.
		XMFLOAT3 center(x,y,z);
		XMFLOAT3 worldUp(0.0f,1.0f,0.0f);

		// Look along each coordinate axis.
		XMFLOAT3 targets[6] =
		{
			XMFLOAT3(x+1.0f,y,z), // +X
			XMFLOAT3(x-1.0f,y,z), // -X
			XMFLOAT3(x,y+1.0f,z), // +Y
			XMFLOAT3(x,y-1.0f,z), // -Y
			XMFLOAT3(x,y,z+1.0f), // +Z
			XMFLOAT3(x,y,z-1.0f)  // -Z
		};

		// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
		// are looking down +Y or -Y, so we need a different "up" vector.
		XMFLOAT3 ups[6] =
		{
			XMFLOAT3(0.0f,1.0f,0.0f),  // +X
			XMFLOAT3(0.0f,1.0f,0.0f),  // -X
			XMFLOAT3(0.0f,0.0f,-1.0f), // +Y
			XMFLOAT3(0.0f,0.0f,+1.0f), // -Y
			XMFLOAT3(0.0f,1.0f,0.0f),	 // +Z
			XMFLOAT3(0.0f,1.0f,0.0f)	 // -Z
		};

		for(int i=0; i < 6; ++i)
		{
			YumeCamera* cam(new FirstPersonCamera(1,M_PI * 0.5f,0.1f,1000.0f));
			cam->SetLookAt(center,targets[i],ups[i]);

			cubemapCams_.push_back(cam);
		}

	}

	void YumeMiscRenderer::Update(float timeStep)
	{
		/*if(!gYume->pInput->GetMouseButtonDown(MOUSEB_LEFT))
			camera_->FrameMove(timeStep);*/

		UpdateCamera(timeStep);
		UpdateLights();

		ConstructFrustum(zFar);

	}

	void YumeMiscRenderer::ApplyShaderParameters(RenderCall* call)
	{
		const YumeMap<YumeHash,Variant>::type& variants = call->GetShaderVariants();
		YumeMap<YumeHash,Variant>::const_iterator It = variants.begin();

		for(It; It != variants.end(); ++It)
		{
			rhi_->SetShaderParameter(It->first,It->second);
		}

		////Matrices
		const YumeMap<YumeHash,DirectX::XMMATRIX>::type& matrices = call->GetShaderMatrices();
		YumeMap<YumeHash,DirectX::XMMATRIX>::const_iterator mIt = matrices.begin();

		for(mIt; mIt != matrices.end(); ++mIt)
		{
			rhi_->SetShaderParameter(mIt->first,mIt->second);
		}

		const YumeMap<YumeHash,DirectX::XMFLOAT4>::type& vectors = call->GetShaderVectors4();
		YumeMap<YumeHash,DirectX::XMFLOAT4>::const_iterator vIt = vectors.begin();

		for(vIt; vIt != vectors.end(); ++vIt)
		{
			rhi_->SetShaderParameter(vIt->first,vIt->second);
		}

		const YumeMap<YumeHash,DirectX::XMFLOAT3>::type& vectors3 = call->GetShaderVectors3();
		YumeMap<YumeHash,DirectX::XMFLOAT3>::const_iterator vIt3 = vectors3.begin();

		for(vIt3; vIt3 != vectors3.end(); ++vIt3)
		{
			rhi_->SetShaderParameter(vIt3->first,vIt3->second);
		}
	}

	void YumeMiscRenderer::RemoveUnusedBuffers()
	{
		for(YumeMap<long long,YumeVector<SharedPtr<YumeTexture> >::type >::iterator i = screenBuffers_.begin(); i != screenBuffers_.end();)
		{
			HashMap<long long,Vector<SharedPtr<YumeTexture> > >::Iterator current = i++;
			YumeVector<SharedPtr<YumeTexture> >::type& buffers = current->second;
			for(unsigned j = buffers.size() - 1; j < buffers.size(); --j)
			{
				TexturePtr buffer = buffers[j];
				if(buffer->GetUseTimer() > 1000U)
				{
					YUMELOG_DEBUG("Removed unused screen buffer size " << String(buffer->GetWidth()).c_str() << "x" << String(buffer->GetHeight()).c_str() <<
						" format " << String(buffer->GetFormat()).c_str());
					buffers.erase(j);
				}
			}
			if(buffers.empty())
			{
				screenBufferAllocations_.erase(current->first);
				screenBuffers_.erase(current);
			}
		}
	}
	TexturePtr YumeMiscRenderer::AllocateAdditionalBuffers(int width,int height,unsigned format,bool cubemap,bool filtered,bool srgb,bool mips,
		unsigned persistentKey)
	{
		bool depthStencil = false;

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
		if(mips)
			searchKey |= 0x160000000000000LL;

		// Add persistent key if defined
		if(persistentKey)
			searchKey += ((long long)persistentKey << 32);

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
				SharedPtr<YumeTexture2D> newTex2D(rhi_->CreateTexture2D());
				YumeString name = "AllocBuffer_";
				name.AppendWithFormat("%i",searchKey);
				newTex2D->SetName(name);
				RenderTargetDesc desc;
				desc.Name = name;
				desc.Usage = depthStencil ? TEXTURE_DEPTHSTENCIL : TEXTURE_RENDERTARGET;
				desc.Index = 0;
				desc.Type = RT_INPUT | RT_OUTPUT;
				newTex2D->SetDesc(desc);
				newTex2D->SetSRGB(true);
				newTex2D->SetSize(width,height,format,depthStencil ? TEXTURE_DEPTHSTENCIL : TEXTURE_RENDERTARGET,1,mips ? 0 : 1);


				newBuffer = newTex2D;

			}
			else
			{
				//Cube
			}

			newBuffer->SetSRGB(srgb);
			newBuffer->SetFilterMode(filtered ? FILTER_BILINEAR : FILTER_NEAREST);
			newBuffer->ResetUseTimer();
			screenBuffers_[searchKey].push_back(newBuffer);

			YUMELOG_DEBUG("Allocated new screen buffer size " << YumeString(width).c_str() << "x" << YumeString(height).c_str() << " format " << YumeString(format).c_str());
			return newBuffer;
		}
		else
		{
			TexturePtr buffer = screenBuffers_[searchKey][allocations];
			return buffer;
		}
	}

	void YumeMiscRenderer::PrepareRendering()
	{

		//Reset allocations
		for(YumeMap<long long,unsigned>::iterator i = screenBufferAllocations_.begin(); i != screenBufferAllocations_.end(); ++i)
			i->second = 0;


		bool hasBackbufferRead = false;
		bool hasPingpong = false;
		bool needSubstitute = false;
		bool hasPostProcess = false;

		unsigned numViewportTextures = 0;

		for(unsigned i = 0; i < defaultPass_->calls_.size();++i)
		{
			RenderCallPtr call = defaultPass_->calls_[i];

			if(!call->GetEnabled())
				continue;

			if(!hasBackbufferRead && call->GetReadBackbuffer())
				hasBackbufferRead = true;

			if(!hasPingpong && CheckPingPong(call,i))
				hasPingpong = true;

			if(!hasPostProcess && call->GetPostProcessPass())
				hasPostProcess = true;
		}


		if(hasBackbufferRead)
		{
			++numViewportTextures;

			if(hasPingpong && !needSubstitute)
				++numViewportTextures;

			for(unsigned i = 0; i < 2; ++i)
			{
				viewportTextures_[i] = i < numViewportTextures ? AllocateAdditionalBuffers(rhi_->GetWidth(),rhi_->GetHeight(),rhi_->GetRGBAFormatNs(),false,true,true,false) : (TexturePtr)0;
			}
		}

		if(hasPostProcess)
		{
			viewportTextures_[numViewportTextures] = AllocateAdditionalBuffers(rhi_->GetWidth(),rhi_->GetHeight(),rhi_->GetRGBA16FormatNs(),false,true,true,true);
			++numViewportTextures;
		}
	}

	void YumeMiscRenderer::Render()
	{
		if(!defaultPass_->calls_.size())
		{
			rhi_->BindBackbuffer();
			rhi_->Clear(CLEAR_COLOR);
		}
		else
		{
			PrepareRendering();

			if(updateCubemap_)
			{
				RenderIntoCubemap();

				if(envType_ == Env_Static)
					updateCubemap_ = false;
				else
					updateCubemap_ = true;
			}

			currentRenderTarget_ = 0;


			bool isPingPonging = false;

			unsigned callSize = defaultPass_->calls_.size();

			unsigned lastCallIndex = 0;
			for(int i=0; i < callSize; ++i)
			{
				if(defaultPass_->calls_[i]->GetEnabled())
					lastCallIndex = i;
			}

			for(int i=0; i < callSize; ++i)
			{
				RenderCallPtr call = defaultPass_->calls_[i];

				//Debugging purposes,no use
				YumeString callName = call->GetPassName();

				if(!call->GetEnabled())
					continue;

				bool backbufferRead = call->GetReadBackbuffer();
				bool backbufferWrite = call->GetWriteBackbuffer();
				bool pingpong = CheckPingPong(call,i);


				unsigned outputCount = call->GetNumOutputs();



				ApplyRendererFlags(call);

				if(backbufferRead && backbufferModified_)
				{
					if(currentRenderTarget_ && pingpong)
						isPingPonging = true;

					if(!isPingPonging)
					{
						if(!currentRenderTarget_)
						{
							rhi_->ResolveToTexture(dynamic_cast<Texture2DPtr>(viewportTextures_[0]),IntRect(0,0,rhi_->GetWidth(),rhi_->GetHeight()));
							currentViewportTexture_ = viewportTextures_[0];
							backbufferModified_ = false;
							usedResolve_ = true;
						}
						else
						{
							if(backbufferWrite)
							{

							}
							else
							{
								currentViewportTexture_ = currentRenderTarget_->GetParentTexture();
							}
						}
					}
					else
					{
						viewportTextures_[1] = viewportTextures_[0];
						viewportTextures_[0] = currentRenderTarget_->GetParentTexture();
						currentViewportTexture_ = viewportTextures_[0];
						backbufferModified_ = false;
					}
				}

				if(pingpong)
					isPingPonging = true;

				if(backbufferWrite)
				{
					if(isPingPonging)
					{
						currentRenderTarget_ = ((Texture2DPtr)viewportTextures_[1])->GetRenderSurface();

						if(call->GetType() == FSTRIANGLE && i == lastCallIndex)
						{
							if(call->GetOutput(0))
								currentRenderTarget_ = ((Texture2DPtr)call->GetOutput(0))->GetRenderSurface();
							else
								currentRenderTarget_ = 0;
						}
					}
					else
					{
						if(call->GetOutput(0))
							currentRenderTarget_ = ((Texture2DPtr)call->GetOutput(0))->GetRenderSurface();
						else
							currentRenderTarget_ = 0;
					}
				}

				switch(call->GetType())
				{
				case CallType::CLEAR:
				{
					RHIEvent e("RenderCall::Clear");

					if(call->IsShadowPass())
					{
						if(!updateRsm_)
							break;
					}
					for(int j=0; j < call->GetNumInputs(); ++j)
					{
						TexturePtr input = call->GetInput(j);

						if(input)
						{
							gYume->pRHI->SetRenderTarget(j,input);

							gYume->pRHI->ClearRenderTarget(j,call->GetClearFlags(),call->GetClearColor(j));
						}
					}


					if(call->GetDepthStencil())
					{
						gYume->pRHI->SetDepthStencil(call->GetDepthStencil());
						/*rhi_->ClearDepthStencil(CLEAR_DEPTH | CLEAR_STENCIL,1.0f,0);*/
						rhi_->ClearDepthStencil(CLEAR_DEPTH,1.0f,0);
					}


				}
				break;
				case CallType::SCENE:
				{
					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::Scene");

					if(call->IsShadowPass())
					{
						if(GetGIEnabled())
						{
							RenderReflectiveShadowMap(call);
						}
						else
						{

						}
					}
					else
					{
						if(call->IsVoxelizePass())
						{
							//Voxelize scene
							giVolume_->Voxelize(call,0,true);

							//SceneNodes::type& renderables = scene_->GetRenderables();

							//for(int i=0; i < renderables.size(); ++i)
							//{
							//	SceneNode* node = renderables[i];

							//	//YumeMesh* geometry = node->GetGeometry();
							//	//auto geometries = geometry->GetGeometries();

							//	//giSvoVolume_.Voxelize(call,geometry,i == 0);
							//}
							/*gYume->pRenderer->GetDefaultPass()->DisableRenderCalls("RSM");
							updateRsm_ = false;*/
						}

						if(call->IsDeferredLightPass())
						{
							RenderLights(call,0);
						}

						if(call->GetSkyboxPass())
						{
							RenderSky(0,camera_);
						}

						if(call->IsForwardPass())
						{
							rhi_->SetRenderTarget(0,(YumeTexture*)0);

							TexturePtr stencil = 0;

							if(call->GetRendererFlags() & RF_NODEPTHSTENCIL)
							{
								rhi_->SetNoDepthStencil(true);
							}
							else
							{
								rhi_->SetNoDepthStencil(false);
								stencil = call->GetDepthStencil();


								if(!stencil)
								{
									rhi_->SetDepthStencil((Texture2DPtr)0);
									rhi_->ClearDepthStencil(CLEAR_DEPTH,1.0f,0.0f);
								}
								else
								{
									rhi_->SetDepthStencil((Texture2DPtr)stencil);
									rhi_->ClearDepthStencil(CLEAR_DEPTH | CLEAR_STENCIL,1.0f,0.0f);

									if(call->GetStencilWrite())
										rhi_->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,1);
								}

								rhi_->SetViewport(IntRect(0,0,rhi_->GetWidth(),rhi_->GetHeight()));

								rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,0));

								rhi_->SetBlendMode(BLEND_REPLACE);

								rhi_->SetShaders(call->GetVs(),call->GetPs());

								Light* light = static_cast<Light*>(scene_->GetDirectionalLight());

								const DirectX::XMFLOAT4& pos = light->GetPosition();
								const DirectX::XMFLOAT4& dir = light->GetDirection();
								const YumeColor& color = light->GetColor();

								const unsigned fSize = 4 * 3 * 4;
								float f[fSize] ={
									pos.x,pos.y,pos.z,pos.w,
									dir.x,dir.y,dir.z,dir.w,
									color.r_,color.g_,color.b_,color.a_
								};

								rhi_->SetShaderParameter("main_light",f,4*3);


								SetSamplers(call);
								SetCameraParameters(false,camera_);
								RenderScene();
								rhi_->BindResetRenderTargets(0);
							}
						}

						if(call->GetDeferred())
						{
							TexturePtr colors = call->GetOutput(0);
							TexturePtr normals = call->GetOutput(1);
							TexturePtr lineardepth = call->GetOutput(2);
							TexturePtr spec = call->GetOutput(3);


							TexturePtr stencil = 0;

							if(call->GetRendererFlags() & RF_NODEPTHSTENCIL)
							{
								rhi_->SetNoDepthStencil(true);
							}
							else
							{
								rhi_->SetNoDepthStencil(false);
								stencil = call->GetDepthStencil();


								if(!stencil)
								{
									rhi_->SetDepthStencil((Texture2DPtr)0);
									rhi_->ClearDepthStencil(CLEAR_DEPTH,1.0f,0.0f);
								}
								else
								{
									rhi_->SetDepthStencil((Texture2DPtr)stencil);
									rhi_->ClearDepthStencil(CLEAR_DEPTH | CLEAR_STENCIL,1.0f,0.0f);

									if(call->GetStencilWrite())
										rhi_->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,1);
								}
							}

							rhi_->SetRenderTarget(0,colors);
							rhi_->SetRenderTarget(1,normals);
							rhi_->SetRenderTarget(2,lineardepth);
							rhi_->SetRenderTarget(3,spec);





							rhi_->SetViewport(IntRect(0,0,rhi_->GetWidth(),rhi_->GetHeight()));


							rhi_->ClearRenderTarget(0,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,0));
							rhi_->ClearRenderTarget(1,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,0));
							rhi_->ClearRenderTarget(2,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,0));
							rhi_->ClearRenderTarget(3,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,0));

							rhi_->SetBlendMode(BLEND_REPLACE);
							/*rhi_->SetBlendMode(BLEND_REPLACE);
							rhi_->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,255);
							rhi_->SetDepthTest(CMP_LESS);*/


							rhi_->SetShaders(call->GetVs(),call->GetPs());

							SetSamplers(call);

							//rhi_->BindSampler(PS,0,1,0); //0 is standard filter
							SetCameraParameters(false,camera_);
							RenderScene();
							rhi_->BindResetRenderTargets(0);
						}
					}
					ev.EndEvent();
				}
				break;
				case CallType::LPV_INJECT:
				{
					curr_ = 0;
					next_ = 1;

					if(!updateRsm_)
						break;

					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::LPV_INJECT");

					rhi_->SetViewport(IntRect(0,0,32,32));

					rhi_->SetBlendMode(BLEND_ADD);

					SetSamplers(call);

					rhi_->SetShaders(call->GetVs(),call->GetPs(),call->GetGs());

					ApplyShaderParameters(call);

					unsigned numOutputs = call->GetNumOutputs();

					for(int i=0; i < MAX_RENDERTARGETS; ++i)
					{
						TexturePtr output = call->GetOutput(i);

						if(output)
						{
							rhi_->SetRenderTarget(i,output);
						}
					}

					unsigned inputSize = call->GetNumInputs();
					YumeVector<TexturePtr>::type inputs;

					unsigned startIndex = M_MAX_UNSIGNED;

					for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
					{
						TexturePtr input = call->GetInput(i);

						if(input)
						{
							if(startIndex == M_MAX_UNSIGNED) startIndex = i;
							inputs.push_back(input);
						}
					}
					rhi_->VSBindSRV(startIndex,inputSize,&inputs[0]);

					gYume->pRHI->BindPsuedoBuffer();

					unsigned int num_vpls = static_cast<unsigned int>(1024*1024);

					rhi_->Draw(POINT_LIST,0,num_vpls);
					gYume->pRHI->BindResetRenderTargets(4);
					gYume->pRHI->BindResetTextures(startIndex,inputSize); //Start at 6 count 3

					ev.EndEvent();
				}
				break;
				case SVO_INJECT:
				{
					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::SVO_INJECT");

					UINT num_vpls = 1024 * 1024;

					unsigned numOutputs = call->GetNumOutputs();


					TexturePtr output = call->GetOutput(1);

					TexturePtr uavs[] ={output};
					rhi_->SetRenderTargetsAndUAVs(0,1,1,uavs);

					unsigned inputSize = call->GetNumInputs();
					YumeVector<TexturePtr>::type inputs;

					unsigned startIndex = M_MAX_UNSIGNED;


					for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
					{
						TexturePtr input = call->GetInput(i);

						if(input)
						{
							if(startIndex == M_MAX_UNSIGNED) startIndex = i;
							inputs.push_back(input);
						}
						else
							inputs.push_back(0);
					}
					rhi_->PSBindSRV(startIndex,inputSize,inputs);

					rhi_->SetShaders(call->GetVs(),call->GetPs());

					ApplyShaderParameters(call);

					SetMainLight();

					rhi_->BindPsuedoBuffer();
					rhi_->Draw(POINT_LIST,0,num_vpls);


					TexturePtr clearUavs[1] ={0};
					rhi_->SetRenderTargetsAndUAVs(0,1,1,clearUavs);


					giVolume_->Filter();

					if(gi_ != Dyn_Svo)
					{
						defaultPass_->DisableRenderCalls("RSM");
						defaultPass_->DisableRenderCalls("RSMVoxelize");
						defaultPass_->DisableRenderCalls("RSMInject");
						updateRsm_ = false;

					}

					rhi_->BindResetTextures(6,3);
				}
				break;
				case LPV_NORMALIZE:
				{
					if(!updateRsm_)
						break;

					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::LPV_NORMALIZE");



					std::swap(curr_,next_);
					rhi_->SetViewport(IntRect(0,0,32,32));

					unsigned numOutputs = call->GetNumOutputs();

					for(int i=0; i < MAX_RENDERTARGETS; ++i)
					{
						TexturePtr output = call->GetOutput(i);

						if(output)
						{
							rhi_->SetRenderTarget(i,output);
						}
					}

					unsigned inputSize = call->GetNumInputs();
					YumeVector<TexturePtr>::type inputs;

					unsigned startIndex = 0;

					for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
					{
						TexturePtr input = call->GetInput(i);

						if(input)
						{
							if(startIndex == 0) startIndex = i;
							inputs.push_back(input);
						}
						else
							inputs.push_back(0);
					}
					rhi_->PSBindSRV(startIndex,inputSize,inputs);

					rhi_->SetBlendMode(BLEND_ADD);

					rhi_->SetShaders(call->GetVs(),call->GetPs(),call->GetGs());

					/*SetGIParameters();*/


#ifndef SVO
					static YumeVector<YumeVertexBuffer*>::type vertexBuffers(1);
					static YumeVector<unsigned>::type elementMasks(1);
					vertexBuffers[0] = giVolume_->GetVolumeGeometry()->GetVertexBuffer(0);
					elementMasks[0] = giVolume_->GetVolumeGeometry()->GetVertexBuffer(0)->GetElementMask();
					rhi_->SetVertexBuffers(vertexBuffers,elementMasks);
#endif

					rhi_->BindNullIndexBuffer();

					rhi_->Draw(TRIANGLE_LIST,0,6 * 32);

					rhi_->BindResetRenderTargets(3);
					rhi_->BindResetTextures(7,4,true);
				}
				break;
				case LPV_PROPAGATE:
				{
					if(!updateRsm_)
						break;

					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::LPV_PROPAGATE");

					unsigned numOutputs = call->GetNumOutputs();

					for(int i=0; i < MAX_RENDERTARGETS; ++i)
					{
						TexturePtr output = call->GetOutput(i);

						if(output)
						{
							rhi_->SetRenderTarget(i,output);
						}
					}

					unsigned inputSize = call->GetNumInputs();
					YumeVector<TexturePtr>::type inputs;

					unsigned startIndex = 0;

					for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
					{
						TexturePtr input = call->GetInput(i);

						if(input)
						{
							if(startIndex == 0) startIndex = i;
							inputs.push_back(input);
						}
						else
							inputs.push_back(0);
					}
					rhi_->PSBindSRV(startIndex,inputSize,inputs);




					rhi_->SetBlendMode(BLEND_ADD);

					rhi_->SetShaders(call->GetVs(),call->GetPs(),call->GetGs());

					for(int i=0; i < num_propagations_; ++i)
					{
						rhi_->SetShaderParameter("iteration",(float)i);
						LPVPropagate(call,i);
					}
					if(gi_ != Dyn_Lpv)
					{
						updateRsm_ = false;
						defaultPass_->DisableRenderCalls("RSM");
					}

				}
				break;
				case CallType::GENERATEMIPS:
				{
					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::GENERATEMIPS");

					for(int i=0; i < call->GetNumInputs(); ++i)
					{
						TexturePtr input = call->GetInput(i);
						if(input)
							rhi_->GenerateMips(input);
					}
				}
				break;
				case ADAPT_LUMINANCE:
				{
					TexturePtr target = currentViewportTexture_;
					TexturePtr ppInput = viewportTextures_[2];
					BlitRenderTarget(target,ppInput,false);
					/*TexturePtr ppInput = target;*/



					rhi_->GenerateMips(ppInput);

					currentAdaptedLuminance_= !currentAdaptedLuminance_;

					gYume->pRHI->SetViewport(IntRect(0,0,1,1));

					TexturePtr adaptLum1 = defaultPass_->GetTextureByName("RT_ADAPT_LUMINANCE_1");
					TexturePtr adaptLum0 = defaultPass_->GetTextureByName("RT_ADAPT_LUMINANCE_0");

					TexturePtr bloomFull = defaultPass_->GetTextureByName("BloomTarget");

					adaptLuminance[0] = adaptLum0;
					adaptLuminance[1] = adaptLum1;

					YumeShaderVariation* fsTriangle = rhi_->GetShader(VS,"LPV/fs_triangle","","fs_triangle_vs");
					{
						RHIEvent e("Adapt Luminance");


						YumeShaderVariation* ps = gYume->pRHI->GetShader(PS,"Bloom","","ps_adapt_exposure");

						gYume->pRHI->SetShaders(fsTriangle,ps,0);
						SetCameraParameters(false,camera_);

						YumeGeometry* fs = GetFsTriangle();

						auto textures = gYume->pRenderer->GetFreeTextures();
						textures[0] = ppInput;
						textures[5] = adaptLuminance[!currentAdaptedLuminance_];
						gYume->pRHI->PSBindSRV(0,1,textures);

						ApplyShaderParameters(call);
						SetPerFrameConstants();

						gYume->pRHI->SetRenderTarget(0,adaptLuminance[currentAdaptedLuminance_]);

						fs->Draw(gYume->pRHI);


						gYume->pRHI->BindResetRenderTargets(0);
						gYume->pRHI->BindResetTextures(0,1);
					}


					{
						RHIEvent e("Bloom Threshold");

						gYume->pRHI->SetViewport(IntRect(0,0,rhi_->GetWidth(),rhi_->GetHeight()));
						YumeShaderVariation* threshold = gYume->pRHI->GetShader(PS,"Bloom","BLOOM_THRESHOLD","ps_bloom_treshold");

						gYume->pRHI->SetShaders(fsTriangle,threshold,0);
						SetCameraParameters(false,camera_);

						YumeGeometry* fs = GetFsTriangle();

						auto textures = gYume->pRenderer->GetFreeTextures();
						textures[0] = ppInput;
						textures[5] = adaptLuminance[!currentAdaptedLuminance_];
						gYume->pRHI->PSBindSRV(0,2,textures);

						ApplyShaderParameters(call);
						SetPerFrameConstants();

						gYume->pRHI->SetRenderTarget(0,bloomFull);


						fs->Draw(gYume->pRHI);

						gYume->pRHI->BindResetTextures(0,13);
						gYume->pRHI->BindResetRenderTargets(0);
					}

				}
				break;
				case CallType::FSTRIANGLE:
				{
					RHIEvent ev;
					if(call->GetPassName().length())
					{
						ev.BeginEvent(call->GetPassName());
					}
					else
						ev.BeginEvent("RenderCall::FSTRIANGLE");

					SetSamplers(call);

					YumeShaderVariation* fsTriangle = rhi_->GetShader(VS,"LPV/fs_triangle","","fs_triangle_vs");


					rhi_->SetShaders(fsTriangle,call->GetPs());


					YumeVector<TexturePtr>::type inputs;



					unsigned startIndex = M_MAX_UNSIGNED;
					unsigned inputSize = call->GetNumInputs();
					for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
					{
						TexturePtr input = call->GetInput(i);

						if(input)
						{
							if(startIndex == M_MAX_UNSIGNED) startIndex = i;
							inputs.push_back(input);
						}
						else
						{
							if(i == 0 && call->GetReadBackbuffer())
							{
								inputs.push_back(currentViewportTexture_);
								startIndex = i;
							}
							else
								inputs.push_back(0);
						}
					}

					if(call->GetPassName() == "BloomFinal")
						inputs[5] = adaptLuminance[currentAdaptedLuminance_];

					inputs[14] = cubemapRt_; //Change this,noise texture conflict


					//Variants
					ApplyShaderParameters(call);

					/*rhi_->BindDefaultDepthStencil();
					rhi_->SetNoDepthStencil(false);*/
					/*rhi_->SetNoDepthStencil(true);
					rhi_->SetDepthStencil((Texture2DPtr)0);
					rhi_->SetStencilTest(true,CMP_ALWAYS,OP_KEEP,OP_KEEP,OP_INCR,OP_DECR,0);*/
					/*rhi_->SetDepthEnable(false);*/

					if(!call->GetPostProcessPass())
					{
						TexturePtr stencil = defaultPass_->GetTextureByName("LightDSV");
						rhi_->SetNoDepthStencil(true);
						rhi_->SetDepthStencil((Texture2DPtr)stencil);
						rhi_->SetDepthEnable(false);
						rhi_->SetDepthTest(CMP_LESS);
						rhi_->SetStencilTest(true,CMP_EQUAL,OP_KEEP,OP_KEEP,OP_KEEP,OP_KEEP,1,M_MAX_UNSIGNED,0);
						rhi_->SetDepthWrite(false);
						rhi_->SetBindReadOnlyDepthStencil(true);
					}

					Light* light = static_cast<Light*>(scene_->GetDirectionalLight());

					const DirectX::XMFLOAT4& pos = light->GetPosition();
					const DirectX::XMFLOAT4& dir = light->GetDirection();
					const YumeColor& color = light->GetColor();

					const unsigned fSize = 4 * 3 * 4;
					float f[fSize] ={
						pos.x,pos.y,pos.z,pos.w,
						dir.x,dir.y,dir.z,dir.w,
						color.r_,color.g_,color.b_,color.a_
					};


					if(call->GetPostProcessPass())
					{
						auto noise = gYume->pResourceManager->PrepareResource<YumeTexture3D>("Textures/noise.dds");
						inputs[14] = noise;
						++inputSize;
					}

					rhi_->PSBindSRV(startIndex,inputSize,inputs);
					gYume->pRHI->SetShaderParameter("main_light",f,4*3);

					bool deptheanble = rhi_->GetDepthState();
					bool depthwrite = rhi_->GetDepthWrite();


					/*rhi_->SetDepthEnable(false);
					rhi_->SetDepthWrite(false);*/

					SetCameraParameters(false,camera_);
					SetPerFrameConstants();

					TexturePtr output = call->GetOutput(0);

					if(!output && !currentRenderTarget_)
					{
						gYume->pRHI->SetViewport(IntRect(0,0,rhi_->GetWidth(),rhi_->GetHeight()));
						rhi_->SetRenderTarget(0,(YumeTexture*)0);
						rhi_->Clear(CLEAR_COLOR,YumeColor(0,0,0,1));

					}
					else if(output)
					{
						gYume->pRHI->SetViewport(IntRect(0,0,output->GetWidth(),output->GetHeight()));
						rhi_->SetRenderTarget(0,output);
						/*rhi_->ClearRenderTarget(0,CLEAR_COLOR);*/

						if(call->GetRendererFlags() & RF_CLEAR)
							rhi_->ClearRenderTarget(0,CLEAR_COLOR);
					}
					else if(currentRenderTarget_)
					{
						gYume->pRHI->SetViewport(IntRect(0,0,currentRenderTarget_->GetWidth(),currentRenderTarget_->GetHeight()));
						rhi_->SetRenderTarget(0,currentRenderTarget_);

						if(call->GetRendererFlags() & RF_CLEAR)
							rhi_->ClearRenderTarget(0,CLEAR_COLOR);
					}

					rhi_->SetNoBlendState(true);


					gYume->pRHI->SetShaderParameter("scene_dim_max",XMFLOAT4(GetMaxBb().x,GetMaxBb().y,GetMaxBb().z,1.0f));
					gYume->pRHI->SetShaderParameter("scene_dim_min",XMFLOAT4(GetMinBb().x,GetMinBb().y,GetMinBb().z,1.0f));

					/*pp_->SetPPParameters*/



					GetFsTriangle()->Draw(gYume->pRHI);




					/*rhi_->SetDepthEnable(deptheanble);
					rhi_->SetDepthWrite(depthwrite);*/

					unsigned nextCall = i + 1;

					bool resetRt = true;

					if(nextCall < callSize)
					{
						RenderCallPtr nc = defaultPass_->calls_[nextCall];
						if(nc)
						{
							if(!nc->GetPassName().Compare("Skybox"))
							{
								resetRt = false;
							}
						}
					}

					if(resetRt)
						rhi_->BindResetRenderTargets(1);
					rhi_->BindResetTextures(0,MAX_TEXTURE_UNITS);
					rhi_->SetBindReadOnlyDepthStencil(false);
					rhi_->SetDepthTest(CMP_LESSEQUAL);
					rhi_->SetStencilTest(true,CMP_ALWAYS);
					rhi_->SetCullMode(CULL_CCW);
					rhi_->SetDepthWrite(true);
				}
				break;
				default:
					break;
				}
				if(backbufferWrite)
					backbufferModified_ = true;
			}
		}
		RemoveUnusedBuffers();
	}

	bool YumeMiscRenderer::CheckPingPong(RenderCall* call,unsigned index)
	{
		CallType al;
		if(call->GetType() == ADAPT_LUMINANCE)
			al = FSTRIANGLE;
		else
			al = call->GetType();


		if((al != FSTRIANGLE)|| !call->GetReadBackbuffer() || !call->GetWriteBackbuffer())
			return false;


		for(int i=index + 1; i < defaultPass_->calls_.size(); ++i)
		{
			RenderCall* c = defaultPass_->calls_[i];

			if(!c->GetEnabled())
				continue;

			if(c->GetWriteBackbuffer())
			{
				if(c->GetType() != FSTRIANGLE)
					return false;
			}
		}

		return true;
	}

	void YumeMiscRenderer::BlitRenderTarget(TexturePtr source,TexturePtr destination,bool depthWrite)
	{
		if(!source)
			return;

		RHIEvent e("Blit");

		// If blitting to the destination rendertarget, use the actual viewport. Intermediate textures on the other hand
		// are always viewport-sized
		IntVector2 srcSize(source->GetWidth(),source->GetHeight());
		IntVector2 destSize = destination ? IntVector2(destination->GetWidth(),destination->GetHeight()) : IntVector2(
			rhi_->GetWidth(),rhi_->GetHeight());

		IntRect srcRect = IntRect(0,0,srcSize.x_,srcSize.y_);
		IntRect destRect = IntRect(0,0,destSize.x_,destSize.y_);


		rhi_->SetRenderTarget(0,destination);
		for(unsigned i = 1; i < MAX_RENDERTARGETS; ++i)
			rhi_->SetRenderTarget(i,(YumeRenderable*)0);
		rhi_->SetViewport(destRect);

		rhi_->SetNoDepthStencil(true);
		static const String shaderName("Copy");
		YumeShaderVariation* triangle = rhi_->GetShader(VS,"LPV/fs_triangle","","fs_triangle_vs");
		rhi_->SetShaders(triangle,rhi_->GetShader(PS,shaderName,shaderName,"ps_copy"));

		SetGBufferShaderParameters(srcSize,srcRect);

		auto textures = GetFreeTextures();
		textures[0] = source;

		rhi_->PSBindSRV(0,1,textures);
		unsigned sampler = defaultPass_->GetSamplerByName("ShadowFilter").second;
		unsigned samplers[2] ={0,sampler};
		rhi_->BindSampler(PS,0,2,samplers); //Standard

		GetFsTriangle()->Draw(rhi_);

		rhi_->SetNoDepthStencil(true);
		rhi_->BindResetRenderTargets(1);
	}

	void YumeMiscRenderer::RenderReflectiveShadowMap(RenderCall* call)
	{
		if(GetGIEnabled() && call->IsShadowPass() && updateRsm_)
		{
			//Set RSM Render targets
			//Already set

			//Set renderer opts
			rhi_->SetDepthTest(CMP_ALWAYS);
			rhi_->SetColorWrite(true);
			rhi_->SetFillMode(FILL_SOLID);
			rhi_->SetClipPlane(false);
			rhi_->SetScissorTest(false);

			//Clear render targets

			//Already cleared
			rhi_->SetViewport(IntRect(0,0,rsmSize,rsmSize));

			//
			SetSamplers(call);

			rhi_->SetDepthTest(CMP_LESS);
			rhi_->SetStencilTest(true,CMP_ALWAYS,OP_KEEP,OP_KEEP,OP_INCR,OP_DECR);

			//Set shaders
			rhi_->SetShaders(call->GetVs(),call->GetPs(),0);

			SetCameraParameters(true,camera_);
			RenderScene();

			rhi_->BindResetRenderTargets(4);
		}
	}

	void YumeMiscRenderer::SetSamplers(RenderCall* call)
	{
		if(call->HasVertexSampler())
		{
			unsigned num = call->GetNumVertexSamplers();
			unsigned startIndex = 0;

			YumeVector<unsigned>::type vsSamplers;
			for(int k=0; k < MAX_TEXTURE_UNITS; ++k)
			{
				unsigned samplerId = call->GetVertexSampler(k);

				if(samplerId != M_MAX_UNSIGNED)
				{
					if(startIndex == 0) startIndex = k;

					vsSamplers.push_back(samplerId);
				}
			}
			rhi_->BindSampler(VS,startIndex,num,&vsSamplers[0]);
		}


		if(call->HasPixelSampler())
		{
			unsigned num = call->GetNumPixelSamplers();
			YumeVector<unsigned>::type psSamplers;
			unsigned startIndex = M_MAX_UNSIGNED;

			for(int k=0; k < MAX_TEXTURE_UNITS; ++k)
			{
				unsigned samplerId = call->GetPixelSampler(k);

				if(samplerId != M_MAX_UNSIGNED)
				{
					if(startIndex == M_MAX_UNSIGNED) startIndex = k;

					psSamplers.push_back(samplerId);
				}
			}
			rhi_->BindSampler(PS,startIndex,num,&psSamplers[0]);
		}
	}


	void YumeMiscRenderer::LPVPropagate(RenderCall* call,float iteration)
	{
		//LPV_R_0 ID 0
		//LPV_G_0 ID 1
		//LPV_B_0 ID 2
		//LPV_R_1 ID 3
		//LPV_G_1 ID 4
		//LPV_B_1 ID 5
		//LPV_ACCUMR ID 6
		//LPV_ACCUMG ID 7
		//LPV_ACCUMB ID 8
		//LPV_INJECT_COUNTER ID 9
		std::swap(curr_,next_);

		YumeString eventName = "Iteration ";
		eventName.AppendWithFormat("%f",iteration);

		RHIEvent e(eventName,true);

		TexturePtr lpvr_next = defaultPass_->GetTextureByName(GetTextureName("LPV_R_",next_));
		TexturePtr lpvg_next = defaultPass_->GetTextureByName(GetTextureName("LPV_G_",next_));
		TexturePtr lpvb_next = defaultPass_->GetTextureByName(GetTextureName("LPV_B_",next_));

		TexturePtr lpvr_curr = defaultPass_->GetTextureByName(GetTextureName("LPV_R_",curr_));
		TexturePtr lpvg_curr = defaultPass_->GetTextureByName(GetTextureName("LPV_G_",curr_));
		TexturePtr lpvb_curr = defaultPass_->GetTextureByName(GetTextureName("LPV_B_",curr_));

		TexturePtr lpvaccumr = call->GetOutput(3);
		TexturePtr lpvaccumg = call->GetOutput(4);
		TexturePtr lpvaccumb = call->GetOutput(5);


		rhi_->SetRenderTarget(0,lpvr_next);
		rhi_->SetRenderTarget(1,lpvg_next);
		rhi_->SetRenderTarget(2,lpvb_next);
		rhi_->SetRenderTarget(3,lpvaccumr);
		rhi_->SetRenderTarget(4,lpvaccumg);
		rhi_->SetRenderTarget(5,lpvaccumb);


		rhi_->ClearRenderTarget(0,CLEAR_COLOR);
		rhi_->ClearRenderTarget(1,CLEAR_COLOR);
		rhi_->ClearRenderTarget(2,CLEAR_COLOR);

		YumeVector<TexturePtr>::type textures = GetFreeTextures();
		textures[7] = lpvr_curr;
		textures[8] = lpvg_curr;
		textures[9] = lpvb_curr;
		gYume->pRHI->PSBindSRV(7,3,textures);

		/*SetGIParameters();*/

		ApplyShaderParameters(call);

		rhi_->BindNullIndexBuffer();

#ifndef SVO
		giVolume_->GetVolumeGeometry()->Draw(gYume->pRHI);
#endif

		rhi_->BindResetRenderTargets(6);
		rhi_->BindResetTextures(7,3,true);

	}

	void YumeMiscRenderer::UpdateGI()
	{
		defaultPass_->EnableRenderCalls("RSM");
		defaultPass_->EnableRenderCalls("RSMVoxelize");
		defaultPass_->EnableRenderCalls("RSMInject");
		updateRsm_ = true;
	}

	void YumeMiscRenderer::SetFloorRoughness(float f)
	{
		SceneNodes::type& renderables = scene_->GetRenderables();

		SceneNode* floorNode = 0;
		for(int i=0; i < renderables.size(); ++i)
		{
			SceneNode* node = renderables[i];

			floorNode = node;
		}

		static_cast<StaticModel*>(floorNode)->SetFloorRoughness(f);
	}

	YumeString YumeMiscRenderer::GetTextureName(const YumeString& s,int num)
	{
		YumeString ss = s;
		ss.AppendWithFormat("%i",num);
		return ss;
	}

	void YumeMiscRenderer::ApplyRendererFlags(RenderCallPtr call)
	{
		unsigned flags = call->GetRendererFlags();

		YumeRHI* rhi = gYume->pRHI;

		//Defaults
		rhi->SetNoDepthStencil(false);
		rhi->SetNoBlendState(false);

		if(flags & RF_NODEPTHSTENCIL)
			rhi->SetNoDepthStencil(true);

		if(flags & RF_NOBLENDSTATE)
			rhi->SetNoBlendState(true);
	}

	void YumeMiscRenderer::SetGIParameters()
	{
		defaultPass_->SetShaderParameter("gi_scale",giParams_.Scale);
		defaultPass_->SetShaderParameter("lpv_flux_amplifier",giParams_.LPVFlux);
		defaultPass_->SetShaderParameter("debug_gi",giParams_.DebugView);
	}

	void YumeMiscRenderer::RenderFullScreenTexture(const IntRect& rect,YumeTexture2D* overlaytexture)
	{
		YumeVector<TexturePtr>::type texture = GetFreeTextures();
		texture[10] = overlaytexture;
		rhi_->PSBindSRV(10,1,texture);

		YumeShaderVariation* triangle = rhi_->GetShader(VS,"LPV/fs_triangle","","fs_triangle_vs");
		YumeShaderVariation* overlay = rhi_->GetShader(PS,"LPV/Overlay");

		rhi_->SetBlendMode(BLEND_PREMULALPHA);

		unsigned samplerSt = defaultPass_->GetSamplerByName("Standard").second;
		unsigned samplerSh = defaultPass_->GetSamplerByName("ShadowFilter").second;
		unsigned samplers[2] ={samplerSt,samplerSh};
		rhi_->BindSampler(PS,0,2,samplers); //Standard

		rhi_->SetShaders(triangle,overlay,0);
		rhi_->SetNoDepthStencil(true);


		rhi_->SetViewport(rect);

		rhi_->SetDepthStencil((YumeTexture2D*)0);
		rhi_->SetRenderTarget(0,(YumeTexture*)0);

		fullscreenTriangle_->Draw(rhi_);

	}

	void YumeMiscRenderer::RenderSky(YumeRenderable* target,YumeCamera* camera)
	{
		//Sky will set its shaders.

		YumeShaderVariation* skyVs = rhi_->GetShader(VS,"ClearSky","ClearSky","sky_vs");
		YumeShaderVariation* skyGs = rhi_->GetShader(GS,"ClearSky","ClearSky","sky_gs");
		YumeShaderVariation* skyPs = rhi_->GetShader(PS,"ClearSky","ClearSky","sky_ps");




		rhi_->SetShaders(skyVs,skyPs,skyGs);

		SetCameraParameters(false,camera);

		DirectX::XMVECTOR eyePos = camera->Position();
		XMMATRIX T = XMMatrixTranslationFromVector(eyePos);

		rhi_->SetShaderParameter("wvp",T * camera->ViewMatrix() * camera->ProjectionMatrix());

		//XMMATRIX view = camera->GetViewMatrix();
		//XMFLOAT4X4 skyView;
		//XMStoreFloat4x4(&skyView,view);

		//skyView._41 = 0;
		//skyView._42 = 0;
		//skyView._43 = 0;
		//rhi_->SetShaderParameter("vp",DirectX::XMMatrixMultiply(XMLoadFloat4x4(&skyView),camera->GetProjMatrix()));
		//rhi_->SetShaderParameter("world",XMMatrixScaling(5000,5000,5000));

		//rhi_->SetDepthEnable(false);
		//rhi_->SetDepthTest(CMP_LESS);
		/*rhi_->SetStencilTest(true,CMP_EQUAL,OP_KEEP,OP_KEEP,OP_KEEP,OP_KEEP,1,M_MAX_UNSIGNED,0);
		rhi_->SetBlendMode(BLEND_REPLACE);
		rhi_->SetCullMode(CULL_NONE);*/
		rhi_->SetNoDepthStencil(false);
		//rhi_->SetDepthStencil((Texture2DPtr)0);
		rhi_->SetDepthTest(CMP_LESSEQUAL);
		rhi_->SetStencilTest(true,CMP_ALWAYS);
		rhi_->SetDepthEnable(true);
		/*rhi_->SetCullMode(CULL_CCW);
		rhi_->SetDepthWrite(true);*/

		rhi_->SetRenderTarget(0,target);
		rhi_->SetShaderParameter("SunDirection",DirectX::XMFLOAT4(0.577350300f,0.577350300f,0.577350300f,1));

		skyGeometry_->Draw(rhi_);


		rhi_->BindResetRenderTargets(1);
	}

	void YumeMiscRenderer::RenderLights(RenderCall* call,TexturePtr t)
	{
		SceneNodes::type& renderables = scene_->GetLights();

		TexturePtr target = call->GetOutput(0);
		TexturePtr stencil = defaultPass_->GetTextureByName("LightDSV");
		rhi_->SetNoDepthStencil(false);
		rhi_->SetDepthStencil((Texture2DPtr)stencil);
		rhi_->SetRenderTarget(0,target);
		rhi_->SetDepthWrite(false);
		rhi_->SetBindReadOnlyDepthStencil(true);
		rhi_->Clear(CLEAR_COLOR);

		bool oldDepthState = rhi_->GetDepthState();

		for(int i=0; i < renderables.size(); ++i)
		{
			SceneNode* node = renderables[i];

			Light* light = static_cast<Light*>(node);

			RHIEvent e("Light Pass");

			LightType ltype = light->GetType();

			BlendMode blend = rhi_->GetBlendMode();


			if(ltype == LT_POINT)
			{
				rhi_->SetDepthTest(CompareMode::CMP_GREATEREQUAL);
				rhi_->SetCullMode(CULL_CW);
				rhi_->SetStencilTest(true,CMP_EQUAL,OP_KEEP,OP_KEEP,OP_KEEP,OP_KEEP,1,M_MAX_UNSIGNED,0);
				rhi_->SetDepthWrite(false);
				rhi_->SetBlendMode(BLEND_ADD);

			}




			if(ltype == LT_DIRECTIONAL)
			{
				rhi_->SetDepthEnable(false);
				rhi_->SetDepthTest(CMP_LESS);
				rhi_->SetStencilTest(true,CMP_EQUAL,OP_KEEP,OP_KEEP,OP_KEEP,OP_KEEP,1,M_MAX_UNSIGNED,0);
				rhi_->SetBlendMode(BLEND_ADD);
			}

			if(light->GetType() == LT_POINT)
			{
				YumeShaderVariation* deferredLightVs = rhi_->GetShader(VS,"NoShadows/DeferredLightVS","NoShadows/DeferredLightVS","vs_df");
				YumeShaderVariation* deferredLightPs = rhi_->GetShader(PS,"NoShadows/DeferredLightPS","POINTLIGHT_BRDF","ps_df");

				rhi_->SetShaders(deferredLightVs,deferredLightPs);
			}
			else
			{
				YumeShaderVariation* deferredLightVs = rhi_->GetShader(VS,"NoShadows/FsTriangle","","fs_triangle_vs");
				YumeShaderVariation* deferredLightPs = rhi_->GetShader(PS,"NoShadows/DeferredLightPS","DIRECTIONALLIGHT","ps_df_pbr");

				rhi_->SetShaders(deferredLightVs,deferredLightPs);
			}

			XMMATRIX volumeTransform = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(light->GetPosition().x,light->GetPosition().y,light->GetPosition().z,1.0f));
			volumeTransform = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(light->GetRange(),light->GetRange(),light->GetRange()),volumeTransform);

			SetCameraParameters(false,camera_);
			ApplyShaderParameters(call);

			Light* dirlight = static_cast<Light*>(scene_->GetDirectionalLight());

			const DirectX::XMFLOAT4& pos = dirlight->GetPosition();
			const DirectX::XMFLOAT4& dir = dirlight->GetDirection();
			const YumeColor& color = dirlight->GetColor();

			const unsigned fSize = 4 * 3 * 4;
			float f[fSize] ={
				pos.x,pos.y,pos.z,pos.w,
				dir.x,dir.y,dir.z,dir.w,
				color.r_,color.g_,color.b_,color.a_
			};
			rhi_->SetShaderParameter("main_light",f,4*3);


			XMMATRIX wv = DirectX::XMMatrixMultiply(volumeTransform,camera_->ViewMatrix());
			rhi_->SetShaderParameter("wv",wv);
			rhi_->SetShaderParameter("LightColor",light->GetColor());
			rhi_->SetShaderParameter("LightPosition",light->GetPosition());
			rhi_->SetShaderParameter("LightDirection",DirectX::XMFLOAT4(-0.577350300f,-0.577350300f,-0.577350300f,light->GetRange()));
			rhi_->SetShaderParameter("volume_transform",volumeTransform);
			/*rhi_->SetShaderParameter("camera_rot",DirectX::XMLoadFloat4x4(&camera_->()));*/

			TexturePtr colors = defaultPass_->GetTextureByName("SCENE_COLORS");
			TexturePtr normals = defaultPass_->GetTextureByName("SCENE_NORMALS");
			TexturePtr spec = defaultPass_->GetTextureByName("SCENE_SPECULAR");
			TexturePtr ld = defaultPass_->GetTextureByName("SCENE_LINEARDEPTH");

			YumeVector<TexturePtr>::type inputs = GetFreeTextures();

			inputs[2] = colors;
			inputs[3] = spec;
			inputs[4] = normals;
			inputs[5] = ld;
			inputs[14] = cubemapRt_;
			rhi_->PSBindSRV(2,4,inputs);

			SetGBufferShaderParameters(IntVector2(gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight()),IntRect(0,0,gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight()));


			if(light->GetType() == LT_POINT)
				pointLightGeometry_->Draw(rhi_);
			else
				GetFsTriangle()->Draw(rhi_);

			//restore
			rhi_->SetDepthTest(CMP_LESSEQUAL);
			rhi_->SetStencilTest(true,CMP_ALWAYS);
			rhi_->SetCullMode(CULL_CCW);
			rhi_->SetDepthWrite(true);
			rhi_->SetBlendMode(blend);
			rhi_->SetDepthEnable(oldDepthState);


			rhi_->BindResetTextures(0,MAX_TEXTURE_UNITS,true);
		}


		rhi_->BindResetRenderTargets(1);
		rhi_->SetBindReadOnlyDepthStencil(false);
	}

	void YumeMiscRenderer::RenderIntoCubemap()
	{
		//setup camera

		//Render cubemap
		unsigned CubemapSize = 256;
		rhi_->SetViewport(IntRect(0,0,CubemapSize,CubemapSize));
		rhi_->SetStencilTest(true,CMP_ALWAYS,OP_REF,OP_KEEP,OP_KEEP,OP_KEEP,1);

		RHIEvent e("RenderCubemap");

		for(int i=0; i < MAX_CUBEMAP_FACES; ++i)
		{
			RHIEvent ev("RenderFace");
			CubeMapFace cube = (CubeMapFace)i;

			YumeRenderable* target = cubemapRt_->GetRenderSurface(cube);

			rhi_->SetRenderTarget(0,target);
			rhi_->SetDepthStencil(cubemapDsv_);
			rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL,YumeColor(0.75f,0.75f,0.75f));

			YumeShaderVariation* vs = rhi_->GetShader(VS,"ForwardSolidVertex","ForwardSolidVertex","MeshVs");
			YumeShaderVariation* ps = rhi_->GetShader(PS,"ForwardSolid","ForwardSolid","MeshPs");

			rhi_->SetShaders(vs,ps);

			unsigned sampler = defaultPass_->GetSamplerByName("ShadowFilter").second;
			unsigned samplers[2] ={0,sampler};
			rhi_->BindSampler(PS,0,2,samplers); //Standard

			SetCameraParameters(false,cubemapCams_[i]);
			RenderScene();

			RenderSky(target,cubemapCams_[i]);
		}

		rhi_->GenerateMips(cubemapRt_);
		//
	}

	void YumeMiscRenderer::RenderScene()
	{
		SceneNodes::type& renderables = scene_->GetRenderables();

		for(int i=0; i < renderables.size(); ++i)
		{
			SceneNode* node = renderables[i];

			StaticModel* mesh = static_cast<StaticModel*>(node);

			const YumeVector<SharedPtr<RenderBatch> >::type& batch = mesh->GetBatches();

			for(int b = 0; b < batch.size(); ++b)
			{
				YumeGeometry* geometry = batch[b]->geo_;
				Material* material = batch[b]->material_;

				DirectX::XMFLOAT3 max = mesh->GetBbMax();
				DirectX::XMFLOAT3 min = mesh->GetBbMin();



				DirectX::XMVECTOR bbMax = DirectX::XMLoadFloat3(&max);
				DirectX::XMVECTOR bbMin = DirectX::XMLoadFloat3(&min);

				DirectX::XMVECTOR size = DirectX::XMVectorSubtract(bbMax,bbMin);
				float len = DirectX::XMVector3Length(size).m128_f32[0];

				DirectX::XMVECTOR center = DirectX::XMVectorAdd(bbMax,DirectX::XMVectorScale(DirectX::XMVectorSubtract(bbMax,bbMin),0.5f));


				DirectX::XMFLOAT3 bbCenter;
				DirectX::XMStoreFloat3(&bbCenter,center);

				DirectX::XMFLOAT3 bbSize;
				DirectX::XMStoreFloat3(&bbSize,size);

				float fsize = bbSize.x;

				if(!disableFrustumCull_)
				{
					if(!CheckBB(bbCenter.x,bbCenter.y,bbCenter.z,len))
						continue;
				}
				const YumeMap<YumeHash,Variant>::type& parameters = material->GetParameters();
				YumeMap<YumeHash,Variant>::const_iterator It = parameters.begin();

				//Set material params
				for(It; It != parameters.end(); ++It)
					rhi_->SetShaderParameter(It->first,It->second);

				const YumeMap<YumeHash,DirectX::XMFLOAT4>::type& vectors = material->GetShaderVectors4();
				YumeMap<YumeHash,DirectX::XMFLOAT4>::const_iterator vIt = vectors.begin();

				for(vIt; vIt != vectors.end(); ++vIt)
				{
					rhi_->SetShaderParameter(vIt->first,vIt->second);
				}

				const YumeMap<YumeHash,DirectX::XMFLOAT3>::type& vectors3 = material->GetShaderVectors3();
				YumeMap<YumeHash,DirectX::XMFLOAT3>::const_iterator vIt3 = vectors3.begin();

				for(vIt3; vIt3 != vectors3.end(); ++vIt3)
				{
					rhi_->SetShaderParameter(vIt3->first,vIt3->second);
				}


				const YumeVector<SharedPtr<YumeTexture> >::type textures = material->GetTextures();
				YumeVector<TexturePtr>::type inputs;




				if(material->GetNumTextures() > 0)
				{
					TexturePtr diffuse = material->GetTexture(MT_DIFFUSE);
					TexturePtr normal = material->GetTexture(MT_NORMAL);
					TexturePtr specular = material->GetTexture(MT_SPECULAR);
					TexturePtr alpha = material->GetTexture(MT_ALPHA);
					TexturePtr roughness = material->GetTexture(MT_ROUGHNESS);

					YumeVector<TexturePtr>::type textures = GetFreeTextures();

					textures[MT_DIFFUSE] = diffuse;
					textures[MT_NORMAL] = normal;
					textures[MT_SPECULAR] = specular;
					textures[MT_ALPHA] = alpha;
					textures[MT_ROUGHNESS] = roughness;
					rhi_->PSBindSRV(0,5,textures);
				}

				rhi_->SetShaderParameter("world",node->GetTransformation());

				geometry->Draw(rhi_);
			}



		}
	}

	YumeVector<TexturePtr>::type YumeMiscRenderer::GetFreeTextures()
	{
		YumeVector<TexturePtr>::type textures;

		for(int i=0; i < 16; ++i)
			textures.push_back(0);

		return textures;
	}

	void YumeMiscRenderer::SetGBufferShaderParameters(const IntVector2& texSize,const IntRect& viewRect)
	{
		float texWidth = (float)texSize.x_;
		float texHeight = (float)texSize.y_;
		float widthRange = 0.5f * viewRect.Width() / texWidth;
		float heightRange = 0.5f * viewRect.Height() / texHeight;


		DirectX::XMFLOAT4 bufferUVOffset(((float)viewRect.left_) / texWidth + widthRange,
			((float)viewRect.top_) / texHeight + heightRange,widthRange,heightRange);
		rhi_->SetShaderParameter("GBufferOffsets",bufferUVOffset);

		float invSizeX = 1.0f / texWidth;
		float invSizeY = 1.0f / texHeight;
		rhi_->SetShaderParameter(PSP_GBUFFERINVSIZE,Vector2(invSizeX,invSizeY));
	}

	void YumeMiscRenderer::ConstructFrustum(float depth)
	{
		XMMATRIX projMatrix = camera_->ProjectionMatrix();

		XMFLOAT4X4 proj;
		XMStoreFloat4x4(&proj,projMatrix);



		float zMinimum = -proj._43 / proj._33;
		float r = depth / (depth - zMinimum);
		proj._33 = r;
		proj._43 = -r * zMinimum;

		XMMATRIX matrix = XMMatrixMultiply(camera_->ViewMatrix(),projMatrix);

		XMFLOAT4X4 vp;
		XMStoreFloat4x4(&vp,matrix);

		float a,b,c,d;

		a = vp._14 + vp._13;
		b = vp._24 + vp._23;
		c = vp._34 + vp._33;
		d = vp._44 + vp._43;
		frustumPlanes_[0] = XMVectorSet(a,b,c,d);
		frustumPlanes_[0] = XMPlaneNormalize(frustumPlanes_[0]);

		a = vp._14 - vp._13;
		b = vp._24 - vp._23;
		c = vp._34 - vp._33;
		d = vp._44 - vp._43;
		frustumPlanes_[1] = XMVectorSet(a,b,c,d);
		frustumPlanes_[1] = XMPlaneNormalize(frustumPlanes_[1]);

		a = vp._14 + vp._11;
		b = vp._24 + vp._21;
		c = vp._34 + vp._31;
		d = vp._44 + vp._41;
		frustumPlanes_[2] = XMVectorSet(a,b,c,d);
		frustumPlanes_[2] = XMPlaneNormalize(frustumPlanes_[2]);


		a = vp._14 - vp._11;
		b = vp._24 - vp._21;
		c = vp._34 - vp._31;
		d = vp._44 - vp._41;
		frustumPlanes_[3] = XMVectorSet(a,b,c,d);
		frustumPlanes_[3] = XMPlaneNormalize(frustumPlanes_[3]);


		a = vp._14 - vp._12;
		b = vp._24 - vp._22;
		c = vp._34 - vp._32;
		d = vp._44 - vp._42;
		frustumPlanes_[4] = XMVectorSet(a,b,c,d);
		frustumPlanes_[4] = XMPlaneNormalize(frustumPlanes_[4]);


		a = vp._14 + vp._12;
		b = vp._24 + vp._22;
		c = vp._34 + vp._32;
		d = vp._44 + vp._42;
		frustumPlanes_[5] = XMVectorSet(a,b,c,d);
		frustumPlanes_[5] = XMPlaneNormalize(frustumPlanes_[5]);
	}

	bool YumeMiscRenderer::CheckPointAgainstFrustum(float x,float y,float z)
	{
		for(int i=0; i<6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet(x,y,z,1.0f)));
			if(ret < 0.0f)
				return false;
		}

		return true;
	}

	bool YumeMiscRenderer::CheckBB(float xCenter,float yCenter,float zCenter,float size)
	{
		for(int i=0; i<6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - size),(yCenter - size),(zCenter - size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + size),(yCenter - size),(zCenter - size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - size),(yCenter + size),(zCenter - size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + size),(yCenter + size),(zCenter - size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - size),(yCenter - size),(zCenter + size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + size),(yCenter - size),(zCenter + size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - size),(yCenter + size),(zCenter + size),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + size),(yCenter + size),(zCenter + size),1.0f)));
			if(ret >= 0.0f)
				continue;

			return false;
		}

		return true;
	}

	bool YumeMiscRenderer::CheckSphere(float xCenter,float yCenter,float zCenter,float radius)
	{
		for(int i=0; i<6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet(xCenter,yCenter,zCenter,1.0f)));
			if(ret < -radius)
				return false;
		}

		return true;
	}

	bool YumeMiscRenderer::CheckRectangle(float xCenter,float yCenter,float zCenter,float xSize,float ySize,float zSize)
	{
		for(int i=0; i<6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - xSize),(yCenter - ySize),(zCenter - zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + xSize),(yCenter - ySize),(zCenter - zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - xSize),(yCenter + ySize),(zCenter - zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - xSize),(yCenter - ySize),(zCenter + zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + xSize),(yCenter + ySize),(zCenter - zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + xSize),(yCenter - ySize),(zCenter + zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter - xSize),(yCenter + ySize),(zCenter + zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(frustumPlanes_[i],XMVectorSet((xCenter + xSize),(yCenter + ySize),(zCenter + zSize),1.0f)));
			if(ret >= 0.0f)
				continue;

			return false;
		}

		return true;
	}

	void YumeMiscRenderer::UpdateCamera(float dt)
	{
		YumeInput* i = gYume->pInput;

		float CamMoveSpeed = cameraMoveSpeed_ * dt;
		const float CamRotSpeed = 0.180f * dt;

		XMVECTOR camPos = camera_->Position();

		if(i->GetKeyDown('W'))
			camPos += XMVectorScale(camera_->Forward(),CamMoveSpeed);
		if(i->GetKeyDown('S'))
			camPos += XMVectorScale(camera_->Back(),CamMoveSpeed);
		if(i->GetKeyDown('A'))
			camPos += XMVectorScale(camera_->Left(),CamMoveSpeed);
		if(i->GetKeyDown('D'))
			camPos += XMVectorScale(camera_->Right(),CamMoveSpeed);

		camera_->SetPosition(camPos);

		float xRot = camera_->XRotation();
		float yRot = camera_->YRotation();

		if(i->GetMouseButtonDown(MOUSEB_RIGHT))
		{

			IntVector2 md = i->GetMouseMove();

			xRot += md.y_ * 0.005f;
			yRot += md.x_ * 0.005f;

			camera_->SetXRotation(xRot);
			camera_->SetYRotation(yRot);
		}
	}

	void YumeMiscRenderer::UpdateLights()
	{
		Light* dirLight = static_cast<Light*>(scene_->GetDirectionalLight());
		dirLight->UpdateLightParameters();

		//abort variant matrices
		defaultPass_->SetShaderParameter("scene_dim_max",XMFLOAT4(GetMaxBb().x,GetMaxBb().y,GetMaxBb().z,1.0f));
		defaultPass_->SetShaderParameter("scene_dim_min",XMFLOAT4(GetMinBb().x,GetMinBb().y,GetMinBb().z,1.0f));


		if(updateRsm_ && giEnabled_)
		{
			DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
			DirectX::XMFLOAT4X4 i;
			DirectX::XMStoreFloat4x4(&i,I);

			/*giLpvVolume_.SetModelMatrix(i,bbMin,bbMax);*/
			giVolume_->SetModelMatrix(i,bbMin,bbMax);
		}
	}

	void YumeMiscRenderer::SetGIDebug(bool enabled)
	{
		giParams_.DebugView = enabled;
	}

	void YumeMiscRenderer::SetGIScale(float f)
	{
		giParams_.Scale = f*10;
	}

	void YumeMiscRenderer::SetLPVFluxAmp(float f)
	{
		giParams_.LPVFlux = f;
		updateRsm_ = true;
		defaultPass_->EnableRenderCalls("RSM");
	}

	void YumeMiscRenderer::SetLPVPos(float x,float y,float z)
	{

	}

	void YumeMiscRenderer::SetLightFlux(float f)
	{

	}

	void YumeMiscRenderer::SetLPVNumberIterations(int num)
	{
		num_propagations_ = num;
		updateRsm_ = true;
		defaultPass_->EnableRenderCalls("RSM");
	}

	void YumeMiscRenderer::SetMainLight()
	{
		Light* light = static_cast<Light*>(scene_->GetDirectionalLight());

		const DirectX::XMFLOAT4& pos = light->GetPosition();
		const DirectX::XMFLOAT4& dir = light->GetDirection();
		const YumeColor& color = light->GetColor();

		const unsigned fSize = 4 * 3 * 4;
		float f[fSize] ={
			pos.x,pos.y,pos.z,pos.w,
			dir.x,dir.y,dir.z,dir.w,
			color.r_,color.g_,color.b_,color.a_
		};

		gYume->pRHI->SetShaderParameter("main_light",f,4*3);
	}

	void YumeMiscRenderer::SetPerFrameConstants()
	{
		float totalTime = gYume->pTimer->GetElapsedTime();
		float timeStep = gYume->pTimer->GetTimeStep();

		rhi_->SetShaderParameter("time_delta",timeStep);
		rhi_->SetShaderParameter("time",totalTime);
	}

	void YumeMiscRenderer::UpdateMeshBb(SceneNode* mesh,const DirectX::XMMATRIX& world)
	{
		DirectX::XMVECTOR v_bb_min = XMLoadFloat3(&mesh->GetBbMin());
		DirectX::XMVECTOR v_bb_max = XMLoadFloat3(&mesh->GetBbMax());

		DirectX::XMVECTOR v_diag = DirectX::XMVectorSubtract(v_bb_max,v_bb_min);

		DirectX::XMVECTOR delta = DirectX::XMVectorScale(v_diag,0.05f);

		v_bb_min = DirectX::XMVectorSubtract(v_bb_min,delta);
		v_bb_max = DirectX::XMVectorAdd(v_bb_max,delta);

		v_bb_min = DirectX::XMVectorSetW(v_bb_min,1.f);
		v_bb_max = DirectX::XMVectorSetW(v_bb_max,1.f);


		v_bb_min = DirectX::XMVector4Transform(v_bb_min,world);
		v_bb_max = DirectX::XMVector4Transform(v_bb_max,world);


		DirectX::XMFLOAT3 bMin;
		DirectX::XMFLOAT3 bMax;

		DirectX::XMStoreFloat3(&bbMin,v_bb_min);
		DirectX::XMStoreFloat3(&bbMax,v_bb_max);

		mesh->SetBoundingBox(bbMin,bbMax);

		//bbMin = dmin(bMin,bbMin);
		//bbMax = dmax(bMax,bbMax);

	}

	void YumeMiscRenderer::SetCameraParameters(bool shadowPass,YumeCamera* camera)
	{
		XMMATRIX view = camera->ViewMatrix();
		XMMATRIX proj = camera->ProjectionMatrix();
		XMFLOAT3 cameraPos;

		SceneNode* dirLight = scene_->GetDirectionalLight();

		if(!dirLight)
			return;

		if(shadowPass)
		{
			XMStoreFloat3(&cameraPos,XMLoadFloat4(&dirLight->GetPosition()));
			XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&dirLight->GetPosition()),XMLoadFloat4(&dirLight->GetDirection()),XMLoadFloat4(&dirLight->GetRotation()));

			view = lightView;
			proj = MakeProjection();
		}
		else
		{
			XMStoreFloat3(&cameraPos,camera->Position());
		}

		XMMATRIX vp = view * proj;
		XMMATRIX vpInv = XMMatrixInverse(nullptr,vp);


		gYume->pRHI->SetShaderParameter("vp",vp);
		gYume->pRHI->SetShaderParameter("view",view);
		gYume->pRHI->SetShaderParameter("vp_inv",vpInv);
		gYume->pRHI->SetShaderParameter("ProjMatrix",proj);
		gYume->pRHI->SetShaderParameter("camera_pos",cameraPos);
		gYume->pRHI->SetShaderParameter("camera_pos_ps",cameraPos); // ¯\_(ツ)_/¯
		gYume->pRHI->SetShaderParameter("z_far",zFar);

		DirectX::XMMATRIX tex
			(
			0.5f,0.0f,0.0f,0.0f,
			0.0f,-0.5f,0.0f,0.0f,
			0.f,0.f,0.5f,0.0f,
			0.5f,0.5f,0.5f,1.0f
			);

		rhi_->SetShaderParameter("viewToTextureSpaceMatrix",proj*tex);

		/*XMFLOAT3 n,f;
		camera_->GetFrustumSize(n,f);
		rhi_->SetShaderParameter("FrustumSize",f);*/
	}


	DirectX::XMMATRIX YumeMiscRenderer::MakeProjection()
	{
		float n = zNear;
		float f = zFar;

		float q = f/(f-n);

		return DirectX::XMMATRIX
			(
			1.f,0.f,0.f,0.f,
			0.f,1.f,0.f,0.f,
			0.f,0.f,q,1.f,
			0.f,0.f,-q*n,0.f
			);
	}

	void YumeMiscRenderer::HandlePostRenderUpdate(float timeStep)
	{
		Update(timeStep);
	}
}

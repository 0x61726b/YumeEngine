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
#include "StaticModel.h"

#include "Logging/logging.h"

#include "Core/YumeFile.h"
#include "YumeRHI.h"

#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"


namespace YumeEngine
{
	StaticModel::StaticModel(const YumeString& model)
		: SceneNode(GT_STATIC),modelName_(model)
	{
		LoadFromFile(model);
	}

	StaticModel::~StaticModel()
	{

	}

	bool StaticModel::LoadFromFile(const YumeString& file)
	{
		SharedPtr<YumeFile> f = gYume->pResourceManager->GetFile(file);


		YumeString fullPath = gYume->pResourceManager->GetFullPath(file);

		YumeString pathName,fileName,extension;
		SplitPath(fullPath,pathName,fileName,extension);

		SharedPtr<YumeFile> m(new YumeFile(pathName + fileName + ".material"));



		YumeString material = m->GetFileExtension();
		unsigned materialCount = m->ReadUInt();

		YumeString fileType = f->GetFileExtension();
		unsigned meshCount = f->ReadUInt();

		for(int i=0; i < meshCount; ++i)
		{
			//Read this mesh's vertex buffer

			unsigned vertexCount = f->ReadUInt();
			unsigned elementMask = f->ReadUInt();
			unsigned vertexSize = f->ReadUInt();


			SharedPtr<YumeVertexBuffer> vb(gYume->pRHI->CreateVertexBuffer());
			vb->SetShadowed(true);
			vb->SetSize(vertexCount,elementMask);

			void* vbbuffer = vb->Lock(0,vertexCount);
			f->Read(vbbuffer,vertexCount * vertexSize);
			vb->Unlock();

			unsigned indexCount = f->ReadUInt();
			unsigned indexSize = f->ReadUInt();

			SharedPtr<YumeIndexBuffer> ib(gYume->pRHI->CreateIndexBuffer());
			ib->SetShadowed(true);
			ib->SetSize(indexCount,true);
			void* ibbuffer = ib->Lock(0,indexCount);
			f->Read(ibbuffer,(indexCount) * indexSize);
			ib->Unlock();


			YumeGeometry* geo(new YumeGeometry);

			geo->SetVertexBuffer(0,vb);
			geo->SetIndexBuffer(ib);
			geo->SetDrawRange(TRIANGLE_LIST,0,ib->GetIndexCount());

			


			//Read bounding box

			float bbMaxX = f->ReadFloat();
			float bbMaxY = f->ReadFloat();
			float bbMaxZ = f->ReadFloat();

			float bbMinX = f->ReadFloat();
			float bbMinY= f->ReadFloat();
			float bbMinZ = f->ReadFloat();



			DirectX::XMFLOAT3 bbMin(bbMinX,bbMinY,bbMinZ);
			DirectX::XMFLOAT3 bbMax(bbMaxX,bbMaxY,bbMaxZ);

			geo->SetBoundingBox(bbMin,bbMax);

			DirectX::XMFLOAT4 diffuseColor = m->ReadVector4();
			DirectX::XMFLOAT4 emissiveColor = m->ReadVector4();
			DirectX::XMFLOAT4 specularColor = m->ReadVector4();
			float shadingmMode = m->ReadFloat();
			float roughness = m->ReadFloat();
			float refractiveIndex = m->ReadFloat();

			YumeString diffuse_tex = m->ReadString();
			YumeString alpha_tex = m->ReadString();
			YumeString emissive_tex = m->ReadString();
			YumeString specular_tex = m->ReadString();
			YumeString normal_tex = m->ReadString();

			SharedPtr<Material> material(new Material);

			material->SetShaderParameter("DiffuseColor",diffuseColor);
			material->SetShaderParameter("EmissiveColor",emissiveColor);
			material->SetShaderParameter("SpecularColor",specularColor);
			material->SetShaderParameter("ShadingMode",shadingmMode);
			material->SetShaderParameter("Roughness",roughness);


			material->SetShaderParameter("has_diffuse_tex",true);
			material->SetShaderParameter("has_alpha_tex",true);
			material->SetShaderParameter("has_specular_tex",true);
			material->SetShaderParameter("has_normal_tex",true);

			if(!material->HasTexture(diffuse_tex))
				material->SetShaderParameter("has_diffuse_tex",false);
			else
				material->SetTexture(MT_DIFFUSE,gYume->pResourceManager->PrepareResource<YumeTexture2D>(diffuse_tex));

			if(!material->HasTexture(alpha_tex))
				material->SetShaderParameter("has_alpha_tex",false);
			else
				material->SetTexture(MT_ALPHA,gYume->pResourceManager->PrepareResource<YumeTexture2D>(alpha_tex));

			if(!material->HasTexture(specular_tex))
				material->SetShaderParameter("has_specular_tex",false);
			else
				material->SetTexture(MT_SPECULAR,gYume->pResourceManager->PrepareResource<YumeTexture2D>(specular_tex));

			if(!material->HasTexture(normal_tex))
				material->SetShaderParameter("has_normal_tex",false);
			else
				material->SetTexture(MT_NORMAL,gYume->pResourceManager->PrepareResource<YumeTexture2D>(normal_tex));

			
			SharedPtr<RenderBatch> batch(new RenderBatch);
			batch->geo_ = geo;
			batch->material_ = material;
			batches_.push_back(batch);
		}

		return true;
	}

	void StaticModel::SetMaterial(MaterialPtr ptr)
	{
		for(int i=0; i < batches_.size(); ++i)
			batches_[i]->material_ = ptr;
	}

	void StaticModel::Initialize()
	{
		SetWorld(DirectX::XMMatrixIdentity());
	}
}

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
#include "AssimpMesh.h"

#include "YumeRHI.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"

#include "Logging/logging.h"

#include <assimp/ProgressHandler.hpp>


namespace YumeEngine
{
	namespace detail
	{
		DirectX::XMFLOAT3 aivec_to_dxvec3(aiVector3D v)
		{
			return DirectX::XMFLOAT3(v.x,v.y,v.z);
		}

		DirectX::XMFLOAT4 aivec_to_dxvec4(aiColor4D v)
		{
			return DirectX::XMFLOAT4(v.r,v.g,v.b,v.a);
		}

		DirectX::XMFLOAT2 aivec_to_dxvec2(aiVector3D v)
		{
			return DirectX::XMFLOAT2(v.x,v.y);
		}

		YumeString to_tstring(aiString s)
		{
			std::string xs(s.data,s.length);
			return YumeString(xs.c_str());
		}
	}

	assimp_mesh::assimp_mesh():
		importer_(),
		mesh_infos_(),
		indices_(),
		num_faces_(0),
		num_vertices_(0)
	{
	}

	const aiScene* const assimp_mesh::assimp_scene() const
	{
		return importer_.GetScene();
	}

	void assimp_mesh::destroy()
	{

	}

	size_t assimp_mesh::num_vertices()
	{
		size_t n = 0;

		for(size_t m = 0; m < assimp_scene()->mNumMeshes; ++m)
			n += assimp_scene()->mMeshes[m]->mNumVertices;

		return n;
	}

	size_t assimp_mesh::num_faces()
	{
		size_t n = 0;

		for(size_t m = 0; m < assimp_scene()->mNumMeshes; ++m)
			n += assimp_scene()->mMeshes[m]->mNumFaces;

		return n;
	}

	struct progress_handler : public Assimp::ProgressHandler
	{
		virtual bool Update(float percentage)
		{
			return true;
		}
	};

	void assimp_mesh::load(const YumeString& file)
	{
		YUMELOG_INFO("Loading Assimp Mesh " <<file.c_str());

		progress_handler ph;
		importer_.SetProgressHandler(&ph);

		const aiScene* scene = importer_.ReadFile(file.c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_MakeLeftHanded |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenSmoothNormals |
			//aiProcess_GenNormals |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_OptimizeMeshes |
			aiProcess_GenUVCoords |
			aiProcess_TransformUVCoords);

		if(!scene)
		{
			std::string error = importer_.GetErrorString();

			YUMELOG_ERROR("Failed loading " << file.c_str());
			return;
		}

		aiMatrix4x4 m;

		load_internal(scene,scene->mRootNode,m);

		importer_.SetProgressHandler(nullptr);

		//tclog << L"Mesh info: " << std::endl
		//      << L" - " << mesh_infos_.size() << L" meshes" << std::endl
		//      << L" - " << "BBOX (" 
		//                << bb_max().x << L", " 
		//                << bb_max().y << L", " 
		//                << bb_max().z << L")"
		//                << L" - (" 
		//                << bb_min().x << L", "
		//                << bb_min().y << L", " 
		//                << bb_min().z << L")"
		//                << std::endl
		//      << L" - " << num_faces_ << L" faces" << std::endl
		//      << L" - " << num_vertices_ << L" vertices" << std::endl;
	}

	void assimp_mesh::load_internal(const aiScene* scene,aiNode* node,aiMatrix4x4 acc_transform)
	{
		aiMatrix4x4 transform = acc_transform * node->mTransformation;

		// store all meshes, one after another
		for(size_t a = 0; a < node->mNumMeshes; ++a)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[a]];

			// store current read num_vertices_ and aiMesh for potential reference
			mesh_info m;
			m.vstart_index   = num_vertices_;
			m.istart_index   = static_cast<UINT>(indices_.size());
			m.num_faces      = mesh->mNumFaces;
			m.num_vertices   = mesh->mNumVertices;
			m.material_index = mesh->mMaterialIndex;

			num_vertices_ += mesh->mNumVertices;
			num_faces_    += mesh->mNumFaces;

			// store vertices
			for(size_t b = 0; b < mesh->mNumVertices; ++b)
			{
				assimp_mesh::vertex v;

				aiVector3D v_trans = transform * mesh->mVertices[b];

				v.position = detail::aivec_to_dxvec3(v_trans);

				// if this is the very first vertex
				if(m.vstart_index == 0 && b == 0)
					init_bb(v.position);
				else
					update_bb(v.position);

				if(mesh->HasNormals())
					v.normal = detail::aivec_to_dxvec3(mesh->mNormals[b]);

				if(mesh->HasTangentsAndBitangents())
					v.tangent = detail::aivec_to_dxvec3(mesh->mTangents[b]);

				for(size_t n = 0; n < mesh->GetNumUVChannels(); ++n)
				{
					v.texcoord[n] = detail::aivec_to_dxvec2(mesh->mTextureCoords[n][b]);
					v.texcoord[n].y = 1.f - v.texcoord[n].y;
				}

				push_back(v);

				m.update(v.position,b == 0);
			}

			// store indices, corrected by startIndex, and attribute
			for(size_t b = 0; b < mesh->mNumFaces; ++b)
			{
				indices_.push_back(mesh->mFaces[b].mIndices[2]);
				indices_.push_back(mesh->mFaces[b].mIndices[1]);
				indices_.push_back(mesh->mFaces[b].mIndices[0]);
			}

			mesh_infos_.push_back(m);
		}

		for(size_t c = 0; c < node->mNumChildren; ++c)
			assimp_mesh::load_internal(scene,node->mChildren[c],transform);
	}

	YumeMesh::YumeMesh()
	{
	}

	YumeMesh::~YumeMesh()
	{

	}
	void YumeMesh::push_back(vertex v)
	{
		YumeVertex gv;

		gv.normal = v.normal;
		gv.position = v.position;
		gv.texcoord = v.texcoord[0];
		gv.tangent = v.tangent;

		vertices_.push_back(gv);
	}

	bool YumeMesh::Load(const YumeString& fileName)
	{
		assimp_mesh::load(fileName);

		file_ = fileName;

		return PrepareMesh();
	}

	bool YumeMesh::PrepareMesh()
	{
		YumeString path,fileName,extension;
		SplitPath(file_,path,fileName,extension);

		for(auto i = mesh_infos_.begin(); i != mesh_infos_.end(); ++i)
		{
			UINT num_vertices = i->num_vertices;
			UINT num_faces = i->num_faces;

			if(num_faces == 0 || num_vertices == 0)
			{
				YUMELOG_INFO("Skipping invalid mesh with " <<
					num_vertices << " vertices and " <<
					num_faces << " faces" << std::endl);
				continue;
			}

			SharedPtr<YumeGeometry> geo(new YumeGeometry);

			mesh_data mesh;
			ZeroMemory(&mesh,sizeof(mesh));

			SharedPtr<YumeVertexBuffer> vb(gYume->pRHI->CreateVertexBuffer());

			vb->SetShadowed(true);
			vb->SetSize(num_vertices,MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1 | MASK_TANGENT);
			vb->SetData(&vertices_[i->vstart_index]);

			SharedPtr<YumeIndexBuffer> ib(gYume->pRHI->CreateIndexBuffer());
			ib->SetShadowed(true);
			ib->SetSize(num_faces * 3,true);
			ib->SetData(&indices_[i->istart_index]);

			geo->SetVertexBuffer(0,vb);
			geo->SetIndexBuffer(ib);
			geo->SetDrawRange(TRIANGLE_LIST,0,ib->GetIndexCount());


			aiMaterial* mat = assimp_scene()->mMaterials[i->material_index];

			aiColor4D color;

			// grab diffuse color
			mesh.diffuse_color = DirectX::XMFLOAT4(0.f,0.f,0.f,0.f);
			if(mat->Get(AI_MATKEY_COLOR_DIFFUSE,color) == AI_SUCCESS)
				mesh.diffuse_color = detail::aivec_to_dxvec4(color);

			float opacity = 0;
			if(mat->Get(AI_MATKEY_OPACITY,opacity) == AI_SUCCESS)
				mesh.diffuse_color.w = opacity;

			// grab specular color
			mesh.specular_color = DirectX::XMFLOAT4(0.f,0.f,0.f,0.f);
			if(mat->Get(AI_MATKEY_COLOR_SPECULAR,color) == AI_SUCCESS)
				mesh.specular_color = detail::aivec_to_dxvec4(color);

			// grab emissive color
			mesh.emissive_color = DirectX::XMFLOAT4(0.f,0.f,0.f,0.f);
			if(mat->Get(AI_MATKEY_COLOR_EMISSIVE,color) == AI_SUCCESS)
				mesh.emissive_color = detail::aivec_to_dxvec4(color);

			// get the shading mode -> abuse as basic material shading map
			int shading_mode;

			mesh.shading_mode = SHADING_DEFAULT;

			if(mat->Get(AI_MATKEY_SHADING_MODEL,shading_mode) == AI_SUCCESS)
			{
				// convert back, don't care about the specific expected implementation
				switch(shading_mode)
				{
				case 9:  mesh.shading_mode = SHADING_OFF;       break;
				case 3:  mesh.shading_mode = SHADING_DEFAULT;   break;
				case 2:  mesh.shading_mode = SHADING_IBL;       break;

				default: mesh.shading_mode = SHADING_DEFAULT;   break;
				};

				if(mesh.emissive_color.x + mesh.emissive_color.y + mesh.emissive_color.z > 0)
					mesh.shading_mode = SHADING_AREA_LIGHT;
			}

			// get specular exponent
			// WARNING: OBJ specifies shininess = Ns * 4
			// convert shininess to roughness first
			float shininess;

			mesh.roughness = 0.0f;
			if(mat->Get(AI_MATKEY_SHININESS,shininess) == AI_SUCCESS)
				mesh.roughness = std::sqrtf(std::sqrtf(2.0f / (shininess + 2.0f)));

			// get refractive index
			float refractive_index;

			mesh.refractive_index = 1.5;
			if(mat->Get(AI_MATKEY_REFRACTI,refractive_index) == AI_SUCCESS)
				mesh.refractive_index = refractive_index;

			aiString str;


			str.Clear();
			if(mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0),str) == AI_SUCCESS)
				mesh.diffuse_tex = gYume->pResourceManager->PrepareResource<YumeTexture2D>(YumeString(str.C_Str()));

			str.Clear();
			if(mat->Get(AI_MATKEY_TEXTURE_EMISSIVE(0),str) == AI_SUCCESS)
				mesh.emissive_tex = gYume->pResourceManager->PrepareResource<YumeTexture2D>(YumeString(str.C_Str()));

			str.Clear();
			if(mat->Get(AI_MATKEY_TEXTURE_SPECULAR(0),str) == AI_SUCCESS)
				mesh.specular_tex = gYume->pResourceManager->PrepareResource<YumeTexture2D>(YumeString(str.C_Str()));

			str.Clear();
			//mat->Get(AI_MATKEY_TEXTURE_NORMALS(0), str);
			if(mat->Get(AI_MATKEY_TEXTURE_HEIGHT(0),str) == AI_SUCCESS)
				mesh.normal_tex = gYume->pResourceManager->PrepareResource<YumeTexture2D>(YumeString(str.C_Str()));

			str.Clear();
			if(mat->Get(AI_MATKEY_TEXTURE_OPACITY(0),str) == AI_SUCCESS)
				mesh.alpha_tex = gYume->pResourceManager->PrepareResource<YumeTexture2D>(YumeString(str.C_Str()));

			meshes_.push_back(mesh);
			geometries_.push_back(geo);

		}
		importer_.FreeScene();

		return true;
	}

	void YumeMesh::Render()
	{
		mesh_data* prev = nullptr;

		for(int m=0; m < meshes_.size(); ++m)
		{
			mesh_data* data = &meshes_[m];
			mesh_info* info = &mesh_infos_[m];

			bool same = (prev &&
				dequal(prev->diffuse_color,data->diffuse_color) &&
				dequal(prev->specular_color,data->specular_color) &&
				dequal(prev->emissive_color,data->emissive_color) &&
				prev->diffuse_tex == data->diffuse_tex &&
				prev->specular_tex == data->specular_tex &&
				prev->normal_tex == data->normal_tex &&
				prev->specular_tex == data->specular_tex &&
				prev->alpha_tex == data->alpha_tex &&
				prev->shading_mode == data->shading_mode &&
				prev->roughness == data->roughness
				);

			if(!same)
			{
				const bool has_diffuse_tex = data->diffuse_tex != nullptr;
				const bool has_normal_tex = data->normal_tex != nullptr;
				const bool has_specular_tex = data->specular_tex != nullptr;
				const bool has_alpha_tex = data->alpha_tex != nullptr;

				Texture2DPtr random = gYume->pResourceManager->PrepareResource<YumeTexture2D>("Textures/test/test.jpg");

				gYume->pRHI->SetShaderParameter("diffuse_color",data->diffuse_color);
				gYume->pRHI->SetShaderParameter("specular_color",data->specular_color);
				gYume->pRHI->SetShaderParameter("emissive_color",data->emissive_color);
				gYume->pRHI->SetShaderParameter("has_diffuse_tex",true);
				gYume->pRHI->SetShaderParameter("has_normal_tex",has_normal_tex);
				gYume->pRHI->SetShaderParameter("has_specular_tex",has_specular_tex);
				gYume->pRHI->SetShaderParameter("has_alpha_tex",has_alpha_tex);
				gYume->pRHI->SetShaderParameter("shading_mode",data->shading_mode);
				gYume->pRHI->SetShaderParameter("roughness",data->roughness);
				gYume->pRHI->SetShaderParameter("refractive_index",data->refractive_index);


				/*if(has_diffuse_tex)
				{*/
				/*TexturePtr texture[] ={data->diffuse_tex.Get()};*/
				TexturePtr texture[] ={random};
				gYume->pRHI->PSBindSRV(0,1,texture);
				/*}*/

				if(has_normal_tex)
				{
					TexturePtr texture[] ={data->normal_tex.Get()};
					gYume->pRHI->PSBindSRV(1,1,texture);
				}

				if(has_specular_tex)
				{
					TexturePtr texture[] ={data->specular_tex.Get()};
					gYume->pRHI->PSBindSRV(2,1,texture);
				}

				if(has_alpha_tex)
				{
					TexturePtr texture[] ={data->alpha_tex.Get()};
					gYume->pRHI->PSBindSRV(3,1,texture);
				}
			}

			geometries_[m]->Draw(gYume->pRHI);
		}
	}

	void YumeMesh::RenderDirect(unsigned index)
	{

	}
}

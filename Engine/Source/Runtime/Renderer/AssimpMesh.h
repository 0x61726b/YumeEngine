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
#ifndef __AssimpMesh_h__
#define __AssimpMesh_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "YumeGeometry.h"

#include "YumeMesh.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class assimp_mesh : public mesh<DirectX::XMFLOAT3>
	{
	public:
		/*! \brief A vertex with all its attributes as loaded by Assimp. */
		struct vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT3 tangent;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texcoord[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		};

	protected:
		/*! \brief A helper struct to supply information about submeshes loaded by Assimp. */
		struct mesh_info : public aabb<DirectX::XMFLOAT3>
		{
			UINT vstart_index;
			UINT istart_index;
			UINT num_faces;
			UINT num_vertices;
			UINT material_index;

			void update(const DirectX::XMFLOAT3& v,bool first)
			{
				if(first)
					init_bb(v);
				else
					update_bb(v);
			}
		};

	protected:
		Assimp::Importer importer_;
		std::vector<mesh_info> mesh_infos_;
		std::vector<unsigned int> indices_;

	private:
		// warning: do not confuse with functions num_vertices() and num_faces()
		UINT num_faces_;
		UINT num_vertices_;

	protected:
		/*! \brief Push back a new vertex: this member needs to be overloaded in derived assimp_mesh objects. */
		virtual void push_back(vertex v) = 0;

		virtual void load_internal(const aiScene* scene,aiNode* node,aiMatrix4x4 transform);

		void load(const YumeString& file);

	public:
		assimp_mesh();
		virtual ~assimp_mesh() {};

		const aiScene* const assimp_scene() const;
		virtual void destroy();

		size_t num_vertices();
		size_t num_faces();
	};

	class YumeAPIExport YumeMesh : public assimp_mesh,public YumeGeometry
	{
	public:
		YumeMesh();
		virtual ~YumeMesh();

		bool Load(const YumeString& fileName);

		bool PrepareMesh();
		
		void Render();

		virtual void push_back(vertex v);

		enum
		{
			SHADING_OFF,
			SHADING_DEFAULT,
			SHADING_IBL,
			SHADING_AREA_LIGHT
		};


		struct YumeVertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;
			DirectX::XMFLOAT3 tangent;
		};

		struct mesh_data_vs
		{
			DirectX::XMFLOAT4X4 world;
		};

		/*! \brief Mesh constant buffer data uploaded to the pixel shader. */
		struct mesh_data_ps
		{
			DirectX::XMFLOAT4 diffuse_color;
			DirectX::XMFLOAT4 specular_color;
			DirectX::XMFLOAT4 emissive_color;
			bool has_diffuse_tex;
			bool has_normal_tex;
			bool has_specular_tex;
			bool has_alpha_tex;
			float shading_mode;
			float roughness;
			float refractive_index;
			float pad;
		};

		/*! \brief Mesh data for CPU side useage. */
		struct mesh_data
		{
			SharedPtr<YumeIndexBuffer> index;
			SharedPtr<YumeVertexBuffer> vertex;
			DirectX::XMFLOAT4 diffuse_color;
			DirectX::XMFLOAT4 specular_color;
			DirectX::XMFLOAT4 emissive_color;
			SharedPtr<YumeTexture2D> diffuse_tex;
			SharedPtr<YumeTexture2D> emissive_tex;
			SharedPtr<YumeTexture2D> specular_tex;
			SharedPtr<YumeTexture2D> normal_tex;
			SharedPtr<YumeTexture2D> alpha_tex;
			float shading_mode;
			float roughness;
			float refractive_index;
		};

	protected:
		YumeVector<YumeVertex>::type vertices_;
		YumeVector<mesh_data>::type meshes_;
		YumeVector<SharedPtr<YumeGeometry> >::type geometries_;
		YumeString file_;
	};
}


//----------------------------------------------------------------------------
#endif

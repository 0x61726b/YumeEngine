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
#ifndef __YumeMesh_h__
#define __YumeMesh_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <DirectXMath.h>
#include <d3d11.h>

#include <memory>
#include <cassert>

#include "Core/PlatformImpl.h"

#include "Math/YumeMath.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	template<typename V>
	struct aabb
	{
	protected:
		V bb_max_;
		V bb_min_;
		DirectX::XMFLOAT4X4 world_;

	protected:
		/*!
		 * \brief Initialize the bounding-box with a first point.
		 *
		 * Before using the bounding box, it must first be intialized with a single point.
		 *
		 * \param p The first point of a larger structure.
		 */
			inline void init_bb(const V& p)
			{
				bb_max_ = bb_min_ = p;
			}

		/*!
		 * \brief Update the bounding-box with a new point.
		 *
		 * Updates the bounding-box with an additional point and recalculates its
		 * maximum and minimum extents.
		 *
		 * \param p A new point.
		 */
		void update_bb(const V& p)
		{
			bb_max_ = dmax(p,bb_max_);
			bb_min_ = dmin(p,bb_min_);
		}

	public:
		/*! \brief Returns the center of the bounding-box. */
		inline V center() const { return bb_min_ + (bb_max_ - bb_min_)/2.0f; };

		/*! \brief Returns the maximum (upper/right/back) of the bounding-box. */
		inline V bb_max() const { return bb_max_; };

		/*! \brief Returns the minimum (lower/left/front) of the bounding-box. */
		inline V bb_min() const { return bb_min_; };

		/*! \brief Update the world matrix of this mesh. */
		inline virtual void set_world(const DirectX::XMFLOAT4X4& world)
		{
			world_ = world;
		}

		/*! \brief Returns the current world matrix of this mesh. */
		inline const DirectX::XMFLOAT4X4& world()
		{
			return world_;
		}
	};

	bool is_visible(const aabb<DirectX::XMFLOAT3>& bbox,const DirectX::XMFLOAT4X4& to_clip);

	const D3D11_INPUT_ELEMENT_DESC standard_vertex_desc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		0
	};

	template<typename V>
	struct mesh : public aabb<V>
	{
	public:
		/*! \brief Returns the number of vertices of a mesh. */
		virtual size_t num_vertices() = 0;

		/*! \brief Returns the number of faces of a mesh. */
		virtual size_t num_faces() = 0;
	};
}


//----------------------------------------------------------------------------
#endif

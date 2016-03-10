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
#ifndef __YumePolyhedron_h__
#define __YumePolyhedron_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeVector3.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class BoundingBox;
	class Frustum;
	class Matrix3;
	class Matrix3x4;
	class Plane;

	
	class YumeAPIExport Polyhedron
	{
	public:
		
		Polyhedron()
		{
		}

		
		Polyhedron(const Polyhedron& polyhedron):
			faces_(polyhedron.faces_)
		{
		}

		
		Polyhedron(const YumeVector<YumeVector<Vector3>::type >::type& faces):
			faces_(faces)
		{
		}

		
		Polyhedron(const BoundingBox& box)
		{
			Define(box);
		}

		
		Polyhedron(const Frustum& frustum)
		{
			Define(frustum);
		}

		
		~Polyhedron();

		
		Polyhedron& operator =(const Polyhedron& rhs)
		{
			faces_ = rhs.faces_;
			return *this;
		}

		
		void Define(const BoundingBox& box);
		
		void Define(const Frustum& frustum);
		
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2);
		
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		
		void AddFace(const YumeVector<Vector3>::type& face);
		
		void Clip(const Plane& plane);
		
		void Clip(const BoundingBox& box);
		
		void Clip(const Frustum& box);
		
		void Clear();
		
		void Transform(const Matrix3& transform);
		
		void Transform(const Matrix3x4& transform);

		
		Polyhedron Transformed(const Matrix3& transform) const;
		
		Polyhedron Transformed(const Matrix3x4& transform) const;

		
		bool Empty() const { return faces_.empty(); }

		
		YumeVector<YumeVector<Vector3>::type >::type faces_;

	private:
		
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2);
		
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		
		YumeVector<Vector3>::type clippedVertices_;
		
		YumeVector<Vector3>::type outFace_;
	};
}


//----------------------------------------------------------------------------
#endif

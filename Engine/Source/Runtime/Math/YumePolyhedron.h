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

	/// A convex volume built from polygon faces.
	class YumeAPIExport Polyhedron
	{
	public:
		/// Construct empty.
		Polyhedron()
		{
		}

		/// Copy-construct from another polyhedron.
		Polyhedron(const Polyhedron& polyhedron):
			faces_(polyhedron.faces_)
		{
		}

		/// Construct from a list of faces.
		Polyhedron(const YumeVector<YumeVector<Vector3>::type >::type& faces):
			faces_(faces)
		{
		}

		/// Construct from a bounding box.
		Polyhedron(const BoundingBox& box)
		{
			Define(box);
		}

		/// Construct from a frustum.
		Polyhedron(const Frustum& frustum)
		{
			Define(frustum);
		}

		/// Destruct.
		~Polyhedron();

		/// Assign from another polyhedron.
		Polyhedron& operator =(const Polyhedron& rhs)
		{
			faces_ = rhs.faces_;
			return *this;
		}

		/// Define from a bounding box.
		void Define(const BoundingBox& box);
		/// Define from a frustum.
		void Define(const Frustum& frustum);
		/// Add a triangle face.
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2);
		/// Add a quadrilateral face.
		void AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		/// Add an arbitrary face.
		void AddFace(const YumeVector<Vector3>::type& face);
		/// Clip with a plane.
		void Clip(const Plane& plane);
		/// Clip with a bounding box.
		void Clip(const BoundingBox& box);
		/// Clip with a frustum.
		void Clip(const Frustum& box);
		/// Clear all faces.
		void Clear();
		/// Transform with a 3x3 matrix.
		void Transform(const Matrix3& transform);
		/// Transform with a 3x4 matrix.
		void Transform(const Matrix3x4& transform);

		/// Return transformed with a 3x3 matrix.
		Polyhedron Transformed(const Matrix3& transform) const;
		/// Return transformed with a 3x4 matrix.
		Polyhedron Transformed(const Matrix3x4& transform) const;

		/// Return whether is empty.
		bool Empty() const { return faces_.empty(); }

		/// Polygon faces.
		YumeVector<YumeVector<Vector3>::type >::type faces_;

	private:
		/// Set a triangle face by index.
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2);
		/// Set a quadrilateral face by index.
		void SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3);
		/// Internal vector for clipped vertices.
		YumeVector<Vector3>::type clippedVertices_;
		/// Internal vector for the new face being constructed.
		YumeVector<Vector3>::type outFace_;
	};
}


//----------------------------------------------------------------------------
#endif

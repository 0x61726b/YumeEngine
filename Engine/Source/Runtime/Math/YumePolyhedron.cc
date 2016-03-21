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
#include "YumePolyhedron.h"
#include "YumeFrustum.h"



namespace YumeEngine
{

	Polyhedron::~Polyhedron()
	{
	}

	void Polyhedron::Define(const BoundingBox& box)
	{
		Vector3 vertices[8];
		vertices[0] = box.min_;
		vertices[1] = Vector3(box.max_.x_,box.min_.y_,box.min_.z_);
		vertices[2] = Vector3(box.min_.x_,box.max_.y_,box.min_.z_);
		vertices[3] = Vector3(box.max_.x_,box.max_.y_,box.min_.z_);
		vertices[4] = Vector3(box.min_.x_,box.min_.y_,box.max_.z_);
		vertices[5] = Vector3(box.max_.x_,box.min_.y_,box.max_.z_);
		vertices[6] = Vector3(box.min_.x_,box.max_.y_,box.max_.z_);
		vertices[7] = box.max_;

		faces_.resize(6);
		SetFace(0,vertices[3],vertices[7],vertices[5],vertices[1]);
		SetFace(1,vertices[6],vertices[2],vertices[0],vertices[4]);
		SetFace(2,vertices[6],vertices[7],vertices[3],vertices[2]);
		SetFace(3,vertices[1],vertices[5],vertices[4],vertices[0]);
		SetFace(4,vertices[7],vertices[6],vertices[4],vertices[5]);
		SetFace(5,vertices[2],vertices[3],vertices[1],vertices[0]);
	}

	void Polyhedron::Define(const Frustum& frustum)
	{
		const Vector3* vertices = frustum.vertices_;

		faces_.resize(6);
		SetFace(0,vertices[0],vertices[4],vertices[5],vertices[1]);
		SetFace(1,vertices[7],vertices[3],vertices[2],vertices[6]);
		SetFace(2,vertices[7],vertices[4],vertices[0],vertices[3]);
		SetFace(3,vertices[1],vertices[5],vertices[6],vertices[2]);
		SetFace(4,vertices[4],vertices[7],vertices[6],vertices[5]);
		SetFace(5,vertices[3],vertices[0],vertices[1],vertices[2]);
	}

	void Polyhedron::AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2)
	{
		faces_.resize(faces_.size() + 1);
		YumeVector<Vector3>::type& face = faces_[faces_.size() - 1];
		face.resize(3);
		face[0] = v0;
		face[1] = v1;
		face[2] = v2;
	}

	void Polyhedron::AddFace(const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3)
	{
		faces_.resize(faces_.size() + 1);
		YumeVector<Vector3>::type& face = faces_[faces_.size() - 1];
		face.resize(4);
		face[0] = v0;
		face[1] = v1;
		face[2] = v2;
		face[3] = v3;
	}

	void Polyhedron::AddFace(const YumeVector<Vector3>::type& face)
	{
		faces_.push_back(face);
	}

	void Polyhedron::Clip(const Plane& plane)
	{
		clippedVertices_.clear();

		for(unsigned i = 0; i < faces_.size(); ++i)
		{
			YumeVector<Vector3>::type& face = faces_[i];
			Vector3 lastVertex;
			float lastDistance = 0.0f;

			outFace_.clear();

			for(unsigned j = 0; j < face.size(); ++j)
			{
				float distance = plane.Distance(face[j]);
				if(distance >= 0.0f)
				{
					if(lastDistance < 0.0f)
					{
						float t = lastDistance / (lastDistance - distance);
						Vector3 clippedVertex = lastVertex + t * (face[j] - lastVertex);
						outFace_.push_back(clippedVertex);
						clippedVertices_.push_back(clippedVertex);
					}

					outFace_.push_back(face[j]);
				}
				else
				{
					if(lastDistance >= 0.0f && j != 0)
					{
						float t = lastDistance / (lastDistance - distance);
						Vector3 clippedVertex = lastVertex + t * (face[j] - lastVertex);
						outFace_.push_back(clippedVertex);
						clippedVertices_.push_back(clippedVertex);
					}
				}

				lastVertex = face[j];
				lastDistance = distance;
			}

			// Recheck the distances of the last and first vertices and add the final clipped vertex if applicable
			float distance = plane.Distance(face[0]);
			if((lastDistance < 0.0f && distance >= 0.0f) || (lastDistance >= 0.0f && distance < 0.0f))
			{
				float t = lastDistance / (lastDistance - distance);
				Vector3 clippedVertex = lastVertex + t * (face[0] - lastVertex);
				outFace_.push_back(clippedVertex);
				clippedVertices_.push_back(clippedVertex);
			}

			// Do not keep faces which are less than triangles
			if(outFace_.size() < 3)
				outFace_.clear();

			face = outFace_;
		}

		// Remove empty faces
		for(unsigned i = faces_.size() - 1; i < faces_.size(); --i)
		{
			if(faces_[i].empty())
				faces_.erase(i);
		}

		// Create a new face from the clipped vertices. First remove duplicates
		for(unsigned i = 0; i < clippedVertices_.size(); ++i)
		{
			for(unsigned j = clippedVertices_.size() - 1; j > i; --j)
			{
				if(clippedVertices_[j].Equals(clippedVertices_[i]))
					clippedVertices_.erase(j);
			}
		}

		if(clippedVertices_.size() > 3)
		{
			outFace_.clear();

			// Start with the first vertex
			outFace_.push_back(clippedVertices_.front());
			clippedVertices_.erase(clippedVertices_.begin());

			while(!clippedVertices_.empty())
			{
				// Then add the vertex which is closest to the last added
				const Vector3& lastAdded = outFace_.back();
				float bestDistance = M_INFINITY;
				unsigned bestIndex = 0;

				for(unsigned i = 0; i < clippedVertices_.size(); ++i)
				{
					float distance = (clippedVertices_[i] - lastAdded).LengthSquared();
					if(distance < bestDistance)
					{
						bestDistance = distance;
						bestIndex = i;
					}
				}

				outFace_.push_back(clippedVertices_[bestIndex]);
				clippedVertices_.erase(bestIndex);
			}

			faces_.push_back(outFace_);
		}
	}

	void Polyhedron::Clip(const Frustum& frustum)
	{
		for(unsigned i = 0; i < NUM_FRUSTUM_PLANES; ++i)
			Clip(frustum.planes_[i]);
	}

	void Polyhedron::Clip(const BoundingBox& box)
	{
		Vector3 vertices[8];
		vertices[0] = box.min_;
		vertices[1] = Vector3(box.max_.x_,box.min_.y_,box.min_.z_);
		vertices[2] = Vector3(box.min_.x_,box.max_.y_,box.min_.z_);
		vertices[3] = Vector3(box.max_.x_,box.max_.y_,box.min_.z_);
		vertices[4] = Vector3(box.min_.x_,box.min_.y_,box.max_.z_);
		vertices[5] = Vector3(box.max_.x_,box.min_.y_,box.max_.z_);
		vertices[6] = Vector3(box.min_.x_,box.max_.y_,box.max_.z_);
		vertices[7] = box.max_;

		Clip(Plane(vertices[5],vertices[7],vertices[3]));
		Clip(Plane(vertices[0],vertices[2],vertices[6]));
		Clip(Plane(vertices[3],vertices[7],vertices[6]));
		Clip(Plane(vertices[4],vertices[5],vertices[1]));
		Clip(Plane(vertices[4],vertices[6],vertices[7]));
		Clip(Plane(vertices[1],vertices[3],vertices[2]));
	}

	void Polyhedron::Clear()
	{
		faces_.clear();
	}

	void Polyhedron::Transform(const Matrix3& transform)
	{
		for(unsigned i = 0; i < faces_.size(); ++i)
		{
			YumeVector<Vector3>::type& face = faces_[i];
			for(unsigned j = 0; j < face.size(); ++j)
				face[j] = transform * face[j];
		}
	}

	void Polyhedron::Transform(const Matrix3x4& transform)
	{
		for(unsigned i = 0; i < faces_.size(); ++i)
		{
			YumeVector<Vector3>::type& face = faces_[i];
			for(unsigned j = 0; j < face.size(); ++j)
				face[j] = transform * face[j];
		}
	}

	Polyhedron Polyhedron::Transformed(const Matrix3& transform) const
	{
		Polyhedron ret;
		ret.faces_.resize(faces_.size());

		for(unsigned i = 0; i < faces_.size(); ++i)
		{
			const YumeVector<Vector3>::type& face = faces_[i];
			YumeVector<Vector3>::type& newFace = ret.faces_[i];
			newFace.resize(face.size());

			for(unsigned j = 0; j < face.size(); ++j)
				newFace[j] = transform * face[j];
		}

		return ret;
	}

	Polyhedron Polyhedron::Transformed(const Matrix3x4& transform) const
	{
		Polyhedron ret;
		ret.faces_.resize(faces_.size());

		for(unsigned i = 0; i < faces_.size(); ++i)
		{
			const YumeVector<Vector3>::type& face = faces_[i];
			YumeVector<Vector3>::type& newFace = ret.faces_[i];
			newFace.resize(face.size());

			for(unsigned j = 0; j < face.size(); ++j)
				newFace[j] = transform * face[j];
		}

		return ret;
	}

	void Polyhedron::SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2)
	{
		YumeVector<Vector3>::type& face = faces_[index];
		face.resize(3);
		face[0] = v0;
		face[1] = v1;
		face[2] = v2;
	}

	void Polyhedron::SetFace(unsigned index,const Vector3& v0,const Vector3& v1,const Vector3& v2,const Vector3& v3)
	{
		YumeVector<Vector3>::type& face = faces_[index];
		face.resize(4);
		face[0] = v0;
		face[1] = v1;
		face[2] = v2;
		face[3] = v3;
	}
}

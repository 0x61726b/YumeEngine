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
#include "YumeSphere.h"
#include "YumeBoundingBox.h"
#include "YumeFrustum.h"
#include "YumePolyhedron.h"

namespace YumeEngine
{

	void Sphere::Define(const Vector3* vertices,unsigned count)
	{
		if(!count)
			return;

		Clear();
		Merge(vertices,count);
	}

	void Sphere::Define(const BoundingBox& box)
	{
		const Vector3& min = box.min_;
		const Vector3& max = box.max_;

		Clear();
		Merge(min);
		Merge(Vector3(max.x_,min.y_,min.z_));
		Merge(Vector3(min.x_,max.y_,min.z_));
		Merge(Vector3(max.x_,max.y_,min.z_));
		Merge(Vector3(min.x_,min.y_,max.z_));
		Merge(Vector3(max.x_,min.y_,max.z_));
		Merge(Vector3(min.x_,max.y_,max.z_));
		Merge(max);
	}

	void Sphere::Define(const Frustum& frustum)
	{
		Define(frustum.vertices_,NUM_FRUSTUM_VERTICES);
	}

	void Sphere::Define(const Polyhedron& poly)
	{
		Clear();
		Merge(poly);
	}

	void Sphere::Merge(const Vector3* vertices,unsigned count)
	{
		while(count--)
			Merge(*vertices++);
	}

	void Sphere::Merge(const BoundingBox& box)
	{
		const Vector3& min = box.min_;
		const Vector3& max = box.max_;

		Merge(min);
		Merge(Vector3(max.x_,min.y_,min.z_));
		Merge(Vector3(min.x_,max.y_,min.z_));
		Merge(Vector3(max.x_,max.y_,min.z_));
		Merge(Vector3(min.x_,min.y_,max.z_));
		Merge(Vector3(max.x_,min.y_,max.z_));
		Merge(Vector3(min.x_,max.y_,max.z_));
		Merge(max);
	}

	void Sphere::Merge(const Frustum& frustum)
	{
		const Vector3* vertices = frustum.vertices_;
		Merge(vertices,NUM_FRUSTUM_VERTICES);
	}

	void Sphere::Merge(const Polyhedron& poly)
	{
		for(unsigned i = 0; i < poly.faces_.size(); ++i)
		{
			const YumeVector<Vector3>::type& face = poly.faces_[i];
			if(!face.empty())
				Merge(&face[0],face.size());
		}
	}

	void Sphere::Merge(const Sphere& sphere)
	{
		if(radius_ < 0.0f)
		{
			center_ = sphere.center_;
			radius_ = sphere.radius_;
			return;
		}

		Vector3 offset = sphere.center_ - center_;
		float dist = offset.Length();

		// If sphere fits inside, do nothing
		if(dist + sphere.radius_ < radius_)
			return;

		// If we fit inside the other sphere, become it
		if(dist + radius_ < sphere.radius_)
		{
			center_ = sphere.center_;
			radius_ = sphere.radius_;
		}
		else
		{
			Vector3 NormalizedOffset = offset / dist;

			Vector3 min = center_ - radius_ * NormalizedOffset;
			Vector3 max = sphere.center_ + sphere.radius_ * NormalizedOffset;
			center_ = (min + max) * 0.5f;
			radius_ = (max - center_).Length();
		}
	}

	Intersection Sphere::IsInside(const BoundingBox& box) const
	{
		float radiusSquared = radius_ * radius_;
		float distSquared = 0;
		float temp;
		Vector3 min = box.min_;
		Vector3 max = box.max_;

		if(center_.x_ < min.x_)
		{
			temp = center_.x_ - min.x_;
			distSquared += temp * temp;
		}
		else if(center_.x_ > max.x_)
		{
			temp = center_.x_ - max.x_;
			distSquared += temp * temp;
		}
		if(center_.y_ < min.y_)
		{
			temp = center_.y_ - min.y_;
			distSquared += temp * temp;
		}
		else if(center_.y_ > max.y_)
		{
			temp = center_.y_ - max.y_;
			distSquared += temp * temp;
		}
		if(center_.z_ < min.z_)
		{
			temp = center_.z_ - min.z_;
			distSquared += temp * temp;
		}
		else if(center_.z_ > max.z_)
		{
			temp = center_.z_ - max.z_;
			distSquared += temp * temp;
		}

		if(distSquared >= radiusSquared)
			return OUTSIDE;

		min -= center_;
		max -= center_;

		Vector3 tempVec = min; // - - -
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.x_ = max.x_; // + - -
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.y_ = max.y_; // + + -
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.x_ = min.x_; // - + -
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.z_ = max.z_; // - + +
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.y_ = min.y_; // - - +
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.x_ = max.x_; // + - +
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;
		tempVec.y_ = max.y_; // + + +
		if(tempVec.LengthSquared() >= radiusSquared)
			return INTERSECTS;

		return INSIDE;
	}

	Intersection Sphere::IsInsideFast(const BoundingBox& box) const
	{
		float radiusSquared = radius_ * radius_;
		float distSquared = 0;
		float temp;
		Vector3 min = box.min_;
		Vector3 max = box.max_;

		if(center_.x_ < min.x_)
		{
			temp = center_.x_ - min.x_;
			distSquared += temp * temp;
		}
		else if(center_.x_ > max.x_)
		{
			temp = center_.x_ - max.x_;
			distSquared += temp * temp;
		}
		if(center_.y_ < min.y_)
		{
			temp = center_.y_ - min.y_;
			distSquared += temp * temp;
		}
		else if(center_.y_ > max.y_)
		{
			temp = center_.y_ - max.y_;
			distSquared += temp * temp;
		}
		if(center_.z_ < min.z_)
		{
			temp = center_.z_ - min.z_;
			distSquared += temp * temp;
		}
		else if(center_.z_ > max.z_)
		{
			temp = center_.z_ - max.z_;
			distSquared += temp * temp;
		}

		if(distSquared >= radiusSquared)
			return OUTSIDE;
		else
			return INSIDE;
	}
}

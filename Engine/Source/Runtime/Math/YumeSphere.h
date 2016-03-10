#pragma once




























#ifndef __YumeSphere_h__
#define __YumeSphere_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVector3.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{

	class BoundingBox;
	class Polyhedron;
	class Frustum;

	
	class YumeAPIExport Sphere
	{
	public:
		
		Sphere():
			center_(Vector3::ZERO),
			radius_(-M_INFINITY)
		{
		}

		
		Sphere(const Sphere& sphere):
			center_(sphere.center_),
			radius_(sphere.radius_)
		{
		}

		
		Sphere(const Vector3& center,float radius):
			center_(center),
			radius_(radius)
		{
		}

		
		Sphere(const Vector3* vertices,unsigned count)
		{
			Define(vertices,count);
		}

		
		Sphere(const BoundingBox& box)
		{
			Define(box);
		}

		
		Sphere(const Frustum& frustum)
		{
			Define(frustum);
		}

		
		Sphere(const Polyhedron& poly)
		{
			Define(poly);
		}

		
		Sphere& operator =(const Sphere& rhs)
		{
			center_ = rhs.center_;
			radius_ = rhs.radius_;
			return *this;
		}

		
		bool operator ==(const Sphere& rhs) const { return center_ == rhs.center_ && radius_ == rhs.radius_; }

		
		bool operator !=(const Sphere& rhs) const { return center_ != rhs.center_ || radius_ != rhs.radius_; }

		
		void Define(const Sphere& sphere)
		{
			Define(sphere.center_,sphere.radius_);
		}

		
		void Define(const Vector3& center,float radius)
		{
			center_ = center;
			radius_ = radius;
		}

		
		void Define(const Vector3* vertices,unsigned count);
		
		void Define(const BoundingBox& box);
		
		void Define(const Frustum& frustum);
		
		void Define(const Polyhedron& poly);

		
		void Merge(const Vector3& point)
		{
			if(radius_ < 0.0f)
			{
				center_ = point;
				radius_ = 0.0f;
				return;
			}

			Vector3 offset = point - center_;
			float dist = offset.Length();

			if(dist > radius_)
			{
				float half = (dist - radius_) * 0.5f;
				radius_ += half;
				center_ += (half / dist) * offset;
			}
		}

		
		void Merge(const Vector3* vertices,unsigned count);
		
		void Merge(const BoundingBox& box);
		
		void Merge(const Frustum& frustum);
		
		void Merge(const Polyhedron& poly);
		
		void Merge(const Sphere& sphere);

		
		void Clear()
		{
			center_ = Vector3::ZERO;
			radius_ = -M_INFINITY;
		}

		
		bool Defined() const
		{
			return radius_ >= 0.0f;
		}

		
		Intersection IsInside(const Vector3& point) const
		{
			float distSquared = (point - center_).LengthSquared();
			if(distSquared < radius_ * radius_)
				return INSIDE;
			else
				return OUTSIDE;
		}

		
		Intersection IsInside(const Sphere& sphere) const
		{
			float dist = (sphere.center_ - center_).Length();
			if(dist >= sphere.radius_ + radius_)
				return OUTSIDE;
			else if(dist + sphere.radius_ < radius_)
				return INSIDE;
			else
				return INTERSECTS;
		}

		
		Intersection IsInsideFast(const Sphere& sphere) const
		{
			float distSquared = (sphere.center_ - center_).LengthSquared();
			float combined = sphere.radius_ + radius_;

			if(distSquared >= combined * combined)
				return OUTSIDE;
			else
				return INSIDE;
		}

		
		Intersection IsInside(const BoundingBox& box) const;
		
		Intersection IsInsideFast(const BoundingBox& box) const;

		
		float Distance(const Vector3& point) const { return Max((point - center_).Length() - radius_,0.0f); }

		
		Vector3 center_;
		
		float radius_;
	};
}


//--------------------------------------------------------------------------------
#endif

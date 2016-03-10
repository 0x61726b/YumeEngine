



























#ifndef __YumePlane_h__
#define __YumePlane_h__
//--------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeMatrix3x4.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{

	
	class YumeAPIExport Plane
	{
	public:
		
		Plane():
			d_(0.0f)
		{
		}

		
		Plane(const Plane& plane):
			normal_(plane.normal_),
			absNormal_(plane.absNormal_),
			d_(plane.d_)
		{
		}

		
		Plane(const Vector3& v0,const Vector3& v1,const Vector3& v2)
		{
			Define(v0,v1,v2);
		}

		
		Plane(const Vector3& normal,const Vector3& point)
		{
			Define(normal,point);
		}

		
		Plane(const Vector4& plane)
		{
			Define(plane);
		}

		
		Plane& operator =(const Plane& rhs)
		{
			normal_ = rhs.normal_;
			absNormal_ = rhs.absNormal_;
			d_ = rhs.d_;
			return *this;
		}

		
		void Define(const Vector3& v0,const Vector3& v1,const Vector3& v2)
		{
			Vector3 dist1 = v1 - v0;
			Vector3 dist2 = v2 - v0;

			Define(dist1.CrossProduct(dist2),v0);
		}

		
		void Define(const Vector3& normal,const Vector3& point)
		{
			normal_ = normal.Normalized();
			absNormal_ = normal_.Abs();
			d_ = -normal_.DotProduct(point);
		}

		
		void Define(const Vector4& plane)
		{
			normal_ = Vector3(plane.x_,plane.y_,plane.z_);
			absNormal_ = normal_.Abs();
			d_ = plane.w_;
		}

		
		void Transform(const Matrix3& transform);
		
		void Transform(const Matrix3x4& transform);
		
		void Transform(const Matrix4& transform);

		
		Vector3 Project(const Vector3& point) const { return point - normal_ * (normal_.DotProduct(point) + d_); }

		
		float Distance(const Vector3& point) const { return normal_.DotProduct(point) + d_; }

		
		Vector3 Reflect(const Vector3& direction) const { return direction - (2.0f * normal_.DotProduct(direction) * normal_); }

		
		Matrix3x4 ReflectionMatrix() const;
		
		Plane Transformed(const Matrix3& transform) const;
		
		Plane Transformed(const Matrix3x4& transform) const;
		
		Plane Transformed(const Matrix4& transform) const;

		
		Vector4 ToVector4() const { return Vector4(normal_,d_); }

		
		Vector3 normal_;
		
		Vector3 absNormal_;
		
		float d_;

		
		static const Plane UP;
	};

}


//--------------------------------------------------------------------------------
#endif

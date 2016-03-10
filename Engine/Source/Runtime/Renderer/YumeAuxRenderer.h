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
#ifndef __YumeAuxRenderer_h__
#define __YumeAuxRenderer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeColor.h"
#include "Math/YumeFrustum.h"
#include "Scene/YumeSceneComponent.h"
#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeCamera;
	class YumeVertexBuffer;
	
	
	struct DebugLine
	{
		
		DebugLine()
		{
		}

		
		DebugLine(const Vector3& start,const Vector3& end,unsigned color):
			start_(start),
			end_(end),
			color_(color)
		{
		}

		
		Vector3 start_;
		
		Vector3 end_;
		
		unsigned color_;
	};

	
	struct DebugTriangle
	{
		
		DebugTriangle()
		{
		}

		
		DebugTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,unsigned color):
			v1_(v1),
			v2_(v2),
			v3_(v3),
			color_(color)
		{
		}

		
		Vector3 v1_;
		
		Vector3 v2_;
		
		Vector3 v3_;
		
		unsigned color_;
	};

	
	class YumeAPIExport YumeDebugRenderer : public YumeSceneComponent
	{
	public:
		YumeDebugRenderer();
		virtual ~YumeDebugRenderer();
		
		
		void SetView(YumeCamera* camera);
		
		void AddLine(const Vector3& start,const Vector3& end,const YumeColor& color,bool depthTest = true);
		
		void AddLine(const Vector3& start,const Vector3& end,unsigned color,bool depthTest = true);
		
		void AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,const YumeColor& color,bool depthTest = true);
		
		void AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,unsigned color,bool depthTest = true);
		
		void AddNode(YumeSceneNode* node,float scale = 1.0f,bool depthTest = true);
		
		void AddBoundingBox(const BoundingBox& box,const YumeColor& color,bool depthTest = true);
		
		void AddBoundingBox(const BoundingBox& box,const Matrix3x4& transform,const YumeColor& color,bool depthTest = true);
		
		void AddFrustum(const Frustum& frustum,const YumeColor& color,bool depthTest = true);
		
		void AddPolyhedron(const Polyhedron& poly,const YumeColor& color,bool depthTest = true);
		
		void AddSphere(const Sphere& sphere,const YumeColor& color,bool depthTest = true);
		
		void AddCylinder(const Vector3& position,float radius,float height,const YumeColor& color,bool depthTest = true);
		
		void AddTriangleMesh
			(const void* vertexData,unsigned vertexSize,const void* indexData,unsigned indexSize,unsigned indexStart,
			unsigned indexCount,const Matrix3x4& transform,const YumeColor& color,bool depthTest = true);
		
		void AddCircle(const Vector3& center,const Vector3& normal,float radius,const YumeColor& color,int steps = 64,bool depthTest = true);
		
		void AddCross(const Vector3& center,float size,const YumeColor& color,bool depthTest = true);
		
		void AddQuad(const Vector3& center,float width,float height,const YumeColor& color,bool depthTest = true);

		
		void Render();

		
		const Matrix3x4& GetView() const { return view_; }

		
		const Matrix4& GetProjection() const { return projection_; }

		
		const Frustum& GetFrustum() const { return frustum_; }

		
		bool IsInside(const BoundingBox& box) const;
		
		bool HasContent() const;

	private:
		
		void HandleEndFrame(YumeHash eventType,VariantMap& eventData);

		
		YumeVector<DebugLine>::type lines_;
		
		YumeVector<DebugLine>::type noDepthLines_;
		
		YumeVector<DebugTriangle>::type triangles_;
		
		YumeVector<DebugTriangle>::type noDepthTriangles_;
		
		Matrix3x4 view_;
		
		Matrix4 projection_;
		
		Frustum frustum_;
		
		SharedPtr<YumeVertexBuffer> vertexBuffer_;
	};
}


//----------------------------------------------------------------------------
#endif

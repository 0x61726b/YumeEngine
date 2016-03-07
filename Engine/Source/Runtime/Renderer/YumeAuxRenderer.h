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
	
	/// Debug rendering line.
	struct DebugLine
	{
		/// Construct undefined.
		DebugLine()
		{
		}

		/// Construct with start and end positions and color.
		DebugLine(const Vector3& start,const Vector3& end,unsigned color):
			start_(start),
			end_(end),
			color_(color)
		{
		}

		/// Start position.
		Vector3 start_;
		/// End position.
		Vector3 end_;
		/// Color.
		unsigned color_;
	};

	/// Debug render triangle.
	struct DebugTriangle
	{
		/// Construct undefined.
		DebugTriangle()
		{
		}

		/// Construct with start and end positions and color.
		DebugTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,unsigned color):
			v1_(v1),
			v2_(v2),
			v3_(v3),
			color_(color)
		{
		}

		/// Vertex a.
		Vector3 v1_;
		/// Vertex b.
		Vector3 v2_;
		/// Vertex c.
		Vector3 v3_;
		/// Color.
		unsigned color_;
	};

	/// Debug geometry rendering component. Should be added only to the root scene node.
	class YumeAPIExport YumeDebugRenderer : public YumeSceneComponent
	{
	public:
		YumeDebugRenderer();
		virtual ~YumeDebugRenderer();
		
		/// Set the camera viewpoint. Call before rendering, or before adding geometry if you want to use culling.
		void SetView(YumeCamera* camera);
		/// Add a line.
		void AddLine(const Vector3& start,const Vector3& end,const YumeColor& color,bool depthTest = true);
		/// Add a line with color already converted to unsigned.
		void AddLine(const Vector3& start,const Vector3& end,unsigned color,bool depthTest = true);
		/// Add a triangle.
		void AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,const YumeColor& color,bool depthTest = true);
		/// Add a triangle with color already converted to unsigned.
		void AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,unsigned color,bool depthTest = true);
		/// Add a scene node represented as its coordinate axes.
		void AddNode(YumeSceneNode* node,float scale = 1.0f,bool depthTest = true);
		/// Add a bounding box.
		void AddBoundingBox(const BoundingBox& box,const YumeColor& color,bool depthTest = true);
		/// Add a bounding box with transform.
		void AddBoundingBox(const BoundingBox& box,const Matrix3x4& transform,const YumeColor& color,bool depthTest = true);
		/// Add a frustum.
		void AddFrustum(const Frustum& frustum,const YumeColor& color,bool depthTest = true);
		/// Add a polyhedron.
		void AddPolyhedron(const Polyhedron& poly,const YumeColor& color,bool depthTest = true);
		/// Add a sphere.
		void AddSphere(const Sphere& sphere,const YumeColor& color,bool depthTest = true);
		/// Add a cylinder
		void AddCylinder(const Vector3& position,float radius,float height,const YumeColor& color,bool depthTest = true);
		/// Add a triangle mesh.
		void AddTriangleMesh
			(const void* vertexData,unsigned vertexSize,const void* indexData,unsigned indexSize,unsigned indexStart,
			unsigned indexCount,const Matrix3x4& transform,const YumeColor& color,bool depthTest = true);
		/// Add a circle.
		void AddCircle(const Vector3& center,const Vector3& normal,float radius,const YumeColor& color,int steps = 64,bool depthTest = true);
		/// Add a cross.
		void AddCross(const Vector3& center,float size,const YumeColor& color,bool depthTest = true);
		/// Add a quad on the XZ plane.
		void AddQuad(const Vector3& center,float width,float height,const YumeColor& color,bool depthTest = true);

		/// Update vertex buffer and render all debug lines. The viewport and rendertarget should be set before.
		void Render();

		/// Return the view transform.
		const Matrix3x4& GetView() const { return view_; }

		/// Return the projection transform.
		const Matrix4& GetProjection() const { return projection_; }

		/// Return the view frustum.
		const Frustum& GetFrustum() const { return frustum_; }

		/// Check whether a bounding box is inside the view frustum.
		bool IsInside(const BoundingBox& box) const;
		/// Return whether has something to render.
		bool HasContent() const;

	private:
		/// Handle end of frame. Clear debug geometry.
		void HandleEndFrame(YumeHash eventType,VariantMap& eventData);

		/// Lines rendered with depth test.
		YumeVector<DebugLine>::type lines_;
		/// Lines rendered without depth test.
		YumeVector<DebugLine>::type noDepthLines_;
		/// Triangles rendered with depth test.
		YumeVector<DebugTriangle>::type triangles_;
		/// Triangles rendered without depth test.
		YumeVector<DebugTriangle>::type noDepthTriangles_;
		/// View transform.
		Matrix3x4 view_;
		/// Projection transform.
		Matrix4 projection_;
		/// View frustum.
		Frustum frustum_;
		/// Vertex buffer.
		SharedPtr<YumeVertexBuffer> vertexBuffer_;
	};
}


//----------------------------------------------------------------------------
#endif

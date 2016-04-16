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
#include "YumeAuxRenderer.h"
#include "Renderer/YumeRHI.h"
#include "Renderer/YumeVertexBuffer.h"
#include "Math/YumePolyhedron.h"
#include "Renderer/YumeShaderVariation.h"
#include "Renderer/YumeCamera.h"
#include "Scene/YumeSceneNode.h"

#include "YumeTexture.h"

#include "YumeRenderer.h"
#include "YumeGeometry.h"
#include "YumeRHI.h"
#include "YumeRenderable.h"

#include "Core/YumeTimer.h"

#include "YumeRHI.h"
#include "Engine/YumeEngine.h"


namespace YumeEngine
{

	extern const char* SUBSYSTEM_CATEGORY;

	// Cap the amount of lines to prevent crash when eg. debug rendering large heightfields
	static const unsigned MAX_LINES = 1000000;
	// Cap the amount of triangles to prevent crash.
	static const unsigned MAX_TRIANGLES = 100000;

	YumeHash YumeDebugRenderer::type_ = "DebugRenderer";

	YumeDebugRenderer::YumeDebugRenderer()
	{
		vertexBuffer_ = SharedPtr<YumeVertexBuffer>(gYume->pRHI->CreateVertexBuffer());

		gYume->pTimer->AddTimeEventListener(this);
	}

	YumeDebugRenderer::~YumeDebugRenderer()
	{
		gYume->pTimer->RemoveTimeEventListener(this);
	}


	void YumeDebugRenderer::SetView(YumeCamera* camera)
	{
		if(!camera)
			return;

		view_ = camera->GetView();
		projection_ = camera->GetProjection();
		frustum_ = camera->GetFrustum();
	}

	void YumeDebugRenderer::AddLine(const Vector3& start,const Vector3& end,const YumeColor& color,bool depthTest)
	{
		AddLine(start,end,color.ToUInt(),depthTest);
	}

	void YumeDebugRenderer::AddLine(const Vector3& start,const Vector3& end,unsigned color,bool depthTest)
	{
		if(lines_.size() + noDepthLines_.size() >= MAX_LINES)
			return;

		if(depthTest)
			lines_.push_back(DebugLine(start,end,color));
		else
			noDepthLines_.push_back(DebugLine(start,end,color));
	}

	void YumeDebugRenderer::AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,const YumeColor& color,bool depthTest)
	{
		AddTriangle(v1,v2,v3,color.ToUInt(),depthTest);
	}

	void YumeDebugRenderer::AddTriangle(const Vector3& v1,const Vector3& v2,const Vector3& v3,unsigned color,bool depthTest)
	{
		if(triangles_.size() + noDepthTriangles_.size() >= MAX_TRIANGLES)
			return;

		if(depthTest)
			triangles_.push_back(DebugTriangle(v1,v2,v3,color));
		else
			noDepthTriangles_.push_back(DebugTriangle(v1,v2,v3,color));
	}

	void YumeDebugRenderer::AddNode(YumeSceneNode* node,float scale,bool depthTest)
	{
		if(!node)
			return;

		Vector3 start = node->GetWorldPosition();
		Quaternion rotation = node->GetWorldRotation();

		AddLine(start,start + rotation * (scale * Vector3::RIGHT),YumeColor::RED.ToUInt(),depthTest);
		AddLine(start,start + rotation * (scale * Vector3::UP),YumeColor::GREEN.ToUInt(),depthTest);
		AddLine(start,start + rotation * (scale * Vector3::FORWARD),YumeColor::BLUE.ToUInt(),depthTest);
	}

	void YumeDebugRenderer::AddBoundingBox(const BoundingBox& box,const YumeColor& color,bool depthTest)
	{
		const Vector3& min = box.min_;
		const Vector3& max = box.max_;

		Vector3 v1(max.x_,min.y_,min.z_);
		Vector3 v2(max.x_,max.y_,min.z_);
		Vector3 v3(min.x_,max.y_,min.z_);
		Vector3 v4(min.x_,min.y_,max.z_);
		Vector3 v5(max.x_,min.y_,max.z_);
		Vector3 v6(min.x_,max.y_,max.z_);

		unsigned uintColor = color.ToUInt();

		AddLine(min,v1,uintColor,depthTest);
		AddLine(v1,v2,uintColor,depthTest);
		AddLine(v2,v3,uintColor,depthTest);
		AddLine(v3,min,uintColor,depthTest);
		AddLine(v4,v5,uintColor,depthTest);
		AddLine(v5,max,uintColor,depthTest);
		AddLine(max,v6,uintColor,depthTest);
		AddLine(v6,v4,uintColor,depthTest);
		AddLine(min,v4,uintColor,depthTest);
		AddLine(v1,v5,uintColor,depthTest);
		AddLine(v2,max,uintColor,depthTest);
		AddLine(v3,v6,uintColor,depthTest);
	}

	void YumeDebugRenderer::AddBoundingBox(const BoundingBox& box,const Matrix3x4& transform,const YumeColor& color,bool depthTest)
	{
		const Vector3& min = box.min_;
		const Vector3& max = box.max_;

		Vector3 v0(transform * min);
		Vector3 v1(transform * Vector3(max.x_,min.y_,min.z_));
		Vector3 v2(transform * Vector3(max.x_,max.y_,min.z_));
		Vector3 v3(transform * Vector3(min.x_,max.y_,min.z_));
		Vector3 v4(transform * Vector3(min.x_,min.y_,max.z_));
		Vector3 v5(transform * Vector3(max.x_,min.y_,max.z_));
		Vector3 v6(transform * Vector3(min.x_,max.y_,max.z_));
		Vector3 v7(transform * max);

		unsigned uintColor = color.ToUInt();

		AddLine(v0,v1,uintColor,depthTest);
		AddLine(v1,v2,uintColor,depthTest);
		AddLine(v2,v3,uintColor,depthTest);
		AddLine(v3,v0,uintColor,depthTest);
		AddLine(v4,v5,uintColor,depthTest);
		AddLine(v5,v7,uintColor,depthTest);
		AddLine(v7,v6,uintColor,depthTest);
		AddLine(v6,v4,uintColor,depthTest);
		AddLine(v0,v4,uintColor,depthTest);
		AddLine(v1,v5,uintColor,depthTest);
		AddLine(v2,v7,uintColor,depthTest);
		AddLine(v3,v6,uintColor,depthTest);
	}


	void YumeDebugRenderer::AddFrustum(const Frustum& frustum,const YumeColor& color,bool depthTest)
	{
		const Vector3* vertices = frustum.vertices_;
		unsigned uintColor = color.ToUInt();

		AddLine(vertices[0],vertices[1],uintColor,depthTest);
		AddLine(vertices[1],vertices[2],uintColor,depthTest);
		AddLine(vertices[2],vertices[3],uintColor,depthTest);
		AddLine(vertices[3],vertices[0],uintColor,depthTest);
		AddLine(vertices[4],vertices[5],uintColor,depthTest);
		AddLine(vertices[5],vertices[6],uintColor,depthTest);
		AddLine(vertices[6],vertices[7],uintColor,depthTest);
		AddLine(vertices[7],vertices[4],uintColor,depthTest);
		AddLine(vertices[0],vertices[4],uintColor,depthTest);
		AddLine(vertices[1],vertices[5],uintColor,depthTest);
		AddLine(vertices[2],vertices[6],uintColor,depthTest);
		AddLine(vertices[3],vertices[7],uintColor,depthTest);
	}

	void YumeDebugRenderer::AddPolyhedron(const Polyhedron& poly,const YumeColor& color,bool depthTest)
	{
		unsigned uintColor = color.ToUInt();

		for(unsigned i = 0; i < poly.faces_.size(); ++i)
		{
			const YumeVector<Vector3>::type& face = poly.faces_[i];
			if(face.size() >= 3)
			{
				for(unsigned j = 0; j < face.size(); ++j)
					AddLine(face[j],face[(j + 1) % face.size()],uintColor,depthTest);
			}
		}
	}

	static Vector3 PointOnSphere(const Sphere& sphere,unsigned theta,unsigned phi)
	{
		return Vector3(
			sphere.center_.x_ + sphere.radius_ * Sin((float)theta) * Sin((float)phi),
			sphere.center_.y_ + sphere.radius_ * Cos((float)phi),
			sphere.center_.z_ + sphere.radius_ * Cos((float)theta) * Sin((float)phi)
			);
	}

	void YumeDebugRenderer::AddSphere(const Sphere& sphere,const YumeColor& color,bool depthTest)
	{
		unsigned uintColor = color.ToUInt();

		for(unsigned j = 0; j < 180; j += 45)
		{
			for(unsigned i = 0; i < 360; i += 45)
			{
				Vector3 p1 = PointOnSphere(sphere,i,j);
				Vector3 p2 = PointOnSphere(sphere,i + 45,j);
				Vector3 p3 = PointOnSphere(sphere,i,j + 45);
				Vector3 p4 = PointOnSphere(sphere,i + 45,j + 45);

				AddLine(p1,p2,uintColor,depthTest);
				AddLine(p3,p4,uintColor,depthTest);
				AddLine(p1,p3,uintColor,depthTest);
				AddLine(p2,p4,uintColor,depthTest);
			}
		}
	}

	void YumeDebugRenderer::AddCylinder(const Vector3& position,float radius,float height,const YumeColor& color,bool depthTest)
	{
		Sphere sphere(position,radius);
		Vector3 heightVec(0,height,0);
		Vector3 offsetXVec(radius,0,0);
		Vector3 offsetZVec(0,0,radius);
		for(unsigned i = 0; i < 360; i += 45)
		{
			Vector3 p1 = PointOnSphere(sphere,i,90);
			Vector3 p2 = PointOnSphere(sphere,i + 45,90);
			AddLine(p1,p2,color,depthTest);
			AddLine(p1 + heightVec,p2 + heightVec,color,depthTest);
		}
		AddLine(position + offsetXVec,position + heightVec + offsetXVec,color,depthTest);
		AddLine(position - offsetXVec,position + heightVec - offsetXVec,color,depthTest);
		AddLine(position + offsetZVec,position + heightVec + offsetZVec,color,depthTest);
		AddLine(position - offsetZVec,position + heightVec - offsetZVec,color,depthTest);
	}

	void YumeDebugRenderer::AddTriangleMesh(const void* vertexData,unsigned vertexSize,const void* indexData,unsigned indexSize,
		unsigned indexStart,unsigned indexCount,const Matrix3x4& transform,const YumeColor& color,bool depthTest)
	{
		unsigned uintColor = color.ToUInt();
		const unsigned char* srcData = (const unsigned char*)vertexData;

		// 16-bit indices
		if(indexSize == sizeof(unsigned short))
		{
			const unsigned short* indices = ((const unsigned short*)indexData) + indexStart;
			const unsigned short* indicesEnd = indices + indexCount;

			while(indices < indicesEnd)
			{
				Vector3 v0 = transform * *((const Vector3*)(&srcData[indices[0] * vertexSize]));
				Vector3 v1 = transform * *((const Vector3*)(&srcData[indices[1] * vertexSize]));
				Vector3 v2 = transform * *((const Vector3*)(&srcData[indices[2] * vertexSize]));

				AddLine(v0,v1,uintColor,depthTest);
				AddLine(v1,v2,uintColor,depthTest);
				AddLine(v2,v0,uintColor,depthTest);

				indices += 3;
			}
		}
		else
		{
			const unsigned* indices = ((const unsigned*)indexData) + indexStart;
			const unsigned* indicesEnd = indices + indexCount;

			while(indices < indicesEnd)
			{
				Vector3 v0 = transform * *((const Vector3*)(&srcData[indices[0] * vertexSize]));
				Vector3 v1 = transform * *((const Vector3*)(&srcData[indices[1] * vertexSize]));
				Vector3 v2 = transform * *((const Vector3*)(&srcData[indices[2] * vertexSize]));

				AddLine(v0,v1,uintColor,depthTest);
				AddLine(v1,v2,uintColor,depthTest);
				AddLine(v2,v0,uintColor,depthTest);

				indices += 3;
			}
		}
	}

	void YumeDebugRenderer::AddCircle(const Vector3& center,const Vector3& normal,float radius,const YumeColor& color,int steps,bool depthTest)
	{
		Quaternion orientation;
		orientation.FromRotationTo(Vector3::UP,normal.Normalized());
		Vector3 p = orientation * Vector3(radius,0,0) + center;
		unsigned uintColor = color.ToUInt();

		for(int i = 1; i <= steps; ++i)
		{
			const float angle = (float)i / (float)steps * 360.0f;
			Vector3 v(radius * Cos(angle),0,radius * Sin(angle));
			Vector3 c = orientation * v + center;
			AddLine(p,c,uintColor,depthTest);
			p = c;
		}

		p = center + normal * (radius / 4.0f);
		AddLine(center,p,uintColor,depthTest);
	}

	void YumeDebugRenderer::AddCross(const Vector3& center,float size,const YumeColor& color,bool depthTest)
	{
		unsigned uintColor = color.ToUInt();

		float halfSize = size / 2.0f;
		for(int i = 0; i < 3; ++i)
		{
			float start[3] ={center.x_,center.y_,center.z_};
			float end[3] ={center.x_,center.y_,center.z_};
			start[i] -= halfSize;
			end[i] += halfSize;
			AddLine(Vector3(start),Vector3(end),uintColor,depthTest);
		}
	}

	void YumeDebugRenderer::AddQuad(const Vector3& center,float width,float height,const YumeColor& color,bool depthTest)
	{
		unsigned uintColor = color.ToUInt();

		Vector3 v0(center.x_ - width / 2,center.y_,center.z_ - height / 2);
		Vector3 v1(center.x_ + width / 2,center.y_,center.z_ - height / 2);
		Vector3 v2(center.x_ + width / 2,center.y_,center.z_ + height / 2);
		Vector3 v3(center.x_ - width / 2,center.y_,center.z_ + height / 2);
		AddLine(v0,v1,uintColor,depthTest);
		AddLine(v1,v2,uintColor,depthTest);
		AddLine(v2,v3,uintColor,depthTest);
		AddLine(v3,v0,uintColor,depthTest);
	}

	void YumeDebugRenderer::RenderInternalTexture(const IntRect& screenPos,YumeTexture* texture)
	{

		if(!texture)
			return;

		gYume->pRHI->SetViewport(screenPos);
		/*gYume->pRHI->Clear(CLEAR_COLOR | CLEAR_DEPTH |CLEAR_STENCIL);*/



		YumeShaderVariation* diffTextureVS = gYume->pRHI->GetShader(VS,"LPV/fs_triangle");
		YumeShaderVariation* diffTexturePS = gYume->pRHI->GetShader(PS,"BasicOverlay");

		gYume->pRHI->SetBlendMode(BLEND_PREMULALPHA);
		gYume->pRHI->SetDepthTest(CMP_ALWAYS);
		gYume->pRHI->SetDepthWrite(false);
		gYume->pRHI->SetFillMode(FILL_SOLID);
		gYume->pRHI->SetClipPlane(false);
		gYume->pRHI->SetScissorTest(false);
		gYume->pRHI->SetStencilTest(false);
		gYume->pRHI->BindSampler(PS,2,1,2);
		gYume->pRHI->BindDefaultDepthStencil();
		gYume->pRHI->SetRenderTarget(0,(YumeRenderable*)0);
		gYume->pRHI->SetTexture(10,texture);
		gYume->pRHI->SetShaders(diffTextureVS,diffTexturePS);
		



		YumeGeometry* geometry = gYume->pRenderer->GetFsTriangle();

		geometry->Draw(gYume->pRHI);
	}

	void YumeDebugRenderer::Render()
	{
		if(!HasContent())
			return;

		YumeRHI* graphics = gYume->pRHI;
		// Engine does not render when window is closed or device is lost
		assert(graphics && graphics->IsInitialized() && !graphics->IsDeviceLost());


		YumeShaderVariation* vs = graphics->GetShader(VS,"Basic","VERTEXCOLOR");
		YumeShaderVariation* ps = graphics->GetShader(PS,"Basic","VERTEXCOLOR");

		unsigned numVertices = (lines_.size() + noDepthLines_.size()) * 2 + (triangles_.size() + noDepthTriangles_.size()) * 3;
		// Resize the vertex buffer if too small or much too large
		if(vertexBuffer_->GetVertexCount() < numVertices || vertexBuffer_->GetVertexCount() > numVertices * 2)
			vertexBuffer_->SetSize(numVertices,MASK_POSITION | MASK_COLOR,true);

		float* dest = (float*)vertexBuffer_->Lock(0,numVertices,true);
		if(!dest)
			return;

		for(unsigned i = 0; i < lines_.size(); ++i)
		{
			const DebugLine& line = lines_[i];

			dest[0] = line.start_.x_;
			dest[1] = line.start_.y_;
			dest[2] = line.start_.z_;
			((unsigned&)dest[3]) = line.color_;
			dest[4] = line.end_.x_;
			dest[5] = line.end_.y_;
			dest[6] = line.end_.z_;
			((unsigned&)dest[7]) = line.color_;

			dest += 8;
		}

		for(unsigned i = 0; i < noDepthLines_.size(); ++i)
		{
			const DebugLine& line = noDepthLines_[i];

			dest[0] = line.start_.x_;
			dest[1] = line.start_.y_;
			dest[2] = line.start_.z_;
			((unsigned&)dest[3]) = line.color_;
			dest[4] = line.end_.x_;
			dest[5] = line.end_.y_;
			dest[6] = line.end_.z_;
			((unsigned&)dest[7]) = line.color_;

			dest += 8;
		}

		for(unsigned i = 0; i < triangles_.size(); ++i)
		{
			const DebugTriangle& triangle = triangles_[i];

			dest[0] = triangle.v1_.x_;
			dest[1] = triangle.v1_.y_;
			dest[2] = triangle.v1_.z_;
			((unsigned&)dest[3]) = triangle.color_;

			dest[4] = triangle.v2_.x_;
			dest[5] = triangle.v2_.y_;
			dest[6] = triangle.v2_.z_;
			((unsigned&)dest[7]) = triangle.color_;

			dest[8] = triangle.v3_.x_;
			dest[9] = triangle.v3_.y_;
			dest[10] = triangle.v3_.z_;
			((unsigned&)dest[11]) = triangle.color_;

			dest += 12;
		}

		for(unsigned i = 0; i < noDepthTriangles_.size(); ++i)
		{
			const DebugTriangle& triangle = noDepthTriangles_[i];

			dest[0] = triangle.v1_.x_;
			dest[1] = triangle.v1_.y_;
			dest[2] = triangle.v1_.z_;
			((unsigned&)dest[3]) = triangle.color_;

			dest[4] = triangle.v2_.x_;
			dest[5] = triangle.v2_.y_;
			dest[6] = triangle.v2_.z_;
			((unsigned&)dest[7]) = triangle.color_;

			dest[8] = triangle.v3_.x_;
			dest[9] = triangle.v3_.y_;
			dest[10] = triangle.v3_.z_;
			((unsigned&)dest[11]) = triangle.color_;

			dest += 12;
		}

		vertexBuffer_->Unlock();

		graphics->SetBlendMode(BLEND_REPLACE);
		graphics->SetColorWrite(true);
		graphics->SetCullMode(CULL_NONE);
		graphics->SetDepthWrite(true);
		graphics->SetScissorTest(false);
		graphics->SetStencilTest(false);
		graphics->SetShaders(vs,ps);
		graphics->SetShaderParameter(VSP_MODEL,Matrix3x4::IDENTITY);
		graphics->SetShaderParameter(VSP_VIEW,view_);
		graphics->SetShaderParameter(VSP_VIEWINV,view_.Inverse());
		graphics->SetShaderParameter(VSP_VIEWPROJ,projection_ * view_);
		graphics->SetShaderParameter(PSP_MATDIFFCOLOR,YumeColor(1.0f,1.0f,1.0f,1.0f));
		graphics->SetVertexBuffer(vertexBuffer_);

		unsigned start = 0;
		unsigned count = 0;
		if(lines_.size())
		{
			count = lines_.size() * 2;
			graphics->SetDepthTest(CMP_LESSEQUAL);
			graphics->Draw(LINE_LIST,start,count);
			start += count;
		}
		if(noDepthLines_.size())
		{
			count = noDepthLines_.size() * 2;
			graphics->SetDepthTest(CMP_ALWAYS);
			graphics->Draw(LINE_LIST,start,count);
			start += count;
		}

		graphics->SetBlendMode(BLEND_ALPHA);

		if(triangles_.size())
		{
			count = triangles_.size() * 3;
			graphics->SetDepthTest(CMP_LESSEQUAL);
			graphics->Draw(TRIANGLE_LIST,start,count);
			start += count;
		}
		if(noDepthTriangles_.size())
		{
			count = noDepthTriangles_.size() * 3;
			graphics->SetDepthTest(CMP_ALWAYS);
			graphics->Draw(TRIANGLE_LIST,start,count);
		}
	}

	bool YumeDebugRenderer::IsInside(const BoundingBox& box) const
	{
		return frustum_.IsInsideFast(box) == INSIDE;
	}

	bool YumeDebugRenderer::HasContent() const
	{
		return !(lines_.empty() && noDepthLines_.empty() && triangles_.empty() && noDepthTriangles_.empty());
	}

	void YumeDebugRenderer::HandleEndFrame(int frameNumber)
	{
		// When the amount of debug geometry is reduced, release memory
		unsigned linesSize = lines_.size();
		unsigned noDepthLinesSize = noDepthLines_.size();
		unsigned trianglesSize = triangles_.size();
		unsigned noDepthTrianglesSize = noDepthTriangles_.size();

		lines_.clear();
		noDepthLines_.clear();
		triangles_.clear();
		noDepthTriangles_.clear();

		if(lines_.capacity() > linesSize * 2)
			lines_.reserve(linesSize);
		if(noDepthLines_.capacity() > noDepthLinesSize * 2)
			noDepthLines_.reserve(noDepthLinesSize);
		if(triangles_.capacity() > trianglesSize * 2)
			triangles_.reserve(trianglesSize);
		if(noDepthTriangles_.capacity() > noDepthTrianglesSize * 2)
			noDepthTriangles_.reserve(noDepthTrianglesSize);
	}

}

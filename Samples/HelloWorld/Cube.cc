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
#include "Cube.h"
#include "Renderer/YumeVertexBuffer.h"
#include "Renderer/YumeIndexBuffer.h"
#include "Renderer/YumeRHI.h"
#include "Renderer/YumeGeometry.h"
#include "Engine/YumeEngine.h"
#include "Core/YumeBase.h"
#include "Renderer/YumeMaterial.h"

#include "Scene/YumeSceneNode.h"
namespace YumeEngine
{
	static const float dummyVertices[] =
	{
		1,1,-1,0,
		-1,1,-1,0,
		-1,1,1,0,
		1,1,1,0,
		1,-1,-1,0,
		-1,-1,-1,0,
		-1,-1,1,0,
		1,-1,1,0
	};

	static const unsigned short dummyIndices[] =
	{
		0,1,2,
		0,2,3,
		0,4,5,
		0,5,1,
		1,5,6,
		1,6,2,
		2,6,7,
		2,7,3,
		3,7,4,
		3,4,0,
		4,7,6,
		4,6,5
	};

	YumeHash Cube::type_ = "Cube";

	Cube::Cube()
		: YumeDrawable(DRAWABLE_GEOMETRY)
	{
		YumeRHI* graphics_ = YumeEngine3D::Get()->GetRenderer();

		YumeVertexBuffer* dlvb = (graphics_->CreateVertexBuffer());
		dlvb->SetShadowed(true);
		dlvb->SetSize(8,MASK_POSITION | MASK_COLOR);
		dlvb->SetData(dummyVertices);

		YumeIndexBuffer* dlib = (graphics_->CreateIndexBuffer());
		dlib->SetShadowed(true);
		dlib->SetSize(36,false);
		dlib->SetData(dummyIndices);

		geo_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		geo_->SetVertexBuffer(0,dlvb);
		geo_->SetIndexBuffer(dlib);
		geo_->SetDrawRange(TRIANGLE_LIST,0,dlib->GetIndexCount());

		const Matrix3x4 worldTransform = Matrix3x4::IDENTITY;
		batches_.resize(1);
		batches_[0].worldTransform_ = &worldTransform;





	}
	Cube::~Cube()
	{

	}

	void Cube::SetMaterial(SharedPtr<YumeMaterial> material)
	{
		for(unsigned i = 0; i < batches_.size(); ++i)
			batches_[i].material_ = material;
	}


	void Cube::SetBoundingBox()
	{
		boundingBox_ = BoundingBox(Vector3(-0.7f,-0.9f,0),Vector3(0.7f,0.9f,0));
		OnMarkedDirty(node_);

		for(int i=0; i < batches_.size(); ++i)
			batches_[i].geometry_ = geo_.get();
	}

	void Cube::OnWorldBoundingBoxUpdate()
	{
		worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
	}

	void Cube::UpdateBatches(const FrameInfo& frame)
	{

	}
}

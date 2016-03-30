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
#include "YumeStaticModel.h"
#include "YumeModel.h"
#include "YumeBatch.h"
#include "YumeCamera.h"
#include "YumeGeometry.h"
#include "YumeMaterial.h"
#include "YumeOcclusion.h"
#include "Scene/YumeOctreeQuery.h"
#include "Logging/logging.h"
#include "YumeResourceManager.h"

#include "Scene/YumeSceneNode.h"

#include "Core/YumeFile.h"



namespace YumeEngine
{

	extern const char* GEOMETRY_CATEGORY;

	YumeHash YumeStaticModel::type_ = "StaticModel";
	YumeStaticModel::YumeStaticModel():
		YumeDrawable(DRAWABLE_GEOMETRY),
		occlusionLodLevel_(M_MAX_UNSIGNED)
	{
	}

	YumeStaticModel::~YumeStaticModel()
	{
	}


	void YumeStaticModel::ProcessRayQuery(const RayOctreeQuery& query,YumePodVector<RayQueryResult>::type& results)
	{
		RayQueryLevel level = query.level_;

		switch(level)
		{
		case RAY_AABB:
			YumeDrawable::ProcessRayQuery(query,results);
			break;

		case RAY_OBB:
		case RAY_TRIANGLE:
		case RAY_TRIANGLE_UV:
			Matrix3x4 inverse(node_->GetWorldTransform().Inverse());
			Ray localRay = query.ray_.Transformed(inverse);
			float distance = localRay.HitDistance(boundingBox_);
			Vector3 normal = -query.ray_.direction_;
			Vector2 geometryUV;
			unsigned hitBatch = M_MAX_UNSIGNED;

			if(level >= RAY_TRIANGLE && distance < query.maxDistance_)
			{
				distance = M_INFINITY;

				for(unsigned i = 0; i < batches_.size(); ++i)
				{
					YumeGeometry* geometry = batches_[i].geometry_;
					if(geometry)
					{
						Vector3 geometryNormal;
						float geometryDistance = level == RAY_TRIANGLE ? geometry->GetHitDistance(localRay,&geometryNormal) :
							geometry->GetHitDistance(localRay,&geometryNormal,&geometryUV);
						if(geometryDistance < query.maxDistance_ && geometryDistance < distance)
						{
							distance = geometryDistance;
							normal = (node_->GetWorldTransform() * Vector4(geometryNormal,0.0f)).Normalized();
							hitBatch = i;
						}
					}
				}
			}

			if(distance < query.maxDistance_)
			{
				RayQueryResult result;
				result.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
				result.normal_ = normal;
				result.textureUV_ = geometryUV;
				result.distance_ = distance;
				result.drawable_ = this;
				result.node_ = node_;
				result.subObject_ = hitBatch;
				results.push_back(result);
			}
			break;
		}
	}

	void YumeStaticModel::UpdateBatches(const FrameInfo& frame)
	{
		const BoundingBox& worldBoundingBox = GetWorldBoundingBox();
		distance_ = frame.camera_->GetDistance(worldBoundingBox.Center());

		if(batches_.size() == 1)
			batches_[0].distance_ = distance_;
		else
		{
			const Matrix3x4& worldTransform = node_->GetWorldTransform();
			for(unsigned i = 0; i < batches_.size(); ++i)
				batches_[i].distance_ = frame.camera_->GetDistance(worldTransform * geometryData_[i].center_);
		}

		float scale = worldBoundingBox.Size().DotProduct(DOT_SCALE);
		float newLodDistance = frame.camera_->GetLodDistance(distance_,scale,lodBias_);

		if(newLodDistance != lodDistance_)
		{
			lodDistance_ = newLodDistance;
			CalculateLodLevels();
		}
	}

	YumeGeometry* YumeStaticModel::GetLodGeometry(unsigned batchIndex,unsigned level)
	{
		if(batchIndex >= geometries_.size())
			return 0;

		// If level is out of range, use visible geometry
		if(level < geometries_[batchIndex].size())
			return geometries_[batchIndex][level];
		else
			return batches_[batchIndex].geometry_;
	}

	unsigned YumeStaticModel::GetNumOccluderTriangles()
	{
		unsigned triangles = 0;

		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			YumeGeometry* geometry = GetLodGeometry(i,occlusionLodLevel_);
			if(!geometry)
				continue;

			// Check that the material is suitable for occlusion (default material always is)
			YumeMaterial* mat = batches_[i].material_;
			if(mat && !mat->GetOcclusion())
				continue;

			triangles += geometry->GetIndexCount() / 3;
		}

		return triangles;
	}

	bool YumeStaticModel::DrawOcclusion(OcclusionBuffer* buffer)
	{
		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			YumeGeometry* geometry = GetLodGeometry(i,occlusionLodLevel_);
			if(!geometry)
				continue;

			// Check that the material is suitable for occlusion (default material always is) and set culling mode
			YumeMaterial* material = batches_[i].material_;
			if(material)
			{
				if(!material->GetOcclusion())
					continue;
				buffer->SetCullMode(material->GetCullMode());
			}
			else
				buffer->SetCullMode(CULL_CCW);

			const unsigned char* vertexData;
			unsigned vertexSize;
			const unsigned char* indexData;
			unsigned indexSize;
			unsigned elementMask;

			geometry->GetRawData(vertexData,vertexSize,indexData,indexSize,elementMask);
			// Check for valid geometry data
			if(!vertexData || !indexData)
				continue;

			unsigned indexStart = geometry->GetIndexStart();
			unsigned indexCount = geometry->GetIndexCount();

			// Draw and check for running out of triangles
			if(!buffer->AddTriangles(node_->GetWorldTransform(),vertexData,vertexSize,indexData,indexSize,indexStart,indexCount))
				return false;
		}

		return true;
	}

	void YumeStaticModel::SetModel(YumeModel* model)
	{
		if(model == model_)
			return;

		model_ = (model);

		if(model)
		{
			// Copy the subgeometry & LOD level structure
			SetNumGeometries(model->GetNumGeometries());
			const YumeVector<YumeVector<SharedPtr<YumeGeometry> >::type >::type& geometries = model->GetGeometries();
			const YumeVector<Vector3>::type& geometryCenters = model->GetGeometryCenters();
			const Matrix3x4* worldTransform = node_ ? &node_->GetWorldTransform() : (const Matrix3x4*)0;
			for(unsigned i = 0; i < geometries.size(); ++i)
			{
				batches_[i].worldTransform_ = worldTransform;
				geometries_[i] = geometries[i];
				geometryData_[i].center_ = geometryCenters[i];
			}

			SetBoundingBox(model->GetBoundingBox());
			ResetLodLevels();
		}
		else
		{
			SetNumGeometries(0);
			SetBoundingBox(BoundingBox());
		}


	}

	void YumeStaticModel::SetMaterial(YumeMaterial* material)
	{
		for(unsigned i = 0; i < batches_.size(); ++i)
			batches_[i].material_ = (material);


	}

	bool YumeStaticModel::SetMaterial(unsigned index,YumeMaterial* material)
	{
		if(index >= batches_.size())
		{
			YUMELOG_ERROR("Material index out of bounds");
			return false;
		}

		batches_[index].material_ = (material);

		return true;
	}



	void YumeStaticModel::SetOcclusionLodLevel(unsigned level)
	{
		occlusionLodLevel_ = level;

	}

	void YumeStaticModel::ApplyMaterialList(const YumeString& fileName)
	{
		String useFileName = fileName;
		if(useFileName.Trimmed().empty() && model_)
		{
			String path,file,extension;
			SplitPath(model_->GetName(),path,file,extension);
			useFileName = path + file + ".txt";
		}


		SharedPtr<YumeFile> file = gYume->pResourceManager->GetFile(useFileName);
		if(!file)
			return;

		unsigned index = 0;
		while(!file->Eof() && index < batches_.size())
		{
			YumeMaterial* material = gYume->pResourceManager->PrepareResource<YumeMaterial>(file->ReadLine());
			if(material)
				SetMaterial(index,material);

			++index;
		}
	}

	YumeMaterial* YumeStaticModel::GetMaterial(unsigned index) const
	{
		return index < batches_.size() ? batches_[index].material_ : (YumeMaterial*)0;
	}

	bool YumeStaticModel::IsInside(const Vector3& point) const
	{
		if(!node_)
			return false;

		Vector3 localPosition = node_->GetWorldTransform().Inverse() * point;
		return IsInsideLocal(localPosition);
	}

	bool YumeStaticModel::IsInsideLocal(const Vector3& point) const
	{
		// Early-out if point is not inside bounding box
		if(boundingBox_.IsInside(point) == OUTSIDE)
			return false;

		Ray localRay(point,Vector3(1.0f,-1.0f,1.0f));

		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			YumeGeometry* geometry = batches_[i].geometry_;
			if(geometry)
			{
				if(geometry->IsInside(localRay))
					return true;
			}
		}

		return false;
	}

	void YumeStaticModel::SetBoundingBox(const BoundingBox& box)
	{
		boundingBox_ = box;
		OnMarkedDirty(node_);
	}

	void YumeStaticModel::SetNumGeometries(unsigned num)
	{
		batches_.resize(num);
		geometries_.resize(num);
		geometryData_.resize(num);
		ResetLodLevels();
	}

	void YumeStaticModel::OnWorldBoundingBoxUpdate()
	{
		Matrix3x4 transform = node_->GetWorldTransform();
		worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
	}

	void YumeStaticModel::ResetLodLevels()
	{
		// Ensure that each subgeometry has at least one LOD level, and reset the current LOD level
		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			if(!geometries_[i].size())
				geometries_[i].resize(1);
			batches_[i].geometry_ = geometries_[i][0];
			geometryData_[i].lodLevel_ = 0;
		}

		// Find out the real LOD levels on next geometry update
		lodDistance_ = M_INFINITY;
	}

	void YumeStaticModel::CalculateLodLevels()
	{
		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			const YumeVector<SharedPtr<YumeGeometry> >::type& batchGeometries = geometries_[i];
			// If only one LOD geometry, no reason to go through the LOD calculation
			if(batchGeometries.size() <= 1)
				continue;

			unsigned j;

			for(j = 1; j < batchGeometries.size(); ++j)
			{
				if(batchGeometries[j] && lodDistance_ <= batchGeometries[j]->GetLodDistance())
					break;
			}

			unsigned newLodLevel = j - 1;
			if(geometryData_[i].lodLevel_ != newLodLevel)
			{
				geometryData_[i].lodLevel_ = newLodLevel;
				batches_[i].geometry_ = batchGeometries[newLodLevel];
			}
		}
	}
}

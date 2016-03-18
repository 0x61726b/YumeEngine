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
#include "YumeModel.h"
#include "YumeGeometry.h"
#include "YumeIndexBuffer.h"
#include "YumeVertexBuffer.h"
#include "YumeRHI.h"
#include "Core/YumeFile.h"
#include "Engine/YumeEngine.h"
#include "Logging/logging.h"



namespace YumeEngine
{

	unsigned LookupVertexBuffer(YumeVertexBuffer* buffer,const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers)
	{
		for(unsigned i = 0; i < buffers.size(); ++i)
		{
			if(buffers[i] == buffer)
				return i;
		}
		return 0;
	}

	unsigned LookupIndexBuffer(YumeIndexBuffer* buffer,const YumeVector<SharedPtr<YumeIndexBuffer> >::type& buffers)
	{
		for(unsigned i = 0; i < buffers.size(); ++i)
		{
			if(buffers[i] == buffer)
				return i;
		}
		return 0;
	}

	YumeHash YumeModel::type_ = "Model";

	YumeModel::YumeModel()
	{
	}

	YumeModel::~YumeModel()
	{
	}


	bool YumeModel::BeginLoad(YumeFile& source)
	{
		// Check ID
		if(source.GetFileExtension() != "UMDL")
		{
			YUMELOG_ERROR(source.GetName() + " is not a valid model file");
			return false;
		}

		geometries_.clear();
		geometryBoneMappings_.clear();
		geometryCenters_.clear();
		morphs_.clear();
		vertexBuffers_.clear();
		indexBuffers_.clear();

		YumeRHI* rhi_ = gYume->pRHI;

		unsigned memoryUse = sizeof(YumeModel);
		bool async = GetAsyncLoadState() == ASYNC_LOADING;

		// Read vertex buffers
		unsigned numVertexBuffers = source.ReadUInt();
		vertexBuffers_.reserve(numVertexBuffers);
		morphRangeStarts_.resize(numVertexBuffers);
		morphRangeCounts_.resize(numVertexBuffers);
		loadVBData_.resize(numVertexBuffers);
		for(unsigned i = 0; i < numVertexBuffers; ++i)
		{
			unsigned vertexCount = source.ReadUInt();
			unsigned elementMask = source.ReadUInt();
			morphRangeStarts_[i] = source.ReadUInt();
			morphRangeCounts_[i] = source.ReadUInt();

			SharedPtr<YumeVertexBuffer> buffer = SharedPtr<YumeVertexBuffer>(rhi_->CreateVertexBuffer());
			unsigned vertexSize = YumeVertexBuffer::GetVertexSize(elementMask);

			// Prepare vertex buffer data to be uploaded during EndLoad()
			if(async)
			{
				loadVBData_[i].vertexCount_ = vertexCount;
				loadVBData_[i].elementMask_ = elementMask;
				loadVBData_[i].dataSize_ = vertexCount * vertexSize;
				loadVBData_[i].data_ = boost::shared_array<unsigned char>(new unsigned char[loadVBData_[i].dataSize_]);
				source.Read(loadVBData_[i].data_.get(),loadVBData_[i].dataSize_);
			}
			else
			{
				// If not async loading, use locking to avoid extra allocation & copy
				loadVBData_[i].data_.reset(); // Make sure no previous data
				buffer->SetShadowed(true);
				buffer->SetSize(vertexCount,elementMask);
				void* dest = buffer->Lock(0,vertexCount);
				source.Read(dest,vertexCount * vertexSize);
				buffer->Unlock();
			}

			memoryUse += sizeof(YumeVertexBuffer) + vertexCount * vertexSize;
			vertexBuffers_.push_back(buffer);
		}

		// Read index buffers
		unsigned numIndexBuffers = source.ReadUInt();
		indexBuffers_.reserve(numIndexBuffers);
		loadIBData_.resize(numIndexBuffers);
		for(unsigned i = 0; i < numIndexBuffers; ++i)
		{
			unsigned indexCount = source.ReadUInt();
			unsigned indexSize = source.ReadUInt();

			SharedPtr<YumeIndexBuffer> buffer =  SharedPtr<YumeIndexBuffer>(rhi_->CreateIndexBuffer());

			// Prepare index buffer data to be uploaded during EndLoad()
			if(async)
			{
				loadIBData_[i].indexCount_ = indexCount;
				loadIBData_[i].indexSize_ = indexSize;
				loadIBData_[i].dataSize_ = indexCount * indexSize;
				loadIBData_[i].data_ = boost::shared_array<unsigned char>(new unsigned char[loadIBData_[i].dataSize_]);
				source.Read(loadIBData_[i].data_.get(),loadIBData_[i].dataSize_);
			}
			else
			{
				// If not async loading, use locking to avoid extra allocation & copy
				loadIBData_[i].data_.reset(); // Make sure no previous data
				buffer->SetShadowed(true);
				buffer->SetSize(indexCount,indexSize > sizeof(unsigned short));
				void* dest = buffer->Lock(0,indexCount);
				source.Read(dest,indexCount * indexSize);
				buffer->Unlock();
			}

			memoryUse += sizeof(YumeIndexBuffer) + indexCount * indexSize;
			indexBuffers_.push_back(buffer);
		}

		// Read geometries
		unsigned numGeometries = source.ReadUInt();
		geometries_.reserve(numGeometries);
		geometryBoneMappings_.reserve(numGeometries);
		geometryCenters_.reserve(numGeometries);
		loadGeometries_.resize(numGeometries);
		for(unsigned i = 0; i < numGeometries; ++i)
		{
			// Read bone mappings
			unsigned boneMappingCount = source.ReadUInt();
			YumeVector<unsigned>::type boneMapping(boneMappingCount);
			for(unsigned j = 0; j < boneMappingCount; ++j)
				boneMapping[j] = source.ReadUInt();
			geometryBoneMappings_.push_back(boneMapping);

			unsigned numLodLevels = source.ReadUInt();
			YumeVector<SharedPtr<YumeGeometry> >::type geometryLodLevels;
			geometryLodLevels.reserve(numLodLevels);
			loadGeometries_[i].resize(numLodLevels);

			for(unsigned j = 0; j < numLodLevels; ++j)
			{
				float distance = source.ReadFloat();
				PrimitiveType type = (PrimitiveType)source.ReadUInt();

				unsigned vbRef = source.ReadUInt();
				unsigned ibRef = source.ReadUInt();
				unsigned indexStart = source.ReadUInt();
				unsigned indexCount = source.ReadUInt();

				if(vbRef >= vertexBuffers_.size())
				{
					YUMELOG_ERROR("Vertex buffer index out of bounds");
					loadVBData_.clear();
					loadIBData_.clear();
					loadGeometries_.clear();
					return false;
				}
				if(ibRef >= indexBuffers_.size())
				{
					YUMELOG_ERROR("Index buffer index out of bounds");
					loadVBData_.clear();
					loadIBData_.clear();
					loadGeometries_.clear();
					return false;
				}

				SharedPtr<YumeGeometry> geometry(new YumeGeometry);
				geometry->SetLodDistance(distance);

				// Prepare geometry to be defined during EndLoad()
				loadGeometries_[i][j].type_ = type;
				loadGeometries_[i][j].vbRef_ = vbRef;
				loadGeometries_[i][j].ibRef_ = ibRef;
				loadGeometries_[i][j].indexStart_ = indexStart;
				loadGeometries_[i][j].indexCount_ = indexCount;

				geometryLodLevels.push_back(geometry);
				memoryUse += sizeof(YumeGeometry);
			}

			geometries_.push_back(geometryLodLevels);
		}

		// Read morphs
		unsigned numMorphs = source.ReadUInt();
		morphs_.reserve(numMorphs);
		for(unsigned i = 0; i < numMorphs; ++i)
		{
			ModelMorph newMorph;

			newMorph.name_ = source.ReadString();
			newMorph.nameHash_ = newMorph.name_;
			newMorph.weight_ = 0.0f;
			unsigned numBuffers = source.ReadUInt();

			for(unsigned j = 0; j < numBuffers; ++j)
			{
				VertexBufferMorph newBuffer;
				unsigned bufferIndex = source.ReadUInt();

				newBuffer.elementMask_ = source.ReadUInt();
				newBuffer.vertexCount_ = source.ReadUInt();

				// Base size: size of each vertex index
				unsigned vertexSize = sizeof(unsigned);
				// Add size of individual elements
				if(newBuffer.elementMask_ & MASK_POSITION)
					vertexSize += sizeof(Vector3);
				if(newBuffer.elementMask_ & MASK_NORMAL)
					vertexSize += sizeof(Vector3);
				if(newBuffer.elementMask_ & MASK_TANGENT)
					vertexSize += sizeof(Vector3);
				newBuffer.dataSize_ = newBuffer.vertexCount_ * vertexSize;
				newBuffer.morphData_ = boost::shared_array<unsigned char>(new unsigned char[newBuffer.dataSize_]);

				source.Read(&newBuffer.morphData_[0],newBuffer.vertexCount_ * vertexSize);

				newMorph.buffers_[bufferIndex] = newBuffer;
				memoryUse += sizeof(VertexBufferMorph) + newBuffer.vertexCount_ * vertexSize;
			}

			morphs_.push_back(newMorph);
			memoryUse += sizeof(ModelMorph);
		}

		// Read skeleton
		skeleton_.Load(source);
		memoryUse += skeleton_.GetNumBones() * sizeof(Bone);

		// Read bounding box
		boundingBox_ = source.ReadBoundingBox();

		// Read geometry centers
		for(unsigned i = 0; i < geometries_.size() && !source.Eof(); ++i)
			geometryCenters_.push_back(source.ReadVector3());
		while(geometryCenters_.size() < geometries_.size())
			geometryCenters_.push_back(Vector3::ZERO);
		memoryUse += sizeof(Vector3) * geometries_.size();

		SetMemoryUsage(memoryUse);
		return true;
	}

	bool YumeModel::EndLoad()
	{
		// Upload vertex buffer data
		for(unsigned i = 0; i < vertexBuffers_.size(); ++i)
		{
			YumeVertexBuffer* buffer = vertexBuffers_[i];
			VertexBufferDesc& desc = loadVBData_[i];
			if(desc.data_)
			{
				buffer->SetShadowed(true);
				buffer->SetSize(desc.vertexCount_,desc.elementMask_);
				buffer->SetData(desc.data_.get());
			}
		}

		// Upload index buffer data
		for(unsigned i = 0; i < indexBuffers_.size(); ++i)
		{
			YumeIndexBuffer* buffer = indexBuffers_[i];
			IndexBufferDesc& desc = loadIBData_[i];
			if(desc.data_)
			{
				buffer->SetShadowed(true);
				buffer->SetSize(desc.indexCount_,desc.indexSize_ > sizeof(unsigned short));
				buffer->SetData(desc.data_.get());
			}
		}

		// Set up geometries
		for(unsigned i = 0; i < geometries_.size(); ++i)
		{
			for(unsigned j = 0; j < geometries_[i].size(); ++j)
			{
				YumeGeometry* geometry = geometries_[i][j];
				GeometryDesc& desc = loadGeometries_[i][j];
				geometry->SetVertexBuffer(0,vertexBuffers_[desc.vbRef_]);
				geometry->SetIndexBuffer(indexBuffers_[desc.ibRef_]);
				geometry->SetDrawRange(desc.type_,desc.indexStart_,desc.indexCount_);
			}
		}

		loadVBData_.clear();
		loadIBData_.clear();
		loadGeometries_.clear();
		return true;
	}

	void YumeModel::SetBoundingBox(const BoundingBox& box)
	{
		boundingBox_ = box;
	}

	bool YumeModel::SetVertexBuffers(const YumeVector<SharedPtr<YumeVertexBuffer> >::type& buffers,const YumeVector<unsigned>::type& morphRangeStarts,
		const YumeVector<unsigned>::type& morphRangeCounts)
	{
		for(unsigned i = 0; i < buffers.size(); ++i)
		{
			if(!buffers[i])
			{
				YUMELOG_ERROR("Null model vertex buffers specified");
				return false;
			}
			if(!buffers[i]->IsShadowed())
			{
				YUMELOG_ERROR("Model vertex buffers must be shadowed");
				return false;
			}
		}

		vertexBuffers_ = buffers;
		morphRangeStarts_.resize(buffers.size());
		morphRangeCounts_.resize(buffers.size());

		// If morph ranges are not specified for buffers, assume to be zero
		for(unsigned i = 0; i < buffers.size(); ++i)
		{
			morphRangeStarts_[i] = i < morphRangeStarts.size() ? morphRangeStarts[i] : 0;
			morphRangeCounts_[i] = i < morphRangeCounts.size() ? morphRangeCounts[i] : 0;
		}

		return true;
	}

	bool YumeModel::SetIndexBuffers(const YumeVector<SharedPtr<YumeIndexBuffer> >::type& buffers)
	{
		for(unsigned i = 0; i < buffers.size(); ++i)
		{
			if(!buffers[i])
			{
				YUMELOG_ERROR("Null model index buffers specified");
				return false;
			}
			if(!buffers[i]->IsShadowed())
			{
				YUMELOG_ERROR("Model index buffers must be shadowed");
				return false;
			}
		}

		indexBuffers_ = buffers;
		return true;
	}

	void YumeModel::SetNumGeometries(unsigned num)
	{
		geometries_.resize(num);
		geometryBoneMappings_.resize(num);
		geometryCenters_.resize(num);

		// For easier creation of from-scratch geometry, ensure that all geometries start with at least 1 LOD level (0 makes no sense)
		for(unsigned i = 0; i < geometries_.size(); ++i)
		{
			if(geometries_[i].empty())
				geometries_[i].resize(1);
		}
	}

	bool YumeModel::SetNumGeometryLodLevels(unsigned index,unsigned num)
	{
		if(index >= geometries_.size())
		{
			YUMELOG_ERROR("Geometry index out of bounds");
			return false;
		}
		if(!num)
		{
			YUMELOG_ERROR("Zero LOD levels not allowed");
			return false;
		}

		geometries_[index].resize(num);
		return true;
	}

	bool YumeModel::SetGeometry(unsigned index,unsigned lodLevel,YumeGeometry* geometry)
	{
		if(index >= geometries_.size())
		{
			YUMELOG_ERROR("Geometry index out of bounds");
			return false;
		}
		if(lodLevel >= geometries_[index].size())
		{
			YUMELOG_ERROR("LOD level index out of bounds");
			return false;
		}

		geometries_[index][lodLevel] = SharedPtr<YumeGeometry>(geometry);
		return true;
	}

	bool YumeModel::SetGeometryCenter(unsigned index,const Vector3& center)
	{
		if(index >= geometryCenters_.size())
		{
			YUMELOG_ERROR("Geometry index out of bounds");
			return false;
		}

		geometryCenters_[index] = center;
		return true;
	}

	void YumeModel::SetGeometryBoneMappings(const YumeVector<YumeVector<unsigned>::type >::type& geometryBoneMappings)
	{
		geometryBoneMappings_ = geometryBoneMappings;
	}

	void YumeModel::SetMorphs(const YumeVector<ModelMorph>::type& morphs)
	{
		morphs_ = morphs;
	}

	SharedPtr<YumeModel> YumeModel::Clone(const YumeString& cloneName) const
	{
		return SharedPtr<YumeModel>();
	}

	unsigned YumeModel::GetNumGeometryLodLevels(unsigned index) const
	{
		return index < geometries_.size() ? geometries_[index].size() : 0;
	}

	YumeGeometry* YumeModel::GetGeometry(unsigned index,unsigned lodLevel) const
	{
		if(index >= geometries_.size() || geometries_[index].empty())
			return 0;

		if(lodLevel >= geometries_[index].size())
			lodLevel = geometries_[index].size() - 1;

		return geometries_[index][lodLevel];
	}

	const ModelMorph* YumeModel::GetMorph(unsigned index) const
	{
		return index < morphs_.size() ? &morphs_[index] : 0;
	}

	const ModelMorph* YumeModel::GetMorph(const YumeString& name) const
	{
		return GetMorph(YumeHash(name));
	}

	const ModelMorph* YumeModel::GetMorph(YumeHash nameHash) const
	{
		for(YumeVector<ModelMorph>::const_iterator i = morphs_.begin(); i != morphs_.end(); ++i)
		{
			if(i->nameHash_ == nameHash)
				return &(*i);
		}

		return 0;
	}

	unsigned YumeModel::GetMorphRangeStart(unsigned bufferIndex) const
	{
		return bufferIndex < vertexBuffers_.size() ? morphRangeStarts_[bufferIndex] : 0;
	}

	unsigned YumeModel::GetMorphRangeCount(unsigned bufferIndex) const
	{
		return bufferIndex < vertexBuffers_.size() ? morphRangeCounts_[bufferIndex] : 0;
	}
}

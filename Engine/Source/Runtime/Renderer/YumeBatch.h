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
#ifndef __YumeBatch_h__
#define __YumeBatch_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeDrawable.h"
#include "YumeMaterial.h"
#include "Math/YumeMath.h"
#include "Math/YumeMatrix3x4.h"
#include "Math/YumeRect.h"

#include <boost/unordered_map.hpp>
//----------------------------------------------------------------------------
#include <xhash>

namespace YumeEngine
{

	class YumeCamera;
	class YumeDrawable;
	class YumeGeometry;
	class YumeLight;
	class YumeMaterial;
	class Matrix3x4;
	class YumeRenderPass;
	class YumeShaderVariation;
	class YumeTexture2D;
	class YumeVertexBuffer;
	class YumeRenderView;
	class YumeRendererEnvironment;
	struct LightBatchQueue;



	struct Batch
	{

		Batch():
			isBase_(false),
			lightQueue_(0)
		{

		}


		Batch(const SourceBatch& rhs):
			distance_(rhs.distance_),
			renderOrder_(rhs.material_ ? rhs.material_->GetRenderOrder() : DEFAULT_RENDER_ORDER),
			isBase_(false),
			geometry_(rhs.geometry_),
			material_(rhs.material_),
			worldTransform_(rhs.worldTransform_),
			numWorldTransforms_(rhs.numWorldTransforms_),
			lightQueue_(0),
			geometryType_(rhs.geometryType_),
			vertexShader_(0),
			geometryShader_(0),
			pixelShader_(0)
		{
		}


		void CalculateSortKey();
		void Prepare(YumeRenderView* view,YumeCamera* camera,bool setModelTransform,bool allowDepthWrite) const;
		void Draw(YumeRenderView* view,YumeCamera* camera,bool allowDepthWrite) const;

		unsigned long long sortKey_;
		float distance_;
		unsigned char renderOrder_;
		unsigned char lightMask_;
		bool isBase_;
		YumeGeometry* geometry_;
		YumeMaterial* material_;
		const Matrix3x4* worldTransform_;
		unsigned numWorldTransforms_;
		YumeRendererEnvironment* zone_;
		LightBatchQueue* lightQueue_;
		YumeRenderPass* pass_;
		YumeShaderVariation* vertexShader_;
		YumeShaderVariation* geometryShader_;
		YumeShaderVariation* pixelShader_;

		GeometryType geometryType_;
	};


	struct InstanceData
	{

		InstanceData()
		{
		}


		InstanceData(const Matrix3x4* worldTransform,float distance):
			worldTransform_(worldTransform),
			distance_(distance)
		{
		}


		const Matrix3x4* worldTransform_;

		float distance_;
	};


	struct BatchGroup : public Batch
	{

		BatchGroup():
			startIndex_(M_MAX_UNSIGNED)
		{
		}


		BatchGroup(const Batch& batch):
			Batch(batch),
			startIndex_(M_MAX_UNSIGNED)
		{
		}


		~BatchGroup()
		{
		}


		void AddTransforms(const Batch& batch)
		{
			InstanceData newInstance;
			newInstance.distance_ = batch.distance_;

			for(unsigned i = 0; i < batch.numWorldTransforms_; ++i)
			{
				newInstance.worldTransform_ = &batch.worldTransform_[i];
				instances_.push_back(newInstance);
			}
		}


		void SetTransforms(void* lockedData,unsigned& freeIndex);
		void Draw(YumeRenderView* view,YumeCamera* camera,bool allowDepthWrite) const;
		YumeVector<InstanceData>::type instances_;
		unsigned startIndex_;
	};


	struct BatchGroupKey
	{

		BatchGroupKey()
		{
		}


		BatchGroupKey(const Batch& batch):
			zone_(batch.zone_),
			lightQueue_(batch.lightQueue_),
			pass_(batch.pass_),
			material_(batch.material_),
			geometry_(batch.geometry_),
			renderOrder_(batch.renderOrder_)
		{
		}


		YumeRendererEnvironment* zone_;
		LightBatchQueue* lightQueue_;
		YumeRenderPass* pass_;
		YumeMaterial* material_;
		YumeGeometry* geometry_;

		unsigned char renderOrder_;
		bool operator ==(const BatchGroupKey& rhs) const
		{
			return zone_ == rhs.zone_ && lightQueue_ == rhs.lightQueue_ && pass_ == rhs.pass_ && material_ == rhs.material_ &&
				geometry_ == rhs.geometry_ && renderOrder_ == rhs.renderOrder_;
		}
		bool operator !=(const BatchGroupKey& rhs) const
		{
			return zone_ != rhs.zone_ || lightQueue_ != rhs.lightQueue_ || pass_ != rhs.pass_ || material_ != rhs.material_ ||
				geometry_ != rhs.geometry_ || renderOrder_ != rhs.renderOrder_;
		}
		bool operator < (const BatchGroupKey& rhs) const
		{
			return zone_ == rhs.zone_ && lightQueue_ == rhs.lightQueue_ && pass_ == rhs.pass_ && material_ == rhs.material_ &&
				geometry_ == rhs.geometry_ && renderOrder_ == rhs.renderOrder_;
		}

		unsigned ToHash() const;
	};

	struct BatchGroupKeyHash
		: std::unary_function<BatchGroupKey,std::size_t>
	{
		std::size_t operator()(BatchGroupKey const& p) const
		{
			return p.ToHash();
		}
	};
}
namespace std
{
	template <>
	struct hash<YumeEngine::BatchGroupKey>
	{
		size_t operator()(const YumeEngine::BatchGroupKey& x) const
		{
			return YumeEngine::YumeHash(x.ToHash());
		}
	};
}
namespace YumeEngine
{
	struct BatchQueue
	{
	public:

		void Clear(int maxSortedInstances);
		void SortBackToFront();
		void SortFrontToBack();
		void SortFrontToBack2Pass(YumePodVector<Batch*>::type& batches);
		void SetTransforms(void* lockedData,unsigned& freeIndex);
		void Draw(YumeRenderView* view,YumeCamera* camera,bool markToStencil,bool usingLightOptimization,bool allowDepthWrite) const;
		unsigned GetNumInstances() const;


		bool IsEmpty() const { return batches_.empty() && batchGroups_.empty(); }


		YumeMap<BatchGroupKey,BatchGroup>::type batchGroups_;
		YumeMap<unsigned,unsigned>::type shaderRemapping_;
		YumeMap<unsigned short,unsigned short>::type materialRemapping_;
		YumeMap<unsigned short,unsigned short>::type geometryRemapping_;


		YumePodVector<Batch>::type batches_;
		YumePodVector<Batch*>::type sortedBatches_;
		YumePodVector<BatchGroup*>::type sortedBatchGroups_;

		unsigned maxSortedInstances_;
	};


	struct ShadowBatchQueue
	{
		YumeCamera* shadowCamera_;
		IntRect shadowViewport_;
		BatchQueue shadowBatches_;
		float nearSplit_;
		float farSplit_;
	};


	struct LightBatchQueue
	{
		YumeLight* light_;
		bool negative_;
		YumeTexture2D* shadowMap_;
		BatchQueue litBaseBatches_;
		BatchQueue litBatches_;
		YumeVector<ShadowBatchQueue>::type shadowSplits_;
		YumePodVector<YumeLight*>::type vertexLights_;
		YumePodVector<Batch>::type volumeBatches_;
	};
}


//----------------------------------------------------------------------------
#endif

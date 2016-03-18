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
#include "YumeSkeleton.h"
#include "Core/YumeFile.h"
#include "YumeResourceManager.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	YumeSkeleton::YumeSkeleton():
		rootBoneIndex_(M_MAX_UNSIGNED)
	{
	}

	YumeSkeleton::~YumeSkeleton()
	{
	}

	bool YumeSkeleton::Load(YumeFile& source)
	{
		ClearBones();

		if(source.Eof())
			return false;

		unsigned bones = source.ReadUInt();
		bones_.reserve(bones);

		for(unsigned i = 0; i < bones; ++i)
		{
			Bone newBone;
			newBone.name_ = source.ReadString();
			newBone.nameHash_ = newBone.name_;
			newBone.parentIndex_ = source.ReadUInt();
			newBone.initialPosition_ = source.ReadVector3();
			newBone.initialRotation_ = source.ReadQuaternion();
			newBone.initialScale_ = source.ReadVector3();
			source.Read(&newBone.offsetMatrix_.m00_,sizeof(Matrix3x4));

			// Read bone collision data
			newBone.collisionMask_ = source.ReadUByte();
			if(newBone.collisionMask_ & BONECOLLISION_SPHERE)
				newBone.radius_ = source.ReadFloat();
			if(newBone.collisionMask_ & BONECOLLISION_BOX)
				newBone.boundingBox_ = source.ReadBoundingBox();

			if(newBone.parentIndex_ == i)
				rootBoneIndex_ = i;

			bones_.push_back(newBone);
		}

		return true;
	}

	void YumeSkeleton::Define(const YumeSkeleton& src)
	{
		ClearBones();

		bones_ = src.bones_;
		// Make sure we clear node references, if they exist
		// (AnimatedModel will create new nodes on its own)
		for(YumeVector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
			i->node_.Reset();
		rootBoneIndex_ = src.rootBoneIndex_;
	}

	void YumeSkeleton::SetRootBoneIndex(unsigned index)
	{
		if(index < bones_.size())
			rootBoneIndex_ = index;
		else
			YUMELOG_ERROR("Root bone index out of bounds");
	}

	void YumeSkeleton::ClearBones()
	{
		bones_.clear();
		rootBoneIndex_ = M_MAX_UNSIGNED;
	}

	void YumeSkeleton::Reset()
	{
		for(YumeVector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
		{
			
			if(i->animated_ && i->node_.Expired())
				i->node_->SetTransform(i->initialPosition_,i->initialRotation_,i->initialScale_);
		}
	}

	void YumeSkeleton::ResetSilent()
	{

	}


	Bone* YumeSkeleton::GetRootBone()
	{
		return GetBone(rootBoneIndex_);
	}

	Bone* YumeSkeleton::GetBone(unsigned index)
	{
		return index < bones_.size() ? &bones_[index] : (Bone*)0;
	}

	Bone* YumeSkeleton::GetBone(const YumeString& name)
	{
		return GetBone(YumeHash(name));
	}

	Bone* YumeSkeleton::GetBone(const char* name)
	{
		return GetBone(YumeHash(name));
	}

	Bone* YumeSkeleton::GetBone(YumeHash nameHash)
	{
		for(YumeVector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
		{
			if(i->nameHash_ == nameHash)
				return &(*i);
		}

		return 0;
	}
}

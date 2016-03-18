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
#ifndef __YumeSkeleton_h__
#define __YumeSkeleton_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeBoundingBox.h"
#include "Scene/YumeSceneNode.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const unsigned BONECOLLISION_NONE = 0x0;
	static const unsigned BONECOLLISION_SPHERE = 0x1;
	static const unsigned BONECOLLISION_BOX = 0x2;

	class YumeFile;
	class YumeResourceManager;

	/// %Bone in a skeleton.
	struct Bone
	{
		/// Construct with defaults.
		Bone():
			parentIndex_(0),
			initialPosition_(Vector3::ZERO),
			initialRotation_(Quaternion::IDENTITY),
			initialScale_(Vector3::ONE),
			animated_(true),
			collisionMask_(0),
			radius_(0.0f)
		{
		}

		/// Bone name.
		YumeString name_;
		/// Bone name hash.
		YumeHash nameHash_;
		/// Parent bone index.
		unsigned parentIndex_;
		/// Reset position.
		Vector3 initialPosition_;
		/// Reset rotation.
		Quaternion initialRotation_;
		/// Reset scale.
		Vector3 initialScale_;
		/// Offset matrix.
		Matrix3x4 offsetMatrix_;
		/// Animation enable flag.
		bool animated_;
		/// Supported collision types.
		unsigned char collisionMask_;
		/// Radius.
		float radius_;
		/// Local-space bounding box.
		BoundingBox boundingBox_;
		/// Scene node.
		WeakPtr<YumeSceneNode> node_;
	};

	/// Hierarchical collection of bones.
	class YumeAPIExport YumeSkeleton
	{
	public:
		/// Construct an empty skeleton.
		YumeSkeleton();
		/// Destruct.
		~YumeSkeleton();

		/// Read from a stream. Return true if successful.
		bool Load(YumeFile& source);
		/// Write to a stream. Return true if successful.
		void Define(const YumeSkeleton& src);
		/// Set root bone's index.
		void SetRootBoneIndex(unsigned index);
		/// Clear bones.
		void ClearBones();
		/// Reset all animating bones to initial positions.
		void Reset();

		/// Return all bones.
		const YumeVector<Bone>::type& GetBones() const { return bones_; }

		/// Return modifiable bones.
		YumeVector<Bone>::type& GetModifiableBones() { return bones_; }

		/// Return number of bones.
		unsigned GetNumBones() const { return bones_.size(); }

		/// Return root bone.
		Bone* GetRootBone();
		/// Return bone by index.
		Bone* GetBone(unsigned index);
		/// Return bone by name.
		Bone* GetBone(const YumeString& boneName);
		/// Return bone by name.
		Bone* GetBone(const char* boneName);
		/// Return bone by name hash.
		Bone* GetBone(YumeHash boneNameHash);

		/// Reset all animating bones to initial positions without marking the nodes dirty. Requires the node dirtying to be performed later.
		void ResetSilent();

	private:
		/// Bones.
		YumeVector<Bone>::type bones_;
		/// Root bone index.
		unsigned rootBoneIndex_;
	};

}


//----------------------------------------------------------------------------
#endif

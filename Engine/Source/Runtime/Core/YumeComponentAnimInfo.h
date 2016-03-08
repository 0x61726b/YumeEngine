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
#ifndef __YumeComponentAnimInfo_h__
#define __YumeComponentAnimInfo_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	/// Animation wrap mode.
	enum WrapMode
	{
		/// Loop mode.
		WM_LOOP = 0,
		/// Play once, when animation finished it will be removed.
		WM_ONCE,
		/// Clamp mode.
		WM_CLAMP,
	};


	class YumeBase;
	class ValueAnimation;
	struct VAnimEventFrame;

	/// Base class for a value animation instance, which includes animation runtime information and updates the target object's value automatically.
	class ValueAnimationInfo
	{
	public:
		/// Construct without target object.
		ValueAnimationInfo(ValueAnimation* animation,WrapMode wrapMode,float speed);
		/// Construct with target object.
		ValueAnimationInfo(YumeBase* target,ValueAnimation* animation,WrapMode wrapMode,float speed);
		/// Copy construct.
		ValueAnimationInfo(const ValueAnimationInfo& other);
		/// Destruct.
		~ValueAnimationInfo();

		/// Advance time position and apply. Return true when the animation is finished. No-op when the target object is not defined.
		bool Update(float timeStep);
		/// Set time position and apply. Return true when the animation is finished. No-op when the target object is not defined.
		bool SetTime(float time);

		/// Set wrap mode.
		void SetWrapMode(WrapMode wrapMode) { wrapMode_ = wrapMode; }

		/// Set speed.
		void SetSpeed(float speed) { speed_ = speed; }

		/// Return target object.
		YumeBase* GetTarget() const;

		/// Return animation.
		ValueAnimation* GetAnimation() const { return animation_.get(); }

		/// Return wrap mode.
		WrapMode GetWrapMode() const { return wrapMode_; }

		/// Return time position.
		float GetTime() const { return currentTime_; }

		/// Return speed.
		float GetSpeed() const { return speed_; }

	protected:
		/// Apply new animation value to the target object. Called by Update().
		virtual void ApplyValue(const Variant& newValue);
		/// Calculate scaled time.
		float CalculateScaledTime(float currentTime,bool& finished) const;
		/// Return event frames.
		void GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames);

		/// Target object.
		SharedPtr<YumeBase> target_;
		/// Attribute animation.
		SharedPtr<ValueAnimation> animation_;
		/// Wrap mode.
		WrapMode wrapMode_;
		/// Animation speed.
		float speed_;
		/// Current time.
		float currentTime_;
		/// Last scaled time.
		float lastScaledTime_;
	};
}


//----------------------------------------------------------------------------
#endif

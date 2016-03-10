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
	
	enum WrapMode
	{
		
		WM_LOOP = 0,
		
		WM_ONCE,
		
		WM_CLAMP,
	};


	class YumeBase;
	class ValueAnimation;
	struct VAnimEventFrame;

	
	class ValueAnimationInfo
	{
	public:
		
		ValueAnimationInfo(ValueAnimation* animation,WrapMode wrapMode,float speed);
		
		ValueAnimationInfo(YumeBase* target,ValueAnimation* animation,WrapMode wrapMode,float speed);
		
		ValueAnimationInfo(const ValueAnimationInfo& other);
		
		~ValueAnimationInfo();

		
		bool Update(float timeStep);
		
		bool SetTime(float time);

		
		void SetWrapMode(WrapMode wrapMode) { wrapMode_ = wrapMode; }

		
		void SetSpeed(float speed) { speed_ = speed; }

		
		YumeBase* GetTarget() const;

		
		ValueAnimation* GetAnimation() const { return animation_.get(); }

		
		WrapMode GetWrapMode() const { return wrapMode_; }

		
		float GetTime() const { return currentTime_; }

		
		float GetSpeed() const { return speed_; }

	protected:
		
		virtual void ApplyValue(const Variant& newValue);
		
		float CalculateScaledTime(float currentTime,bool& finished) const;
		
		void GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames);

		
		SharedPtr<YumeBase> target_;
		
		SharedPtr<ValueAnimation> animation_;
		
		WrapMode wrapMode_;
		
		float speed_;
		
		float currentTime_;
		
		float lastScaledTime_;
	};
}


//----------------------------------------------------------------------------
#endif

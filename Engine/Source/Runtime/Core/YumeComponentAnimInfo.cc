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
#include "YumeComponentAnimInfo.h"
#include "YumeComponentAnimation.h"
#include "Logging/logging.h"




namespace YumeEngine
{

	ValueAnimationInfo::ValueAnimationInfo(ValueAnimation* animation,WrapMode wrapMode,float speed):
		animation_(animation),
		wrapMode_(wrapMode),
		speed_(speed),
		currentTime_(0.0f),
		lastScaledTime_(0.0f)
	{
		speed_ = Max(0.0f,speed_);
	}

	ValueAnimationInfo::ValueAnimationInfo(YumeBase* target,ValueAnimation* animation,WrapMode wrapMode,float speed):
		target_(target),
		animation_(animation),
		wrapMode_(wrapMode),
		speed_(speed),
		currentTime_(0.0f),
		lastScaledTime_(0.0f)
	{
		speed_ = Max(0.0f,speed_);
	}

	ValueAnimationInfo::ValueAnimationInfo(const ValueAnimationInfo& other):
		target_(other.target_),
		animation_(other.animation_),
		wrapMode_(other.wrapMode_),
		speed_(other.speed_),
		currentTime_(0.0f),
		lastScaledTime_(0.0f)
	{
	}

	ValueAnimationInfo::~ValueAnimationInfo()
	{
	}

	bool ValueAnimationInfo::Update(float timeStep)
	{
		if(!animation_ || !target_)
			return true;

		return SetTime(currentTime_ + timeStep * speed_);
	}

	bool ValueAnimationInfo::SetTime(float time)
	{
		if(!animation_ || !target_)
			return true;

		currentTime_ = time;

		if(!animation_->IsValid())
			return true;

		bool finished = false;

		// Calculate scale time by wrap mode
		float scaledTime = CalculateScaledTime(currentTime_,finished);

		// Apply to the target object
		ApplyValue(animation_->GetAnimationValue(scaledTime));

		// Send keyframe event if necessary
		if(animation_->HasEventFrames())
		{
			YumeVector<const VAnimEventFrame*>::type eventFrames;
			GetEventFrames(lastScaledTime_,scaledTime,eventFrames);

			/*for(unsigned i = 0; i < eventFrames.Size(); ++i)
				target_->SendEvent(eventFrames[i]->eventType_,const_cast<VariantMap&>(eventFrames[i]->eventData_));*/
		}

		lastScaledTime_ = scaledTime;

		return finished;
	}

	YumeBase* ValueAnimationInfo::GetTarget() const
	{
		return target_.get();
	}

	void ValueAnimationInfo::ApplyValue(const Variant& newValue)
	{
	}

	float ValueAnimationInfo::CalculateScaledTime(float currentTime,bool& finished) const
	{
		float beginTime = animation_->GetBeginTime();
		float endTime = animation_->GetEndTime();

		switch(wrapMode_)
		{
		case WM_LOOP:
		{
			float span = endTime - beginTime;
			float time = fmodf(currentTime - beginTime,span);
			if(time < 0.0f)
				time += span;
			return beginTime + time;
		}

		case WM_ONCE:
			finished = (currentTime >= endTime);
			// Fallthrough

		case WM_CLAMP:
			return Clamp(currentTime,beginTime,endTime);

		default:
			YUMELOG_ERROR("Unsupported attribute animation wrap mode");
			return beginTime;
		}
	}

	void ValueAnimationInfo::GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames)
	{
		switch(wrapMode_)
		{
		case WM_LOOP:
			if(beginTime < endTime)
				animation_->GetEventFrames(beginTime,endTime,eventFrames);
			else
			{
				animation_->GetEventFrames(beginTime,animation_->GetEndTime(),eventFrames);
				animation_->GetEventFrames(animation_->GetBeginTime(),endTime,eventFrames);
			}
			break;

		case WM_ONCE:
		case WM_CLAMP:
			animation_->GetEventFrames(beginTime,endTime,eventFrames);
			break;
		}
	}

}

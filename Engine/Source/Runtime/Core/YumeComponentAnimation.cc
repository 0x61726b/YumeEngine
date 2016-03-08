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
#include "YumeComponentAnimation.h"
#include "YumeFile.h"
#include "Math/YumeMath.h"
#include "Logging/logging.h"



namespace YumeEngine
{

	const char* interpMethodNames[] =
	{
		"Linear",
		"Spline",
		0
	};

	ValueAnimation::ValueAnimation():
		owner_(0),
		interpolationMethod_(IM_LINEAR),
		splineTension_(0.5f),
		valueType_(VAR_EMPTY),
		interpolatable_(false),
		beginTime_(M_INFINITY),
		endTime_(-M_INFINITY),
		splineTangentsDirty_(false)
	{
	}

	ValueAnimation::~ValueAnimation()
	{
	}

	bool ValueAnimation::BeginLoad(YumeFile& source)
	{
		return true;
	}

	bool ValueAnimation::Save(YumeFile& dest) const
	{
		return true;
	}

	bool ValueAnimation::LoadXML(const XmlNode& source)
	{
		
		return true;
	}

	bool ValueAnimation::SaveXML(XmlNode& dest) const
	{

		return true;
	}

	void ValueAnimation::SetValueType(VariantType valueType)
	{
		if(valueType == valueType_)
			return;

		valueType_ = valueType;
		interpolatable_ =
			(valueType_ == VAR_FLOAT) || (valueType_ == VAR_VECTOR2) || (valueType_ == VAR_VECTOR3) || (valueType_ == VAR_VECTOR4) ||
			(valueType_ == VAR_QUATERNION) || (valueType_ == VAR_COLOR);

		if((valueType_ == VAR_INTRECT) || (valueType_ == VAR_INTVECTOR2))
		{
			interpolatable_ = true;
			// Force linear interpolation for IntRect and IntVector2
			interpolationMethod_ = IM_LINEAR;
		}

		keyFrames_.clear();
		eventFrames_.clear();
		beginTime_ = M_INFINITY;
		endTime_ = -M_INFINITY;
	}

	void ValueAnimation::SetOwner(void* owner)
	{
		owner_ = owner;
	}

	void ValueAnimation::SetInterpolationMethod(InterpMethod method)
	{
		if(method == interpolationMethod_)
			return;

		// Force linear interpolation for IntRect and IntVector2
		if((valueType_ == VAR_INTRECT) || (valueType_ == VAR_INTVECTOR2))
			method = IM_LINEAR;

		interpolationMethod_ = method;
		splineTangentsDirty_ = true;
	}

	void ValueAnimation::SetSplineTension(float tension)
	{
		splineTension_ = tension;
		splineTangentsDirty_ = true;
	}

	bool ValueAnimation::SetKeyFrame(float time,const Variant& value)
	{
		if(valueType_ == VAR_EMPTY)
			SetValueType(value.GetType());
		else if(value.GetType() != valueType_)
			return false;

		VAnimKeyFrame keyFrame;
		keyFrame.time_ = time;
		keyFrame.value_ = value;

		if(keyFrames_.empty() || time > keyFrames_.back().time_)
			keyFrames_.push_back(keyFrame);
		else
		{
			for(unsigned i = 0; i < keyFrames_.size(); ++i)
			{
				// Guard against interpolation error caused by division by error due to 0 delta time between two key frames
				if(time == keyFrames_[i].time_)
					return false;
				if(time < keyFrames_[i].time_)
				{
					keyFrames_.insert(keyFrames_.begin() + i,keyFrame);
					break;
				}
			}
		}

		beginTime_ = Min(time,beginTime_);
		endTime_ = Max(time,endTime_);
		splineTangentsDirty_ = true;

		return true;
	}

	void ValueAnimation::SetEventFrame(float time,const YumeHash& eventType,const VariantMap& eventData)
	{
		VAnimEventFrame eventFrame;
		eventFrame.time_ = time;
		eventFrame.eventType_ = eventType;
		eventFrame.eventData_ = eventData;

		if(eventFrames_.empty() || time >= eventFrames_.back().time_)
			eventFrames_.push_back(eventFrame);
		else
		{
			for(unsigned i = 0; i < eventFrames_.size(); ++i)
			{
				if(time < eventFrames_[i].time_)
				{
					eventFrames_.insert(eventFrames_.begin() + i,eventFrame);
					break;
				}
			}
		}
	}

	bool ValueAnimation::IsValid() const
	{
		return (interpolationMethod_ == IM_LINEAR && keyFrames_.size() > 1) ||
			(interpolationMethod_ == IM_SPLINE && keyFrames_.size() > 2);
	}

	Variant ValueAnimation::GetAnimationValue(float scaledTime)
	{
		unsigned index = 1;
		for(; index < keyFrames_.size(); ++index)
		{
			if(scaledTime < keyFrames_[index].time_)
				break;
		}

		if(index >= keyFrames_.size() || !interpolatable_)
			return keyFrames_[index - 1].value_;
		else
		{
			if(interpolationMethod_ == IM_LINEAR)
				return LinearInterpolation(index - 1,index,scaledTime);
			else
				return SplineInterpolation(index - 1,index,scaledTime);
		}
	}

	void ValueAnimation::GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames) const
	{
		for(unsigned i = 0; i < eventFrames_.size(); ++i)
		{
			const VAnimEventFrame& eventFrame = eventFrames_[i];
			if(eventFrame.time_ > endTime)
				break;

			if(eventFrame.time_ >= beginTime)
				eventFrames.push_back(&eventFrame);
		}
	}

	Variant ValueAnimation::LinearInterpolation(unsigned index1,unsigned index2,float scaledTime) const
	{
		const VAnimKeyFrame& keyFrame1 = keyFrames_[index1];
		const VAnimKeyFrame& keyFrame2 = keyFrames_[index2];

		float t = (scaledTime - keyFrame1.time_) / (keyFrame2.time_ - keyFrame1.time_);
		const Variant& value1 = keyFrame1.value_;
		const Variant& value2 = keyFrame2.value_;

		switch(valueType_)
		{
		case VAR_FLOAT:
			return Lerp(value1.Get<float>(),value2.Get<float>(),t);

		case VAR_VECTOR2:
			return value1.GetVector2().Lerp(value2.GetVector2(),t);

		case VAR_VECTOR3:
			return value1.GetVector3().Lerp(value2.GetVector3(),t);

		case VAR_VECTOR4:
			return value1.GetVector4().Lerp(value2.GetVector4(),t);

		case VAR_QUATERNION:
			return value1.GetQuaternion().Slerp(value2.GetQuaternion(),t);

		case VAR_COLOR:
			return value1.GetColor().Lerp(value2.GetColor(),t);

		case VAR_INTRECT:
		{
			float s = 1.0f - t;
			const IntRect& r1 = value1.GetIntRect();
			const IntRect& r2 = value2.GetIntRect();
			return IntRect((int)(r1.left_ * s + r2.left_ * t),(int)(r1.top_ * s + r2.top_ * t),(int)(r1.right_ * s + r2.right_ * t),
				(int)(r1.bottom_ * s + r2.bottom_ * t));
		}

		case VAR_INTVECTOR2:
		{
			float s = 1.0f - t;
			const IntVector2& v1 = value1.GetIntVector2();
			const IntVector2& v2 = value2.GetIntVector2();
			return IntVector2((int)(v1.x_ * s + v2.x_ * t),(int)(v1.y_ * s + v2.y_ * t));
		}

		case VAR_DOUBLE:
			return value1.GetDouble() * (1.0f - t) + value2.GetDouble() * t;

		default:
			YUMELOG_ERROR("Invalid value type for linear interpolation");
			return Variant();
		}
	}

	Variant ValueAnimation::SplineInterpolation(unsigned index1,unsigned index2,float scaledTime)
	{
		if(splineTangentsDirty_)
			UpdateSplineTangents();

		const VAnimKeyFrame& keyFrame1 = keyFrames_[index1];
		const VAnimKeyFrame& keyFrame2 = keyFrames_[index2];

		float t = (scaledTime - keyFrame1.time_) / (keyFrame2.time_ - keyFrame1.time_);

		float tt = t * t;
		float ttt = t * tt;

		float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
		float h2 = -2.0f * ttt + 3.0f * tt;
		float h3 = ttt - 2.0f * tt + t;
		float h4 = ttt - tt;

		const Variant& v1 = keyFrame1.value_;
		const Variant& v2 = keyFrame2.value_;
		const Variant& t1 = splineTangents_[index1];
		const Variant& t2 = splineTangents_[index2];

		switch(valueType_)
		{
		case VAR_FLOAT:
			return v1.Get<float>() * h1 + v2.Get<float>() * h2 + t1.Get<float>() * h3 + t2.Get<float>()* h4;

		/*case VAR_VECTOR2:
			return v1.GetVector2() * h1 + v2.GetVector2() * h2 + t1.GetVector2() * h3 + t2.GetVector2() * h4;

		case VAR_VECTOR3:
			return v1.GetVector3() * h1 + v2.GetVector3() * h2 + t1.GetVector3() * h3 + t2.GetVector3() * h4;

		case VAR_VECTOR4:
			return v1.GetVector4() * h1 + v2.GetVector4() * h2 + t1.GetVector4() * h3 + t2.GetVector4() * h4;

		case VAR_QUATERNION:
			return v1.GetQuaternion() * h1 + v2.GetQuaternion() * h2 + t1.GetQuaternion() * h3 + t2.GetQuaternion() * h4;

		case VAR_COLOR:
			return v1.GetColor() * h1 + v2.GetColor() * h2 + t1.GetColor() * h3 + t2.GetColor() * h4;

		case VAR_DOUBLE:
			return v1.GetDouble() * h1 + v2.GetDouble() * h2 + t1.GetDouble() * h3 + t2.GetDouble() * h4;
*/
		default:
			YUMELOG_ERROR("Invalid value type for spline interpolation");
			return Variant();
		}
	}

	void ValueAnimation::UpdateSplineTangents()
	{
		splineTangents_.clear();

		if(!IsValid())
			return;

		unsigned size = keyFrames_.size();
		splineTangents_.resize(size);

		for(unsigned i = 1; i < size - 1; ++i)
			splineTangents_[i] = SubstractAndMultiply(keyFrames_[i + 1].value_,keyFrames_[i - 1].value_,splineTension_);

		// If spline is not closed, make end point's tangent zero
		if(keyFrames_[0].value_ != keyFrames_[size - 1].value_)
			splineTangents_[0] = splineTangents_[size - 1] =
			SubstractAndMultiply(keyFrames_[0].value_,keyFrames_[0].value_,splineTension_);
		else
			splineTangents_[0] = splineTangents_[size - 1] =
			SubstractAndMultiply(keyFrames_[1].value_,keyFrames_[size - 2].value_,splineTension_);

		splineTangentsDirty_ = false;
	}

	Variant ValueAnimation::SubstractAndMultiply(const Variant& value1,const Variant& value2,float t) const
	{
		switch(valueType_)
		{
		case VAR_FLOAT:
			return (value1.GetFloat() - value2.GetFloat()) * t;

		case VAR_VECTOR2:
			return (value1.GetVector2() - value2.GetVector2()) * t;

		case VAR_VECTOR3:
			return (value1.GetVector3() - value2.GetVector3()) * t;

		case VAR_VECTOR4:
			return (value1.GetVector4() - value2.GetVector4()) * t;

		case VAR_QUATERNION:
			return (value1.GetQuaternion() - value2.GetQuaternion()) * t;

		case VAR_COLOR:
			return (value1.GetColor() - value2.GetColor()) * t;

		case VAR_DOUBLE:
			return (value1.GetDouble() - value2.GetDouble()) * t;

		default:
			YUMELOG_ERROR("Invalid value type for spline interpolation's substract and multiply operation");
			return Variant::EMPTY;
		}
	}
}

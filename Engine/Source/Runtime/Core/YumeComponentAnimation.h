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
#ifndef __YumeComponentAnimation_h__
#define __YumeComponentAnimation_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeResource.h"
#include "Core/YumeVariant.h"

#include "YumeXmlParser.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class XMLElement;
	class JSONValue;

	/// Interpolation method.
	enum InterpMethod
	{
		/// Linear interpolation (default).
		IM_LINEAR = 0,
		/// Cardinal spline interpolation, default tension value is 0.5f. For more information please refer to http://cubic.org/docs/hermite.htm.
		IM_SPLINE,
	};

	/// Value animation key frame.
	struct VAnimKeyFrame
	{
		/// Time.
		float time_;
		/// Value.
		Variant value_;
	};

	/// Value animation event frame.
	struct VAnimEventFrame
	{
		/// Time.
		float time_;
		/// Event type.
		YumeHash eventType_;
		/// Event data.
		VariantMap eventData_;
	};

	/// Value animation class.
	class YumeAPIExport ValueAnimation : public YumeResource
	{
	public:
		ValueAnimation();
		virtual ~ValueAnimation();
		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);
		/// Save resource. Return true if successful.
		virtual bool Save(YumeFile& dest) const;
		/// Load from XML data. Return true if successful.
		bool LoadXML(const XmlNode& source);
		/// Save as XML data. Return true if successful.
		bool SaveXML(XmlNode& dest) const;
		
		/// Set owner.
		void SetOwner(void* owner);
		/// Set interpolation method.
		void SetInterpolationMethod(InterpMethod method);
		/// Set spline tension, should be between 0.0f and 1.0f, but this is not a must.
		void SetSplineTension(float tension);
		/// Set value type.
		void SetValueType(VariantType valueType);

		/// Set key frame.
		bool SetKeyFrame(float time,const Variant& value);
		/// Set event frame.
		void SetEventFrame(float time,const YumeHash& eventType,const VariantMap& eventData = VariantMap());

		/// Return animation is valid.
		bool IsValid() const;

		/// Return owner.
		void* GetOwner() const { return owner_; }

		/// Return interpolation method.
		InterpMethod GetInterpolationMethod() const { return interpolationMethod_; }

		/// Return spline tension.
		float GetSplineTension() const { return splineTension_; }

		/// Return value type.
		VariantType GetValueType() const { return valueType_; }

		/// Return begin time.
		float GetBeginTime() const { return beginTime_; }

		/// Return end time.
		float GetEndTime() const { return endTime_; }

		/// Return animation value.
		Variant GetAnimationValue(float scaledTime);

		/// Has event frames.
		bool HasEventFrames() const { return !eventFrames_.empty(); }

		/// Return all event frames between time.
		void GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames) const;

	protected:
		/// Linear interpolation.
		Variant LinearInterpolation(unsigned index1,unsigned index2,float scaledTime) const;
		/// Spline interpolation.
		Variant SplineInterpolation(unsigned index1,unsigned index2,float scaledTime);
		/// Update spline tangents.
		void UpdateSplineTangents();
		/// Return (value1 - value2) * t.
		Variant SubstractAndMultiply(const Variant& value1,const Variant& value2,float t) const;

		/// Owner.
		void* owner_;
		/// Interpolation method.
		InterpMethod interpolationMethod_;
		/// Spline tension.
		float splineTension_;
		/// Value type.
		VariantType valueType_;
		/// Interpolatable flag.
		bool interpolatable_;
		/// Begin time.
		float beginTime_;
		/// End time.
		float endTime_;
		/// Key frames.
		YumeVector<VAnimKeyFrame>::type keyFrames_;
		/// Spline tangents.
		YumeVector<Variant>::type splineTangents_;
		/// Spline tangents dirty.
		bool splineTangentsDirty_;
		/// Event frames.
		YumeVector<VAnimEventFrame>::type eventFrames_;
	};
}


//----------------------------------------------------------------------------
#endif

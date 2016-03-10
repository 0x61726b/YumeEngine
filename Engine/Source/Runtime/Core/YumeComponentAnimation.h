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

	
	enum InterpMethod
	{
		
		IM_LINEAR = 0,
		
		IM_SPLINE,
	};

	
	struct VAnimKeyFrame
	{
		
		float time_;
		
		Variant value_;
	};

	
	struct VAnimEventFrame
	{
		
		float time_;
		
		YumeHash eventType_;
		
		VariantMap eventData_;
	};

	
	class YumeAPIExport ValueAnimation : public YumeResource
	{
	public:
		ValueAnimation();
		virtual ~ValueAnimation();
		
		virtual bool BeginLoad(YumeFile& source);
		
		virtual bool Save(YumeFile& dest) const;
		
		bool LoadXML(const XmlNode& source);
		
		bool SaveXML(XmlNode& dest) const;
		
		
		void SetOwner(void* owner);
		
		void SetInterpolationMethod(InterpMethod method);
		
		void SetSplineTension(float tension);
		
		void SetValueType(VariantType valueType);

		
		bool SetKeyFrame(float time,const Variant& value);
		
		void SetEventFrame(float time,const YumeHash& eventType,const VariantMap& eventData = VariantMap());

		
		bool IsValid() const;

		
		void* GetOwner() const { return owner_; }

		
		InterpMethod GetInterpolationMethod() const { return interpolationMethod_; }

		
		float GetSplineTension() const { return splineTension_; }

		
		VariantType GetValueType() const { return valueType_; }

		
		float GetBeginTime() const { return beginTime_; }

		
		float GetEndTime() const { return endTime_; }

		
		Variant GetAnimationValue(float scaledTime);

		
		bool HasEventFrames() const { return !eventFrames_.empty(); }

		
		void GetEventFrames(float beginTime,float endTime,YumeVector<const VAnimEventFrame*>::type& eventFrames) const;

	protected:
		
		Variant LinearInterpolation(unsigned index1,unsigned index2,float scaledTime) const;
		
		Variant SplineInterpolation(unsigned index1,unsigned index2,float scaledTime);
		
		void UpdateSplineTangents();
		
		Variant SubstractAndMultiply(const Variant& value1,const Variant& value2,float t) const;

		
		void* owner_;
		
		InterpMethod interpolationMethod_;
		
		float splineTension_;
		
		VariantType valueType_;
		
		bool interpolatable_;
		
		float beginTime_;
		
		float endTime_;
		
		YumeVector<VAnimKeyFrame>::type keyFrames_;
		
		YumeVector<Variant>::type splineTangents_;
		
		bool splineTangentsDirty_;
		
		YumeVector<VAnimEventFrame>::type eventFrames_;
	};
}


//----------------------------------------------------------------------------
#endif

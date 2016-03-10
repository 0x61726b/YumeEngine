



























#include "YumeHeaders.h"
#include "YumeVector4.h"
#include "YumeMath.h"

namespace YumeEngine
{

	const Vector4 Vector4::ZERO;
	const Vector4 Vector4::ONE(1.0f,1.0f,1.0f,1.0f);

	YumeString Vector4::ToString() const
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer,"%g %g %g %g",x_,y_,z_,w_);
		return YumeString (tempBuffer);
	}

}

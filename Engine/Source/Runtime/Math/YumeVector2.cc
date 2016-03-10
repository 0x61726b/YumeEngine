



























#include "YumeHeaders.h"
#include "YumeVector2.h"
#include "YumeMath.h"

namespace YumeEngine
{

	const Vector2 Vector2::ZERO;
	const Vector2 Vector2::LEFT(-1.0f,0.0f);
	const Vector2 Vector2::RIGHT(1.0f,0.0f);
	const Vector2 Vector2::UP(0.0f,1.0f);
	const Vector2 Vector2::DOWN(0.0f,-1.0f);
	const Vector2 Vector2::ONE(1.0f,1.0f);

	const IntVector2 IntVector2::ZERO;

	YumeString Vector2::ToString() const
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer,"%g %g",x_,y_);
		return YumeString(tempBuffer);
	}

	YumeString IntVector2::ToString() const
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer,"%d %d",x_,y_);
		return YumeString(tempBuffer);
	}

}

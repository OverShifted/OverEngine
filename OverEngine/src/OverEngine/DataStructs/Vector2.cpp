#include "pcheader.h"

#include "Vector2.h"

namespace OverEngine
{
	void Vector2::Set(float _x, float _y)
	{
		this->x = _x;
		this->y = _y;
	}

	std::string Vector2::ToString() const
	{
		std::stringstream ss;//Vector2( x:5 , y:10 )
		ss << "Vector2( x:" << this->x << " , y:" << this->y << " )";
		return ss.str();
	}
}
#pragma once

#include <d2d1.h>

namespace sb {

class DpiScale {
	float mDpiScale;

public:
	DpiScale()
	    : mDpiScale(1.f) {}

	inline void SetDpi(float dpi) { mDpiScale = dpi / 96.f; }
	inline float PixelToDip(float pixel) const { return pixel / mDpiScale; }
	inline D2D1_POINT_2F PixelsToDips(int pixelX, int pixelY) const {
		return {PixelToDip((float)pixelX), PixelToDip((float)pixelY)};
	};
};

}  // namespace sb
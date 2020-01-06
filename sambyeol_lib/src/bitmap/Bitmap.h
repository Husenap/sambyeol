#pragma once

#include <functional>
#include <vector>

#include <atlbase.h>
#include <d2d1.h>

#include <glm/glm.hpp>

using BitmapPixel   = uint32_t;
using BitmapIndex   = uint32_t;
using BitmapProcess = std::function<glm::vec3(glm::vec2)>;

class Bitmap {
public:
	void Draw(CComPtr<ID2D1HwndRenderTarget> renderTarget, D2D1_RECT_F destRect);
	void Process(const BitmapProcess& pred);

	void Update();

	HRESULT Resize(CComPtr<ID2D1HwndRenderTarget> renderTarget, D2D1_SIZE_U size);

	inline D2D1_SIZE_U GetSize() const {
		if (mBitmap) {
			return mBitmap->GetPixelSize();
		}
		return {0, 0};
	}
	inline UINT32 GetWidth() const { return GetSize().width; }
	inline UINT32 GetHeight() const { return GetSize().height; }

	CComPtr<ID2D1Bitmap> mBitmap;
	std::vector<BitmapPixel> mData;
	std::vector<BitmapIndex> mIndices;
	std::vector<float> mDepthBuffer;

private:
};
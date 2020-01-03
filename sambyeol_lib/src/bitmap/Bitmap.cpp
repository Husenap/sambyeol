#include "Bitmap.h"

#include <execution>
#include <numeric>

void Bitmap::Draw(CComPtr<ID2D1HwndRenderTarget> renderTarget, D2D1_RECT_F destRect) {
	renderTarget->DrawBitmap(mBitmap,
	                         destRect,
	                         1.0f,
	                         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
	                         {0.f, 0.f, mBitmap->GetSize().width, mBitmap->GetSize().height});
}

void Bitmap::Process(const BitmapProcess& pred) {
	uint32_t w = GetWidth();
	uint32_t h = GetHeight();
	std::for_each(std::execution::par_unseq, mIndices.begin(), mIndices.end(), [&w, &h, &data = mData, &pred](BitmapIndex i) {
		const uint32_t x = i % w;
		const uint32_t y = i / w;

		const glm::vec3 c = glm::clamp(pred({(float)x / w, (float)y / h}), 0.f, 1.f)*255.f;

		data[i] = (uint32_t)(c.r) << 16 | (uint32_t)(c.g) << 8 |
		          (uint32_t)(c.b);
	});
	mBitmap->CopyFromMemory(NULL, mData.data(), mBitmap->GetPixelSize().width * sizeof(uint32_t));
}

HRESULT Bitmap::Resize(CComPtr<ID2D1HwndRenderTarget> renderTarget, D2D1_SIZE_U size) {
	mBitmap.Release();

	HRESULT hr = renderTarget->CreateBitmap(
	    size, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &mBitmap);

	mData.resize((std::size_t)mBitmap->GetPixelSize().width * mBitmap->GetPixelSize().height);
	mBitmap->CopyFromMemory(NULL, mData.data(), mBitmap->GetPixelSize().width * sizeof(uint32_t));

	mIndices.resize(mData.size());
	std::iota(mIndices.begin(), mIndices.end(), 0);

	return hr;
}

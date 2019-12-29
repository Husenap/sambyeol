#include "MainWindow.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <WindowsX.h>

#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

MainWindow::MainWindow()
    : mMousePoint(D2D1::Point2F())
    , mZoom(1.f)
    , mZoomMatrix(D2D1::Matrix3x2F::Scale(mZoom, mZoom)) {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory))) {
			return -1;
		}
		if (FAILED(DWriteCreateFactory(
		        DWRITE_FACTORY_TYPE_SHARED, __uuidof(mWriteFactory), reinterpret_cast<IUnknown**>(&mWriteFactory)))) {
			return -1;
		}
		if (FAILED(Initialize())) {
			return -1;
		}
		mIsOpen = true;
		break;
	case WM_DESTROY:
		mIsOpen = false;
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnResize();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp();
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel((GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA));
		break;
	default:
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

HRESULT MainWindow::Initialize() {
	HRESULT hr = S_OK;

	if (SUCCEEDED(hr)) {
		hr = mWriteFactory->CreateTextFormat(L"Roboto Mono",
		                                     NULL,
		                                     DWRITE_FONT_WEIGHT_NORMAL,
		                                     DWRITE_FONT_STYLE_NORMAL,
		                                     DWRITE_FONT_STRETCH_NORMAL,
		                                     mDpiScale.PixelToDip(72),
		                                     L"",
		                                     &mTextFormat);
	}
	if (SUCCEEDED(hr)) {
		mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		mTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}

	return hr;
}

HRESULT MainWindow::CreateGraphicsResources() {
	HRESULT hr = S_OK;

	if (mRenderTarget) {
		return hr;
	}

	if (!mRenderTarget) {
		hr = mFactory->CreateHwndRenderTarget(
		    D2D1::RenderTargetProperties(),
		    D2D1::HwndRenderTargetProperties(mHwnd, GetSize(), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		    &mRenderTarget);
	}
	if (SUCCEEDED(hr)) {
		hr = InitBitmap();
	}
	if (SUCCEEDED(hr)) {
		CalculateLayout();
	}

	return hr;
}

HRESULT MainWindow::InitBitmap() {
	if (mBitmap && mBitmap->GetPixelSize() == mRenderTarget->GetPixelSize()) {
		return S_OK;
	}
	if (mRenderTarget->GetPixelSize().width <= 0 || mRenderTarget->GetPixelSize().height <= 0) {
		return S_OK;
	}

	mBitmap.Release();

	std::cout << "new bitmap size: " << mRenderTarget->GetPixelSize().width << ", "
	          << mRenderTarget->GetPixelSize().height << std::endl;

	HRESULT hr = mRenderTarget->CreateBitmap(
	    mRenderTarget->GetPixelSize(),
	    D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
	    &mBitmap);

	mData.resize((std::size_t)mBitmap->GetPixelSize().width * mBitmap->GetPixelSize().height);
	std::iota(mData.begin(), mData.end(), 0xff0099ff);
	mBitmap->CopyFromMemory(NULL, mData.data(), mBitmap->GetPixelSize().width * sizeof(uint32_t));

	return hr;
}

void MainWindow::DiscardGraphicsResources() {
	mRenderTarget.Release();
	mBitmap.Release();
}

void MainWindow::CalculateLayout() {
	if (mRenderTarget) {
		InitBitmap();
	}
}

void MainWindow::OnPaint() {
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(mHwnd, &ps);
		mRenderTarget->BeginDraw();
		mRenderTarget->Clear(D2D1::ColorF(0x1e1e1e));

		mRenderTarget->SetTransform(mZoomMatrix);

		static int rrr = 0;
		std::iota(mData.begin(), mData.end(), 0xff0099ff + ++rrr);
		mBitmap->CopyFromMemory(NULL, mData.data(), mBitmap->GetPixelSize().width * sizeof(uint32_t));

		mRenderTarget->DrawBitmap(mBitmap,
		                          {0.f, 0.f, mRenderTarget->GetSize().width, mRenderTarget->GetSize().height},
		                          1.0f,
		                          D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		                          {0.f, 0.f, mBitmap->GetSize().width, mBitmap->GetSize().height});

		hr = mRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			DiscardGraphicsResources();
		}

		EndPaint(mHwnd, &ps);
	} else {
		DiscardGraphicsResources();
	}
}

void MainWindow::OnResize() {
	if (mRenderTarget) {
		mRenderTarget->Resize(GetSize());
		CalculateLayout();
		InvalidateRect(mHwnd, NULL, FALSE);
	}
}

void MainWindow::OnLButtonDown(int x, int y, DWORD flags) {
	D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);
	dips.x /= mZoom;
	dips.y /= mZoom;
}

void MainWindow::OnLButtonUp() {}

void MainWindow::OnMouseMove(int x, int y, DWORD flags) {
	D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);
	dips.x /= mZoom;
	dips.y /= mZoom;
}

void MainWindow::OnMouseWheel(float delta) {
	mZoom       = std::clamp(mZoom + delta * 0.1f, 0.1f, 2.f);
	mZoomMatrix = D2D1::Matrix3x2F::Scale(mZoom, mZoom);
}

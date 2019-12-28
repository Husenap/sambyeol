#include "MainWindow.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <WindowsX.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

#include "../resources/resource.h"

MainWindow::MainWindow()
    : mEllipse(D2D1::Ellipse({0.f, 0.f}, 0.f, 0.f))
    , mMousePoint(D2D1::Point2F())
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
		return 0;
	case WM_DESTROY:
		mIsOpen = false;
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		OnPaint();
		return 0;
	case WM_SIZE:
		OnResize();
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel((GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA));
		return 0;
	default:
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

void MainWindow::HandleCommand(WORD command) {
	switch (command) {
	case ID_DRAW_MODE:
		std::cout << "Draw mode!" << std::endl;
		break;
	case ID_SELECT_MODE:
		std::cout << "Select mode!" << std::endl;
		break;
	case ID_TOGGLE_MODE:
		std::cout << "Toggle mode!" << std::endl;
		break;
	}
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
		    D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(mHwnd, GetSize()), &mRenderTarget);
	}
	if (SUCCEEDED(hr)) {
		hr = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f), &mBrush);
	}
	if (SUCCEEDED(hr)) {
		hr = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.2f, 0.2f), &mStroke);
	}
	if (SUCCEEDED(hr)) {
		hr = mRenderTarget->CreateBitmap(
		    {100, 100},
		    D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
		    &mBitmap);
		std::vector<uint32_t> data;
		data.resize(mBitmap->GetPixelSize().width * mBitmap->GetPixelSize().height);
		std::fill(data.begin(), data.end(), 0xff0099ff);
		mBitmap->CopyFromMemory(NULL, data.data(), mBitmap->GetPixelSize().width * sizeof(uint32_t));
	}
	if (SUCCEEDED(hr)) {
		CalculateLayout();
	}

	return hr;
}

void MainWindow::DiscardGraphicsResources() {
	mRenderTarget.Release();
	mBrush.Release();
	mStroke.Release();
	mBitmap.Release();
}

void MainWindow::CalculateLayout() {
	if (mRenderTarget) {
		D2D1_SIZE_F size   = mRenderTarget->GetSize();
		const float x      = size.width / 2.f;
		const float y      = size.height / 2.f;
		const float radius = std::min(x, y);
		mEllipse           = D2D1::Ellipse({x, y}, radius, radius);
	}
}

void MainWindow::OnPaint() {
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(mHwnd, &ps);

		mRenderTarget->BeginDraw();

		mRenderTarget->SetTransform(mZoomMatrix);

		mRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		mRenderTarget->FillEllipse(mEllipse, mBrush);
		mRenderTarget->DrawEllipse(mEllipse, mStroke);

		SYSTEMTIME time;
		GetLocalTime(&time);

		const float hourAngle   = (360.f / 12.f) * time.wHour + time.wMinute * 0.5f;
		const float minuteAngle = (360.f / 60.f) * time.wMinute;
		const float secondAngle = (360.f / 60.f) * time.wSecond;

		DrawClockHand(0.6f, hourAngle, 6.f);
		DrawClockHand(0.85f, minuteAngle, 4.f);
		DrawClockHand(0.9f, secondAngle, 2.f);

		const static std::array<std::wstring, 12> numbers{
		    L"일", L"이", L"삼", L"사", L"오", L"육", L"칠", L"팔", L"구", L"십", L"십일", L"십이"};
		for (int i = 0; i < numbers.size(); ++i) {
			float angle = (360.f / 12.f) * 0.01745f * (float)(i - 2);
			DrawNumber(angle, numbers[i]);
		}

		mRenderTarget->SetTransform(mZoomMatrix);

		/*
		mRenderTarget->DrawBitmap(mBitmap,
		                          {0.f, 0.f, 1024.f, 1024.f},
		                          1.0f,
		                          D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		                          {0.f, 0.f, mBitmap->GetSize().width, mBitmap->GetSize().height});
		                          */

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
	SetCapture(mHwnd);

	D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);
	dips.x /= mZoom;
	dips.y /= mZoom;

	mMousePoint      = dips;
	mEllipse.point   = mMousePoint;
	mEllipse.radiusX = mEllipse.radiusY = 1.f;
}

void MainWindow::OnLButtonUp() {
	ReleaseCapture();
}

void MainWindow::OnMouseMove(int x, int y, DWORD flags) {
	if (flags & MK_LBUTTON) {
		D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);
		dips.x /= mZoom;
		dips.y /= mZoom;

		const float w  = (dips.x - mMousePoint.x) / 2.f;
		const float h  = (dips.y - mMousePoint.y) / 2.f;
		const float x1 = mMousePoint.x + w;
		const float y1 = mMousePoint.y + h;

		mEllipse = D2D1::Ellipse({x1, y1}, w, h);
	}
}

void MainWindow::OnMouseWheel(float delta) {
	mZoom       = std::clamp(mZoom + delta * 0.1f, 0.1f, 2.f);
	mZoomMatrix = D2D1::Matrix3x2F::Scale(mZoom, mZoom);
}

void MainWindow::DrawNumber(float angle, const std::wstring& number) {
	float dist    = std::min(mEllipse.radiusX, mEllipse.radiusY) * 0.9f;
	const float x = mEllipse.point.x + std::cosf(angle) * mEllipse.radiusX * 0.9f;
	const float y = mEllipse.point.y + std::sinf(angle) * mEllipse.radiusY * 0.9f;

	mRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(0.5f, 0.5f) * D2D1::Matrix3x2F::Translation(x, y) *
	                            mZoomMatrix);

	mRenderTarget->DrawText(number.data(), (UINT32)number.size(), mTextFormat, D2D1::RectF(), mStroke);
}

void MainWindow::DrawClockHand(float handLength, float angle, float strokeWidth) {
	mRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, mEllipse.point) * mZoomMatrix);

	D2D1_POINT_2F endPoint = mEllipse.point;
	endPoint.y -= std::min(mEllipse.radiusX, mEllipse.radiusY) * handLength;

	mRenderTarget->DrawLine(mEllipse.point, endPoint, mStroke, mDpiScale.PixelToDip(strokeWidth));
}

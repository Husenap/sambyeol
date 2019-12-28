#include "MainWindow.h"

#pragma comment(lib, "d2d1.lib")

#include <algorithm>

MainWindow::MainWindow()
    : mDPIScaleX(1.f)
    , mDPIScaleY(1.f)
    , mEllipse(D2D1::Ellipse({0.f, 0.f}, 0.f, 0.f)) {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory))) {
			return -1;
		}
		InitializeDPIScale();
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
	default:
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

void MainWindow::InitializeDPIScale() {
	mFactory->GetDesktopDpi(&mDPIScaleX, &mDPIScaleY);

	mDPIScaleX /= 96.0f;
	mDPIScaleY /= 96.0f;
}

HRESULT MainWindow::CreateGraphicsResources() {
	HRESULT hr = S_OK;
	if (!mRenderTarget) {
		hr = mFactory->CreateHwndRenderTarget(
		    D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(mHwnd, GetSize()), &mRenderTarget);
		if (SUCCEEDED(hr)) {
			hr = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f), &mBrush);
			hr = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.2f, 0.2f), &mStroke);

			if (SUCCEEDED(hr)) {
				CalculateLayout();
			}
		}
	}

	return hr;
}

void MainWindow::DiscardGraphicsResources() {
	mRenderTarget.Release();
	mBrush.Release();
	mStroke.Release();
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

		mRenderTarget->SetTransform(D2D1::IdentityMatrix());

		hr = mRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			DiscardGraphicsResources();
		}

		EndPaint(mHwnd, &ps);
	}
}

void MainWindow::OnResize() {
	if (mRenderTarget) {
		mRenderTarget->Resize(GetSize());
		CalculateLayout();
		InvalidateRect(mHwnd, NULL, FALSE);
	}
}

void MainWindow::DrawClockHand(float handLength, float angle, float strokeWidth) {
	mRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, mEllipse.point));

	D2D1_POINT_2F endPoint = mEllipse.point;
	endPoint.y -= mEllipse.radiusY * handLength;

	mRenderTarget->DrawLine(mEllipse.point, endPoint, mStroke, strokeWidth);
}

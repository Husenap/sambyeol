#include "MainWindow.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <algorithm>
#include <array>

MainWindow::MainWindow()
    : mEllipse(D2D1::Ellipse({0.f, 0.f}, 0.f, 0.f)) {}

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
		                                     72.f,
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

		mRenderTarget->SetTransform(D2D1::IdentityMatrix());

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
			float angle = (360.f / 12.f) * 0.01745f * (float)(i-2);
			DrawNumber(angle, numbers[i]);
		}

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

void MainWindow::DrawNumber(float angle, const std::wstring& number) {
	float dist    = mEllipse.radiusY * 0.9f;
	const float x = mEllipse.point.x + std::cosf(angle) * dist;
	const float y = mEllipse.point.y + std::sinf(angle) * dist;

	mRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(0.5f, 0.5f) * D2D1::Matrix3x2F::Translation(x, y));

	mRenderTarget->DrawText(number.data(), number.size(), mTextFormat, D2D1::RectF(), mStroke);
}

void MainWindow::DrawClockHand(float handLength, float angle, float strokeWidth) {
	mRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, mEllipse.point));

	D2D1_POINT_2F endPoint = mEllipse.point;
	endPoint.y -= mEllipse.radiusY * handLength;

	mRenderTarget->DrawLine(mEllipse.point, endPoint, mStroke, strokeWidth);
}

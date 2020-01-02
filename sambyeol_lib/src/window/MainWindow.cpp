#include "MainWindow.h"

#include <windowsx.h>

MainWindow::MainWindow()
    : mMousePoint(D2D1::Point2F())
    , mPS({}) {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory))) {
			return -1;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		OnResize();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		break;
	default:
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

bool MainWindow::BeginFrame() {
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr)) {
		BeginPaint(mHwnd, &mPS);
		mRenderTarget->BeginDraw();
		mRenderTarget->Clear(D2D1::ColorF(0x1e1e1e));
		return true;
	}

	DiscardGraphicsResources();
	return false;
}

void MainWindow::EndFrame() {
	HRESULT hr = mRenderTarget->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
		DiscardGraphicsResources();
	}

	EndPaint(mHwnd, &mPS);
}

void MainWindow::OnResize() {
	if (mRenderTarget) {
		mRenderTarget->Resize(GetSize());
		InitBitmap();
		InvalidateRect(mHwnd, NULL, FALSE);
	}
}

void MainWindow::OnLButtonDown(int x, int y, DWORD flags) {
	D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);

	mMousePoint = dips;
}

void MainWindow::OnMouseMove(int x, int y, DWORD flags) {
	D2D1_POINT_2F dips = mDpiScale.PixelsToDips(x, y);

	mMousePoint = dips;
}

HRESULT MainWindow::CreateGraphicsResources() {
	HRESULT hr = S_OK;

	if (mRenderTarget) {
		return hr;
	}

	hr = mFactory->CreateHwndRenderTarget(
	    D2D1::RenderTargetProperties(),
	    D2D1::HwndRenderTargetProperties(mHwnd, GetSize(), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
	    &mRenderTarget);
	if (SUCCEEDED(hr)) {
		hr = InitBitmap();
	}

	return hr;
}

HRESULT MainWindow::InitBitmap() {
	if (mRenderTarget->GetPixelSize().width <= 0 || mRenderTarget->GetPixelSize().height <= 0) {
		return S_OK;
	}
	if (mBitmap.GetSize() == mRenderTarget->GetPixelSize()) {
		return S_OK;
	}

	return mBitmap.Resize(mRenderTarget, mRenderTarget->GetPixelSize());
}

void MainWindow::DiscardGraphicsResources() {
	mRenderTarget.Release();
}

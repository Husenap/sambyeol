#pragma once

#include "BaseWindow.h"

#include <atlbase.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include <string>

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCWSTR ClassName() const override { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void OnPaint();

private:
	HRESULT Initialize();
	HRESULT CreateGraphicsResources();

	HRESULT InitBitmap();

	void DiscardGraphicsResources();

	void CalculateLayout();

	void OnResize();

	void OnLButtonDown(int x, int y, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int x, int y, DWORD flags);
	void OnMouseWheel(float delta);

	CComPtr<ID2D1Factory> mFactory;
	CComPtr<IDWriteFactory> mWriteFactory;

	CComPtr<IDWriteTextFormat> mTextFormat;
	CComPtr<ID2D1HwndRenderTarget> mRenderTarget;
	CComPtr<ID2D1Bitmap> mBitmap;
	std::vector<uint32_t> mData;

	D2D1_POINT_2F mMousePoint;

	float mZoom;
	D2D1_MATRIX_3X2_F mZoomMatrix;
};
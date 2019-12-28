#pragma once

#include "BaseWindow.h"

#include <atlbase.h>
#include <d2d1.h>
#include <dwrite.h>

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCWSTR ClassName() const override { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void HandleCommand(WORD command) override;

private:
	HRESULT Initialize();
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();

	void CalculateLayout();

	void DrawClockHand(float handLength, float angle, float strokeWidth);
	void DrawNumber(float angle, const std::wstring& number);

	void OnPaint();
	void OnResize();

	void OnLButtonDown(int x, int y, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int x, int y, DWORD flags);
	void OnMouseWheel(float delta);

	CComPtr<ID2D1Factory> mFactory;
	CComPtr<IDWriteFactory> mWriteFactory;

	CComPtr<ID2D1HwndRenderTarget> mRenderTarget;

	CComPtr<ID2D1SolidColorBrush> mBrush;
	CComPtr<ID2D1SolidColorBrush> mStroke;

	CComPtr<IDWriteTextFormat> mTextFormat;
	CComPtr<ID2D1Bitmap> mBitmap;
	D2D1_ELLIPSE mEllipse;

	D2D1_POINT_2F mMousePoint;

	float mZoom;
	D2D1_MATRIX_3X2_F mZoomMatrix;
};
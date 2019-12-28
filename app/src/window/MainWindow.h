#pragma once

#include "BaseWindow.h"

#include <atlbase.h>
#include <d2d1.h>

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCWSTR ClassName() const override { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void InitializeDPIScale();
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();

	void CalculateLayout();

	void OnPaint();
	void OnResize();

	float mDPIScaleX;
	float mDPIScaleY;

	CComPtr<ID2D1Factory> mFactory;
	CComPtr<ID2D1HwndRenderTarget> mRenderTarget;
	CComPtr<ID2D1SolidColorBrush> mBrush;
	CComPtr<ID2D1SolidColorBrush> mStroke;
	D2D1_ELLIPSE mEllipse;
	void DrawClockHand(float handLength, float angle, float strokeWidth);
};
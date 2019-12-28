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

private:
	HRESULT Initialize();
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();

	void CalculateLayout();

	void DrawClockHand(float handLength, float angle, float strokeWidth);
	void DrawNumber(float angle, const std::wstring& number);

	void OnPaint();
	void OnResize();

	CComPtr<ID2D1Factory> mFactory;
	CComPtr<ID2D1HwndRenderTarget> mRenderTarget;
	CComPtr<ID2D1SolidColorBrush> mBrush;
	CComPtr<ID2D1SolidColorBrush> mStroke;
	D2D1_ELLIPSE mEllipse;

	CComPtr<IDWriteFactory> mWriteFactory;
	CComPtr<IDWriteTextFormat> mTextFormat;
};
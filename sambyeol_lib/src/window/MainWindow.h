#pragma once

#include <vector>

#include <atlbase.h>
#include <d2d1.h>

#include "../bitmap/Bitmap.h"
#include "BaseWindow.h"

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCWSTR ClassName() const override { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
	bool BeginFrame();
	void EndFrame();

	CComPtr<ID2D1HwndRenderTarget> mRenderTarget;
	Bitmap mBitmap;

private:
	void OnResize();
	void OnLButtonDown(int x, int y, DWORD flags);
	void OnMouseMove(int x, int y, DWORD flags);

	HRESULT CreateGraphicsResources();
	HRESULT InitBitmap();
	void DiscardGraphicsResources();

	CComPtr<ID2D1Factory> mFactory;
	PAINTSTRUCT mPS;

	D2D1_POINT_2F mMousePoint;
};
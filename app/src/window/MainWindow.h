#pragma once

#include "BaseWindow.h"

#include <atlbase.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>

class MyEllipse {
	D2D1_ELLIPSE mEllipse;
	D2D1_COLOR_F mColor;

public:
	MyEllipse()
	    : mEllipse({{0.f, 0.f}, 1.f, 1.f})
	    , mColor(D2D1::ColorF(D2D1::ColorF::Red)) {}
	D2D1_POINT_2F GetPoint() const { return mEllipse.point; }
	void SetPoint(D2D1_POINT_2F pt) { mEllipse.point = pt; }
	float GetRadiusX() const { return mEllipse.radiusX; }
	void SetRadiusX(float r) { mEllipse.radiusX = r; }
	float GetRadiusY() const { return mEllipse.radiusY; }
	void SetRadiusY(float r) { mEllipse.radiusY = r; }

	void Draw(CComPtr<ID2D1HwndRenderTarget> renderTarget, CComPtr<ID2D1SolidColorBrush> brush) const;
	bool HitTest(float x, float y) const;
};

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCWSTR ClassName() const override { return L"Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void HandleCommand(WORD command) override;

private:
	enum class Mode { DrawMode, SelectMode, DragMode };

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

	CComPtr<IDWriteTextFormat> mTextFormat;
	CComPtr<ID2D1Bitmap> mBitmap;
	D2D1_ELLIPSE mEllipse;

	D2D1_POINT_2F mMousePoint;

	float mZoom;
	D2D1_MATRIX_3X2_F mZoomMatrix;

	HCURSOR mCursor;

	Mode mCurrentMode;
	std::vector<MyEllipse> mShapes;
	decltype(mShapes)::iterator mSelection;

	void SetMode(Mode mode);
	void InsertEllipse(D2D1_POINT_2F dips);
	void ClearSelection();
	bool HitTest(D2D1_POINT_2F dips);
	MyEllipse* Selection();
};
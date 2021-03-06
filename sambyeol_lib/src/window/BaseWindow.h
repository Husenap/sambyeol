#pragma once

#include <Windows.h>
#include <d2d1.h>

#include "DpiScale.h"

template <class DERIVED_TYPE>
class BaseWindow {
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DERIVED_TYPE* pThis = NULL;

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis                 = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->mHwnd = hwnd;

			pThis->mDpiScale.SetDpi((float)GetDpiForWindow(hwnd));
		} else {
			LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
			pThis        = reinterpret_cast<DERIVED_TYPE*>(ptr);
		}

		switch (uMsg) {
		case WM_DPICHANGED:
			pThis->mDpiScale.SetDpi((float)GetDpiForWindow(hwnd));
			break;
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	BaseWindow()
	    : mHwnd(NULL) {}
	virtual ~BaseWindow() {}

	BOOL Create(LPCWSTR lpWindowName,
	            DWORD dwStyle,
	            DWORD dwExStyle = NULL,
	            int x           = CW_USEDEFAULT,
	            int y           = CW_USEDEFAULT,
	            int w           = CW_USEDEFAULT,
	            int h           = CW_USEDEFAULT) {
		mTitle = lpWindowName;

		WNDCLASS wc = {};

		wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
		wc.hInstance     = NULL;
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);

		mHwnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle, x, y, w, h, NULL, NULL, NULL, this);

		if (mHwnd) {
			D2D1_SIZE_U size = GetSize();
			size             = {2 * w - size.width, 2 * h - size.height};
			MoveWindow(mHwnd, x, y, size.width, size.height, TRUE);
		}

		return mHwnd ? TRUE : FALSE;
	}

	HWND Window() const { return mHwnd; }
	inline RECT GetRect() const {
		RECT rc;
		GetClientRect(mHwnd, &rc);
		return rc;
	}
	inline D2D1_SIZE_U GetSize() const {
		RECT rc = GetRect();
		return D2D1::SizeU(rc.right, rc.bottom);
	}

	const std::wstring& GetTitle() const { return mTitle; }
	void SetTitle(const std::wstring& title) {
		mTitle = title;
		SetWindowTextW(mHwnd, mTitle.c_str());
	}

protected:
	virtual LPCWSTR ClassName() const                                      = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND mHwnd;
	sb::DpiScale mDpiScale;

	std::wstring mTitle;
};

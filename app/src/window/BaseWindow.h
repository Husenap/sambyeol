#pragma once

#include <windows.h>

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
			pThis->mIsOpen = true;
		} else {
			LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
			pThis        = reinterpret_cast<DERIVED_TYPE*>(ptr);
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	BaseWindow()
	    : mHwnd(NULL) {}

	BOOL Create(LPCWSTR lpWindowName,
	            DWORD dwStyle,
	            DWORD dwExStyle = NULL,
	            int x           = CW_USEDEFAULT,
	            int y           = CW_USEDEFAULT,
	            int w           = CW_USEDEFAULT,
	            int h           = CW_USEDEFAULT) {
		WNDCLASS wc = {};

		wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
		wc.hInstance     = NULL;
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);

		mHwnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle, x, y, w, h, NULL, NULL, NULL, this);

		return mHwnd ? TRUE : FALSE;
	}

	HWND Window() const { return mHwnd; }
	bool IsOpen() const { return mIsOpen; }

protected:
	virtual LPCWSTR ClassName() const                                       = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND mHwnd;
	bool mIsOpen;
};
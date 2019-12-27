#include "MainWindow.h"

MainWindow::MainWindow() {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		mIsOpen = false;
		PostQuitMessage(0);
		return 0;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(mHwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOWTEXT));
		EndPaint(mHwnd, &ps);
	}
		return 0;
	default:
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

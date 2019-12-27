#include "MainWindow.h"

MainWindow::MainWindow() {}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		std::cout << "resize event" << std::endl;
		return 0;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(mHwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOWTEXT));

		EndPaint(mHwnd, &ps);
		std::cout << "paint event" << std::endl;
		return 0;
	}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

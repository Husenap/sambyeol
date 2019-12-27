#include <iostream>
#include <thread>

#include <Windows.h>

#include "window/MainWindow.h"

int main() {
	MainWindow window;

	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
	if (FAILED(hr)) {
		std::cout << "Failed to initialize the COM library" << std::endl;
		return hr;
	}

	if (!window.Create("SamByeol", WS_OVERLAPPEDWINDOW, 0, 0, 0, 500, 500)) {
		std::cout << "Failed to create window!" << std::endl;
		return 1;
	}
	ShowWindow(window.Window(), SW_SHOW);

	MSG msg = {};

	while (window.IsOpen()) {
		while (PeekMessage(&msg, window.Window(), NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		std::this_thread::yield();
	}

	CoUninitialize();

	return 0;
}
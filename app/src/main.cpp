#include <iostream>
#include <thread>

#include <ShObjIdl.h>
#include <Windows.h>
#include <atlbase.h>

#include "sambyeol/SambyeolWindow.h"

int main() {
	SambyeolWindow window;

	if (!window.Create(L"삼별", WS_OVERLAPPEDWINDOW, 0, 0, 0, 800, 600)) {
		std::cout << "Failed to create window!" << std::endl;
		return 1;
	}
	ShowWindow(window.Window(), SW_SHOW);

	HRESULT hr;
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
	if (FAILED(hr)) {
		std::cout << "Failed to initialize the COM library" << std::endl;
		return hr;
	}

	bool isRunning = true;
	MSG msg        = {};
	while (isRunning) {
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				isRunning = false;
			}
		}

		window.OnPaint();
		std::this_thread::yield();
	}

	CoUninitialize();

	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	main();
}

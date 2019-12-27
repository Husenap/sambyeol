#include <iostream>
#include <thread>

#include <ShObjIdl.h>
#include <Windows.h>
#include <atlbase.h>

#include "window/MainWindow.h"

int main() {
	MainWindow window;

	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
	if (FAILED(hr)) {
		std::cout << "Failed to initialize the COM library" << std::endl;
		return hr;
	}

	if (!window.Create(L"삼별", WS_OVERLAPPEDWINDOW, 0, 0, 0, 500, 500)) {
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

		CComPtr<IFileOpenDialog> pFileOpen;
		hr = pFileOpen.CoCreateInstance(__uuidof(FileOpenDialog));

		if (SUCCEEDED(hr)) {
			CComQIPtr<IFileDialogCustomize> pFileDialogCustomize(pFileOpen);
			if (pFileDialogCustomize) {
				pFileDialogCustomize->AddText(10001, L"안녕하세요 여러분, 트와이스입니다!");
			}

			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr)) {
				CComPtr<IShellItem> pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {
						MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						CoTaskMemFree(pszFilePath);
					}
				}
			}
		}

		std::this_thread::yield();
	}

	CoUninitialize();

	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	main();
}

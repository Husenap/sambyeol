#pragma once

#include "BaseWindow.h"

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	LPCSTR ClassName() const override { return "Main Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
};
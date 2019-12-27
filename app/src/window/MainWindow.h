#pragma once

#include "BaseWindow.h"

class MainWindow : public BaseWindow<MainWindow> {
public:
	MainWindow();

	virtual LPCSTR ClassName() const override { return "Main Window Class"; }

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
};
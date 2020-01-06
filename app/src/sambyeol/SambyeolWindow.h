#pragma once

#include <chrono>

#include "window/MainWindow.h"
#include "util/FPSCounter.h"

class SambyeolWindow : public MainWindow {
public:
	SambyeolWindow();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void OnPaint();

private:
	FPSCounter mFPSCounter;

	std::chrono::steady_clock::time_point mStartTimePoint;
	std::chrono::steady_clock::time_point mCurrentTimePoint;
	float mTime;

	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<glm::vec3> mNormals;
};

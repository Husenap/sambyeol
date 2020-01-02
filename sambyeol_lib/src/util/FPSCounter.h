#pragma once

#include <vector>

class FPSCounter {
public:
	FPSCounter(int stackSize);

	void PushTime(float time);
	float GetFPS();

private:
	int mMaxSize;
	int mCurrentIndex;
	std::vector<float> mStack;
};

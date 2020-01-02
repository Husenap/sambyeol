#include "FPSCounter.h"

#include <numeric>

FPSCounter::FPSCounter(int stackSize)
    : mMaxSize(stackSize)
    , mCurrentIndex(0) {}

void FPSCounter::PushTime(float time) {
	if (time <= 1e-05) return;

	float fps = 1.f / time;

	if (mStack.size() < mMaxSize) {
		mStack.emplace_back(fps);
	} else {
		mStack[mCurrentIndex] = fps;
		mCurrentIndex         = (mCurrentIndex + 1) % mMaxSize;
	}
}

float FPSCounter::GetFPS() {
	return std::accumulate(mStack.begin(), mStack.end(), 0.f) / (float)(mStack.size());
}

#include "SambyeolWindow.h"

#include <string>

#include <glm/gtc/matrix_transform.hpp>

constexpr float Epsilon = 1e-8f;

SambyeolWindow::SambyeolWindow()
    : mFPSCounter(60)
    , mStartTimePoint(std::chrono::steady_clock::now())
    , mCurrentTimePoint(mStartTimePoint)
    , mTime(0.f) {}

LRESULT SambyeolWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		OnPaint();
		break;
	default:
		return MainWindow::HandleMessage(uMsg, wParam, lParam);
	}

	return TRUE;
}

bool RayTriangleIntersect(const glm::vec3& ro, const glm::vec3& rd, const glm::vec3 v[3], float& t, glm::vec2& st) {
	const glm::vec3 v0v1 = v[1] - v[0];
	const glm::vec3 v0v2 = v[2] - v[0];
	const glm::vec3 pvec = glm::cross(rd, v0v2);

	const float det = glm::dot(v0v1, pvec);

	if (std::fabs(det) < Epsilon) {
		return false;
	}

	const float invDet = 1.f / det;

	const glm::vec3 tvec = ro - v[0];
	st.s                 = glm::dot(tvec, pvec) * invDet;
	if (st.s < 0.f || st.s > 1.0f) {
		return false;
	}

	const glm::vec3 qvec = glm::cross(tvec, v0v1);
	st.t                 = glm::dot(rd, qvec) * invDet;
	if (st.t < 0.f || st.s + st.t > 1.f) {
		return false;
	}

	t = glm::dot(v0v2, qvec) * invDet;

	return true;
}

void SambyeolWindow::OnPaint() {
	auto now = std::chrono::steady_clock::now();
	float dt = float(std::chrono::duration_cast<std::chrono::nanoseconds>(now - mCurrentTimePoint).count()) / 1e9f;
	mCurrentTimePoint = now;
	mTime += dt;
	mFPSCounter.PushTime(dt);

	SetTitle(L"삼별 FPS: " + std::to_wstring(mFPSCounter.GetFPS()));

	const glm::mat4 rot = glm::rotate(glm::mat4(1.0f), mTime, glm::vec3(0.f, 1.f, 0.f));
	// clang-format off
	const glm::vec3 vertices[3] = {
		rot*glm::vec4(-0.5f, +0.5f, 0.f, 1.f),
		rot*glm::vec4(+0.5f, +0.5f, 0.f, 1.f),
		rot*glm::vec4(+0.0f, -0.5f, 0.f, 1.f)
	};
	const glm::vec3 vertices1[3] = {
		rot*glm::vec4(+0.5f, +0.5f, 0.f, 1.f),
		rot*glm::vec4(+0.9f, -0.3f, 0.f, 1.f),
		rot*glm::vec4(+0.0f, -0.5f, 0.f, 1.f)
	};
	// clang-format on

	if (BeginFrame()) {
		const auto pred = [& t = mTime, &vertices, &vertices1](glm::vec2 uv) {
			float d = glm::length(uv - 0.5f) * 2.f;

			const glm::vec3 ro(std::cosf(t), 0.f, -2.f + std::sinf(t));
			const glm::vec3 rd = glm::normalize(glm::vec3((uv - 0.5f) * 2.f, -1.f));
			float t;
			glm::vec2 st;
			if (RayTriangleIntersect(ro, rd, vertices, t, st)) {
				return glm::vec3(st, 1.f - st.s - st.t);
			}
			if (RayTriangleIntersect(ro, rd, vertices1, t, st)) {
				return glm::vec3(st, 1.f - st.s - st.t);
			}

			return glm::vec3(d);
		};
		mBitmap.Process(pred);
		mBitmap.Draw(mRenderTarget, {0.f, 0.f, mRenderTarget->GetSize().width, mRenderTarget->GetSize().height});

		EndFrame();
	} else {
	}
}

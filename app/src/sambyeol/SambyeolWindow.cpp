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

	if (det < Epsilon) {
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
	if (st.s < 0.f || st.t < 0.f || st.s + st.t > 1.f) {
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

	if (BeginFrame()) {
		float fov              = 70.f;
		float perspectiveScale = std::tanf((3.1415f / 360.f) * fov);
		float aspectRatio      = (float)mBitmap.GetWidth() / mBitmap.GetHeight();

		// clang-format off
		const glm::mat4 rot = glm::rotate(glm::mat4(
			{1.f, 0.f, 0.f, 0.f},
			{0.f, 1.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{0.f, 0.f, 1.f+std::cosf(mTime), 1.f}
		), mTime*1.5f, glm::vec3(0.f, 1.f, 1.f));
		const glm::mat4 scale = glm::mat4(
			{1.f, 0.f, 0.f, 0.f},
			{0.f, 2.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{0.f, 0.f, 0.f, 1.f}
		);
		const glm::vec3 vertices[8] = {
			rot*scale*glm::vec4(-1.0f, +1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, +1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, -1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, -1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, +1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, +1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, -1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, -1.0f, +1.f, 1.f)
		};
		const glm::vec3 vertexColor[8] = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.5f},
			{0.5f, 1.0f, 0.0f},
		};
		const unsigned int indices[36] = {
			// FRONT FACE
			0, 1, 2,
			1, 3, 2,
			// BACK FACE
			4, 6, 5,
			6, 7, 5, 
			// LEFT FACE
			4, 0, 6,
			0, 2, 6,
			// RIGHT FACE
			1, 5, 3,
			5, 7, 3,
			// TOP FACE
			4, 5, 0,
			5, 1, 0,
			// BOTTOM FACE
			2, 3, 6,
			3, 7, 6
		};
		const glm::vec3 ro(0.f, 0.f, -4.f);
		// clang-format on

		const auto pred =
		    [& time = mTime, &ro, &aspectRatio, &perspectiveScale, &vertices, &indices, &vertexColor](glm::vec2 uv) {
			    glm::vec3 rd = glm::vec3((uv - 0.5f) * 2.f * perspectiveScale, 1.f);
			    rd.x *= aspectRatio;
			    rd = glm::normalize(rd);

			    float t;
			    glm::vec2 st;
			    float tMin = 1000.f;
			    glm::vec3 col;
			    for (int i = 0; i < 36; i += 3) {
				    glm::vec3 verts[3] = {vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]};
				    if (RayTriangleIntersect(ro, rd, verts, t, st) && t < tMin) {
					    tMin = t;
					    col  = (1.f - st.s - st.t) * vertexColor[indices[i]] + st.s * vertexColor[indices[i + 1]] +
					          st.t * vertexColor[indices[i + 2]];
				    }
			    }

			    if (tMin < 1000.f) {
				    return glm::pow(col, glm::vec3(1.f / 1.8f));
			    }
			    return glm::vec3(0.f);
		    };
		mBitmap.Process(pred);
		mBitmap.Draw(mRenderTarget, {0.f, 0.f, mRenderTarget->GetSize().width, mRenderTarget->GetSize().height});

		EndFrame();
	} else {
	}
}

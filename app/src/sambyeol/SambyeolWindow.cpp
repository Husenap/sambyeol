#include "SambyeolWindow.h"

#include <string>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr float Epsilon = 1e-8f;

SambyeolWindow::SambyeolWindow()
    : mFPSCounter(60)
    , mStartTimePoint(std::chrono::steady_clock::now())
    , mCurrentTimePoint(mStartTimePoint)
    , mTime(0.f) {
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile("monkey.fbx", aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene) {
		std::cout << "Failed to load model file" << std::endl;
		std::cout << "Error: " << imp.GetErrorString() << std::endl;
	} else {
		if (scene->HasMeshes()) {
			for (int i = 0; i < scene->mNumMeshes && i < 1; ++i) {
				auto mesh = scene->mMeshes[i];
				mVertices.resize(mesh->mNumVertices);
				mNormals.resize(mesh->mNumVertices);
				for (int v = 0; v < mesh->mNumVertices; ++v) {
					mVertices[v] = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
					mNormals[v] = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
				}
				mIndices.resize(mesh->mNumFaces*3);
				for (int f = 0; f < mesh->mNumFaces; ++f) {
					mIndices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
					mIndices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
					mIndices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
				}
			}
		}
	}
}

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

bool RayTriangleIntersect(
    const glm::vec3& ro, const glm::vec3& rd, const glm::vec3 v[3], float& t, glm::vec2& st, glm::vec3& normal) {
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

	normal = glm::normalize(glm::cross(v0v1, v0v2));

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
		), mTime*1.5f, glm::vec3(std::cosf(mTime*0.9f+2.f), 1.f, std::sinf(mTime*0.95f)));
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
		const glm::vec3 ro(0.f, 0.f, -3.f);
		const static glm::vec3 lightDir = glm::normalize(glm::vec3(1.f, 1.f, -1.f));
		// clang-format on

		const auto pred =
		    [& time = mTime, &ro, &aspectRatio, &perspectiveScale, &vertices = mVertices, &indices = mIndices, &vertexColor = mNormals](glm::vec2 uv) {
			    glm::vec3 rd = glm::vec3((uv - 0.5f) * 2.f * perspectiveScale, 1.f);
			    rd.x *= aspectRatio;
			    rd = glm::normalize(rd);

			    float t;
			    glm::vec2 st;
			    float tMin = 1000.f;
			    glm::vec3 normal;
			    glm::vec3 col;
			    for (int i = 0; i < indices.size(); i += 3) {
				    glm::vec3 verts[3] = {vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]};
				    if (RayTriangleIntersect(ro, rd, verts, t, st, normal) && t < tMin) {
					    tMin = t;
					    col  = (1.f - st.s - st.t) * vertexColor[indices[i]] + st.s * vertexColor[indices[i + 1]] +
					          st.t * vertexColor[indices[i + 2]];

					    float attenuation = glm::max(glm::dot(normal, lightDir), 0.05f);

					    col = glm::vec3(attenuation);
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

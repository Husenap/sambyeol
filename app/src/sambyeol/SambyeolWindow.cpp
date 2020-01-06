#include "SambyeolWindow.h"

#include <string>
#include <execution>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

constexpr float Epsilon = 1e-8f;

SambyeolWindow::SambyeolWindow()
    : mFPSCounter(60)
    , mStartTimePoint(std::chrono::steady_clock::now())
    , mCurrentTimePoint(mStartTimePoint)
    , mTime(0.f) {
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile("monkey.fbx", aiProcessPreset_TargetRealtime_Fast);

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

glm::vec3 MultMatrixVector(const glm::mat4& m, const glm::vec3& v) {
	glm::vec4 p = m * glm::vec4(v, 1.f);
	return glm::vec3(p) / p.w;
}

glm::vec3 ConvertToRaster(const glm::vec3& vertexWorld,
	const glm::mat4& worldToCamera,
	const glm::vec4& rect,
	const float nearPlane,
	const uint32_t imageWidth,
	const uint32_t imageHeight) {
	glm::vec3 vertexCamera = MultMatrixVector(worldToCamera, vertexWorld);

	glm::vec2 vertexScreen = nearPlane * vertexCamera / -vertexCamera.z;
	glm::vec2 vertexNDC    = {
        (2.f * vertexScreen.x - (rect.z + rect.x)) / (rect.z - rect.x),
        (2.f * vertexScreen.y - (rect.w + rect.y)) / (rect.w - rect.y),
    };


	return {
		(vertexNDC.x + 1.f) / 2.f * imageWidth,
		(1.f - vertexNDC.y) / 2.f * imageHeight,
		-vertexCamera.z
	};
}

float EdgeFunction(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
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
		const std::vector<glm::vec3> vertices = {
			rot*scale*glm::vec4(-1.0f, +1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, +1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, -1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, -1.0f, -1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, +1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, +1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(-1.0f, -1.0f, +1.f, 1.f),
			rot*scale*glm::vec4(+1.0f, -1.0f, +1.f, 1.f)
		};
		const std::vector<glm::vec3> vertexColor = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.5f},
			{0.5f, 1.0f, 0.0f},
		};
		const std::vector<unsigned int> indices = {
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

		float farPlane  = 100.f;
		float nearPlane = 1.f;
		float fc = std::cosf(mTime);
		float fs = std::sinf(mTime);
		const glm::mat4 worldToCamera = glm::mat4(
			{1.f, 0.f, 0.f, 0.f},
			{0.f, 1.f, 0.f, 0.f},
			{0.f, 0.f, 1.f, 0.f},
			{fs*3.f, 0.f,std::cosf(mTime*1.7f)*1.f-3.f, 1.f}
		);
		const glm::mat4 cameraToWorld = glm::inverse(worldToCamera);

		// clang-format on

		std::fill(std::execution::par_unseq, mBitmap.mData.begin(), mBitmap.mData.end(), 0);
		std::fill(std::execution::par_unseq, mBitmap.mDepthBuffer.begin(), mBitmap.mDepthBuffer.end(), farPlane);

		uint32_t w = mBitmap.GetWidth();
		uint32_t h = mBitmap.GetHeight();

		std::for_each(
		    std::execution::par,
		    mBitmap.mIndices.begin(),
		    mBitmap.mIndices.begin() + mIndices.size()/3,
		    [&](uint32_t i) {
			    // for (std::size_t i = 0; i < mIndices.size(); i += 3) {
			    const glm::vec3& v0 = mVertices[mIndices[i*3 + 0]];
			    const glm::vec3& v1 = mVertices[mIndices[i*3 + 1]];
			    const glm::vec3& v2 = mVertices[mIndices[i*3 + 2]];

			    glm::vec3 v0Raster =
			        ConvertToRaster(v0, worldToCamera, {-aspectRatio, -1.f, aspectRatio, 1.f}, nearPlane, w, h);
			    glm::vec3 v1Raster =
			        ConvertToRaster(v1, worldToCamera, {-aspectRatio, -1.f, aspectRatio, 1.f}, nearPlane, w, h);
			    glm::vec3 v2Raster =
			        ConvertToRaster(v2, worldToCamera, {-aspectRatio, -1.f, aspectRatio, 1.f}, nearPlane, w, h);

			    v0Raster.z = 1.f / v0Raster.z;
			    v1Raster.z = 1.f / v1Raster.z;
			    v2Raster.z = 1.f / v2Raster.z;

			    float xmin = glm::compMin(glm::vec3(v0Raster.x, v1Raster.x, v2Raster.x));
			    float ymin = glm::compMin(glm::vec3(v0Raster.y, v1Raster.y, v2Raster.y));
			    float xmax = glm::compMax(glm::vec3(v0Raster.x, v1Raster.x, v2Raster.x));
			    float ymax = glm::compMax(glm::vec3(v0Raster.y, v1Raster.y, v2Raster.y));

			    if (xmin > w - 1 || xmax < 0 || ymin > h - 1 || ymax < 0) return;

				glm::vec3 n0 = mNormals[mIndices[i * 3 + 0]];
				glm::vec3 n1 = mNormals[mIndices[i * 3 + 1]];
				glm::vec3 n2 = mNormals[mIndices[i * 3 + 2]];

			    uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin)));
			    uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin)));
			    uint32_t x1 = std::min(int32_t(w) - 1, (int32_t)(std::floor(xmax)));
			    uint32_t y1 = std::min(int32_t(h) - 1, (int32_t)(std::floor(ymax)));

			    float area = EdgeFunction(v0Raster, v1Raster, v2Raster);

			    std::for_each(std::execution::seq,
			                  mBitmap.mIndices.begin() + y0,
			                  mBitmap.mIndices.begin() + y1 + 1,
			                  [&](uint32_t y) {
				                  // for (uint32_t y = y0; y <= y1; ++y) {
				                  for (uint32_t x = x0; x <= x1; ++x) {
					                  glm::vec3 pixelSample(x + 0.5f, y + 0.5f, 0.f);
					                  float w0 = EdgeFunction(v1Raster, v2Raster, pixelSample);
					                  float w1 = EdgeFunction(v2Raster, v0Raster, pixelSample);
					                  float w2 = EdgeFunction(v0Raster, v1Raster, pixelSample);
					                  if (w0 >= 0.f && w1 >= 0.f && w2 >= 0.f) {
						                  w0 /= area;
						                  w1 /= area;
						                  w2 /= area;
						                  float z = 1.f / (v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2);

						                  if (z < mBitmap.mDepthBuffer[y * w + x]) {
							                  mBitmap.mDepthBuffer[y * w + x] = z;

							                  glm::vec3 n = glm::normalize(w0 * n0 + w1 * n1 + w2 * n2);
							                  float NdotV = std::max(0.f, glm::dot(n, -lightDir));

							                  const glm::vec3 c = glm::vec3(255.0f * NdotV);
							                  mBitmap.mData[y * w + x] =
							                      (uint32_t)(c.r) << 16 | (uint32_t)(c.g) << 8 | (uint32_t)(c.b);
						                  }
					                  }
				                  }
				                  //}
			                  });
			    //}
		    });

		mBitmap.Update();
		mBitmap.Draw(mRenderTarget, {0.f, 0.f, mRenderTarget->GetSize().width, mRenderTarget->GetSize().height});

		EndFrame();
	} else {
	}
}

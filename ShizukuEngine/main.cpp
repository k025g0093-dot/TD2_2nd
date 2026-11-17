#include "ShizukuEngine/ShizukuEngine.h"
#include "Game/SceneManeger.h"
#include <Windows.h>

const int32_t kClineWidth = 1280;
const int32_t kClineHeight = 720;

enum class Scene { kUnknown = 0, kTitle, kGame, kGameClear, kGameOver };

Scene scene = Scene::kTitle;
ShizukuEngine* engine = nullptr;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	engine = new ShizukuEngine(kClineWidth, kClineHeight, L"ShizukuEngine");

	// シーンマネージャー
	SceneManager sceneManager;
	sceneManager.Initialize(engine);

	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {

			engine->Update();

			sceneManager.ChangeScene();
			sceneManager.UpdateScene();


			engine->PreDraw();

			sceneManager.DrawScene();

			engine->PostDraw();

			if (engine->GetInput().GetKeyDown(VK_ESCAPE)) {
				break;
			}
		}
	}

	delete engine;

	return 0;
}
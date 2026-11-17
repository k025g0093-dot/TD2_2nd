#include "SceneManeger.h"

#include "TitleScene.h"
#include "GameScene.h"

// 初期化
void SceneManager::Initialize(ShizukuEngine* engine) {

	this->engine = engine;

	scene = Scene::kTitle;

	title = new TitleScene();
	title->Initialize(engine);
}

// シーン切り替え
void SceneManager::ChangeScene() {

	switch (scene) {

	case Scene::kTitle:

		if (title->IsFinished()) {

			scene = Scene::kGame;

			delete title;
			title = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize(engine);
		}

		break;

	case Scene::kGame:

		if (gameScene->IsFinished()) {

			scene = Scene::kTitle;

			delete gameScene;
			gameScene = nullptr;

			title = new TitleScene();
			title->Initialize(engine);
		}

		break;

	default:
		break;
	}
}

// ゲームの更新処理
void SceneManager::UpdateScene() {

	switch (scene) {

	case Scene::kTitle:

		title->Update(1.0f / 60.0f);

		break;

	case Scene::kGame:

		gameScene->Update(1.0f / 60.0f);

		break;

	default:
		break;
	}
}

// ゲームの描画処理
void SceneManager::DrawScene() {

	switch (scene) {

	case Scene::kTitle:

		title->Draw();

		break;

	case Scene::kGame:

		gameScene->Draw();

		break;

	default:
		break;
	}
}

SceneManager::~SceneManager() {

	delete title;
	title = nullptr;

	delete gameScene;
	gameScene = nullptr;
}
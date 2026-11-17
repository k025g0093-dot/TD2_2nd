#pragma once

#include "IScene.h"

class ShizukuEngine;
class TitleScene;
class GameScene;

class SceneManager {

public:

	enum class Scene {
		kUnknown = 0,
		kTitle,
		kGame,
		kGameClear,
		kGameOver
	};

public:

	void Initialize(ShizukuEngine* engine);

	void ChangeScene();
	void UpdateScene();
	void DrawScene();

	~SceneManager();

private:

	ShizukuEngine* engine = nullptr;
	TitleScene* title = nullptr;
	GameScene* gameScene = nullptr;

	Scene scene = Scene::kTitle;

};
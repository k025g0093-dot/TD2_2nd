#pragma once
#include "IScene.h"
#include "../ShizukuEngine/ShizukuEngine.h"
#include "Fade.h"
#include "Player/Player.h"
#include "Player/PlayerInputMapper.h"
#include "Enemy/Enemy.h"
#include "Enemy/Boss.h"
#include "FollowCamera.h"

#include <vector>

class GameScene : public IScene
{

public:

	void Initialize(ShizukuEngine* engine)override;
	void Update(float dt)override;
	void Draw()override;

	bool IsFinished()const { return isFinished; }

private:
	void Run(float dt);

	// ザコ敵の出現
	void SpawnEnemy();

	void AllCollision();

	void BossToPlayerCollision();

	void EnemyToEnemyCollision();

	void EnemyToPlayer();

private:

	ShizukuEngine* mEngine = nullptr;
	
	Phese mPhese = Phese::kFadeIn;

	Fade* mFade = nullptr;

	Player mPlayer;
	PlayerInputMapper mInputMapper;

	FollowCamera mFollowCamera;

	Boss mBoss;
	std::vector<Enemy> mEnemies;

	float mSpawnTimer = 0.0f;
	float mSpawnInterval = 3.0f;
	int mSpawnCount = 0;
	static constexpr int kMaxEnemies = 20;

	//素材（モデルど）int型で指定

	int field = 0;
	int player = 0;
	int dancce = 0;

	int enemy = 0;
	int enemyAnimation = 0;

	int bossModel = 0;
	int bossAnimation = 0;

	//ゲームの状態
	bool isFinished = false;

};


#include "GameScene.h"

#include <cstdio>
#include <Windows.h>
#include <algorithm>

void GameScene::Initialize(ShizukuEngine* engine) {
	mEngine = engine;

	mFade = new Fade();
	mFade->Initialize(mEngine);
	mFade->Start(Fade::Status::FadeIn, 1.0f);

	//モデル読み込み
	field = mEngine->LoadModel("resources/field/field.obj");
	player = mEngine->LoadSkinnedModel("resources/Flair/Flair.dae");
	dancce= mEngine->LoadAnimation("resources/Flair/Flair.dae", player);
	dancce = mEngine->CreateAnimator(dancce);

	mPlayer.Initialize(player, dancce,Vector3{ 0.0f,0.0f,0.0f });
	mFollowCamera.Initialize(mPlayer.GetPosition());

	// 敵
	enemy = mEngine->LoadModel("resources/Flair/Flair.dae");
	enemyAnimation = mEngine->LoadAnimation("resources/Flair/Flair.dae", enemy);
	enemyAnimation = mEngine->CreateAnimator(enemy);

	// エリート敵
	eliteModel = mEngine->LoadSkinnedModel("resources/Flair/Flair.dae");
	eliteAnimation = mEngine->LoadAnimation("resources/Flair/Flair.dae", eliteModel);
	eliteAnimation = mEngine->CreateAnimator(eliteModel);

	// ボス
	bossModel = mEngine->LoadModel("resources/Flair/Flair.dae");
	bossAnimation = mEngine->LoadAnimation("resources/Flair/Flair.dae", bossModel);
	bossAnimation = mEngine->CreateAnimator(bossModel);

	mBoss.Initialize(bossModel, bossAnimation, { 5.0f, 0.0f, 5.0f });

	// ザコ敵の初期化
	mEnemies.clear();

	mSpawnTimer = 0.0f;
	mSpawnCount = 0;
}

void GameScene::Update(float dt) {

	mFade->Update();

	switch (mPhese)
	{
	case Phese::kFadeIn:

		if (mFade->IsFinished()) {
			mPhese = Phese::kMain;
		}

		break;
	case Phese::kMain:

		Run(dt);

		if (mEngine->GetInput().GetKeyDown(VK_SPACE)) {
			mFade->Start(Fade::Status::FadeOut, 1.0f);
			mPhese = Phese::kFadeOut;
		}

		break;
	case Phese::kFadeOut:

		if (mFade->IsFinished()) {
			isFinished = true;
		}

		break;
	default:
		break;
	}

}

void GameScene::Draw() {

	mFade->Draw();

	mPlayer.Draw(mEngine);

	mBoss.Draw(mEngine);

	for (Enemy& enemy : mEnemies) {
		enemy.Draw(mEngine);
	}

	mEngine->DrawModel(
		field,
		{ 0,0,0 },
		{ 0,0,0 },
		{ 1,1,1 },
		{ 1,1,1,1 }
	);

}

void GameScene::Run(float dt) {

	PlayerCommand cmd = mInputMapper.Make(mEngine->GetInput(),dt);

	mEngine->GetCamera().SetCameraRot(
		mFollowCamera.UpdateRotate(cmd.aim)
	);

	mPlayer.Update(cmd, mFollowCamera.GetYaw(), dt);

	mEngine->GetCamera().SetCameraPos(
		mFollowCamera.UpdatePos(mPlayer.GetPosition())
	);

	mBoss.Update(mPlayer.GetPosition(), dt);

	// エリート召喚
	if (mBoss.ConsumeSummonRequest()) {
		SpawnEliteEnemy();
	}

	// ザコ敵・エリートの更新
	for (Enemy& enemy : mEnemies) {
		enemy.Update(mPlayer.GetPosition(), dt);
	}

	// ザコ敵出現タイマー
	mSpawnTimer += dt;

	if (mSpawnTimer >= mSpawnInterval) {

		mSpawnTimer = 0.0f;

		if (mEnemies.size() < kMaxEnemies) {
			SpawnEnemy();
		}
	}

	AllCollision();
}

void GameScene::SpawnEnemy() {

	Vector3 spawnPosition{};

	// フィールドの四隅付近から順番に出現
	switch (mSpawnCount % 4) {

	case 0:
		spawnPosition = { 50.0f, 0.0f, 50.0f };
		break;

	case 1:
		spawnPosition = { -50.0f, 0.0f, 50.0f };
		break;

	case 2:
		spawnPosition = { 50.0f, 0.0f, -50.0f };
		break;

	case 3:
		spawnPosition = { -50.0f, 0.0f, -50.0f };
		break;
	}

	// 敵の種類
	Enemy::Type type = Enemy::Type::kChase;

	// 5体に1体を遠距離型にする
	if (mSpawnCount % 5 == 1) {
		type = Enemy::Type::kRanged;
	}

	// ザコ敵生成
	Enemy newEnemy;

	newEnemy.Initialize(
		enemy,
		enemyAnimation,
		spawnPosition,
		type
	);

	mEnemies.push_back(newEnemy);

	mSpawnCount++;
}

void GameScene::SpawnEliteEnemy() {

	// ボスの周囲に4体召喚する
	const int summonCount = 4;

	// ボスからの出現距離
	const float spawnDistance = 6.0f;

	for (int i = 0; i < summonCount; i++) {

		Vector3 spawnPosition = mBoss.GetPosition();

		// ボスの周囲4方向に出現
		switch (i) {

		case 0:
			spawnPosition.x += spawnDistance;
			break;

		case 1:
			spawnPosition.x -= spawnDistance;
			break;

		case 2:
			spawnPosition.z += spawnDistance;
			break;

		case 3:
			spawnPosition.z -= spawnDistance;
			break;
		}

		// フィールド外に出ないようにする
		spawnPosition.x = std::clamp(
			spawnPosition.x, -95.0f, 95.0f
		);

		spawnPosition.z = std::clamp(
			spawnPosition.z, -95.0f, 95.0f
		);

		spawnPosition.y = 0.0f;

		// エリート敵生成
		Enemy elite;

		elite.Initialize(
			eliteModel,
			eliteAnimation,
			spawnPosition,
			Enemy::Type::kElite
		);

		mEnemies.push_back(elite);
	}
}

void GameScene::AllCollision() {
	EnemyToPlayer();
	BossToPlayerCollision();
	EnemyToEnemyCollision();
}

void GameScene::EnemyToPlayer() {

	for (Enemy& enemy : mEnemies) {

		OBB enemyObb{};
		enemyObb.center = enemy.GetPosition();
		enemyObb.orientations[0] = { 1,0,0 };
		enemyObb.orientations[1] = { 0,1,1 };
		enemyObb.orientations[2] = { 0,0,1 };
		enemyObb.size = { 1.0f,1.0f,1.0f };

		std::vector<OBB>playerObb{};
		playerObb = mEngine->CreateAnimationModelOBBsCollision(
			player, dancce,
			mPlayer.GetPosition(), { 0,mPlayer.GetRotationY(),0 }, mPlayer.GetScale()
		);

		if (mEngine->IsAnimationOBBToOBB(playerObb, enemyObb)) {
			/*TODO　ここに当たり判定の内容を記述*/
		}
	}
}


void GameScene::BossToPlayerCollision() {

	std::vector<OBB>playerObb{};
	playerObb = mEngine->CreateAnimationModelOBBsCollision(
		player, dancce,
		mPlayer.GetPosition(), { 0,mPlayer.GetRotationY(),0 }, mPlayer.GetScale()
	);

	OBB bossObb{};
	bossObb.center = mBoss.GetPosition();
	bossObb.orientations[0] = { 1,0,0 };
	bossObb.orientations[1] = { 0,1,1 };
	bossObb.orientations[2] = { 0,0,1 };
	bossObb.size = mBoss.GetScale();

	if (mEngine->IsAnimationOBBToOBB(playerObb, bossObb)) {
		/*TODO　ここに当たり判定の内容を記述*/
	}

}


void GameScene::EnemyToEnemyCollision() {

	for (Enemy& enemy : mEnemies) {

		OBB enemyObb1{};
		enemyObb1.center = enemy.GetPosition();
		enemyObb1.orientations[0] = { 1,0,0 };
		enemyObb1.orientations[1] = { 0,1,1 };
		enemyObb1.orientations[2] = { 0,0,1 };
		enemyObb1.size = { 1.0f,1.0f,1.0f };

		OBB enemyObb2{};
		enemyObb2.center = enemy.GetPosition();
		enemyObb2.orientations[0] = { 1,0,0 };
		enemyObb2.orientations[1] = { 0,1,1 };
		enemyObb2.orientations[2] = { 0,0,1 };
		enemyObb2.size = { 1.0f,1.0f,1.0f };

		if (mEngine->IsOBBToOBB(enemyObb1, enemyObb2)) {

			/*TODO　敵同士が重ならないようにする処理で*/

		}

	}

}

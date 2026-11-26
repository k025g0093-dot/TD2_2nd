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

	// 敵の移動前の位置を保存
	mEnemyOldPositions.clear();

	for (Enemy& enemy : mEnemies) {
		mEnemyOldPositions.push_back(enemy.GetPosition());
	}

	// 敵の更新
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

	// 移動前の位置が保存されていなければ処理しない
	if (mEnemyOldPositions.size() != mEnemies.size()) {
		return;
	}

	// 移動先が他の敵と重なるかを調べる
	auto IsBlocked = [&](size_t index, const Vector3& position) {

		OBB enemyObb1{};
		enemyObb1.center = position;
		enemyObb1.orientations[0] = { 1,0,0 };
		enemyObb1.orientations[1] = { 0,1,0 };
		enemyObb1.orientations[2] = { 0,0,1 };
		enemyObb1.size = { 1.0f,1.0f,1.0f };

		for (size_t j = 0; j < mEnemies.size(); j++) {

			if (index == j) {
				continue;
			}

			OBB enemyObb2{};
			enemyObb2.center = mEnemies[j].GetPosition();
			enemyObb2.orientations[0] = { 1,0,0 };
			enemyObb2.orientations[1] = { 0,1,0 };
			enemyObb2.orientations[2] = { 0,0,1 };
			enemyObb2.size = { 1.0f,1.0f,1.0f };

			if (mEngine->IsOBBToOBB(enemyObb1, enemyObb2)) {
				return true;
			}
		}

		return false;
		};

	// 敵を1体ずつ確認
	for (size_t i = 0; i < mEnemies.size(); i++) {

		Enemy& enemy = mEnemies[i];

		Vector3 oldPosition = mEnemyOldPositions[i];
		Vector3 nextPosition = enemy.GetPosition();

		// 今回のフレームで進もうとした距離
		float dx = nextPosition.x - oldPosition.x;
		float dz = nextPosition.z - oldPosition.z;

		float distance = std::sqrt(dx * dx + dz * dz);

		// 移動していない敵は処理しない
		if (distance <= 0.0001f) {
			continue;
		}

		// 重ならなければ普通に移動
		if (!IsBlocked(i, nextPosition)) {
			continue;
		}

		// いったん移動前の位置に戻す
		enemy.SetPosition(oldPosition);

		// 進もうとした方向を正規化
		float dirX = dx / distance;
		float dirZ = dz / distance;

		// 回り込み用の角度
		const float kPi = 3.14159265f;

		const float angles[] = {
			45.0f,
			90.0f,
			135.0f
		};

		bool moved = false;

		// 左右の回り込みを試す
		for (float angleDegree : angles) {

			float angle = angleDegree * kPi / 180.0f;

			for (int side = 0; side < 2; side++) {

				// 敵ごとに優先方向を固定して、
				// 左右の切り替わりを減らす
				float sign = (i % 2 == 0) ? 1.0f : -1.0f;

				if (side == 1) {
					sign = -sign;
				}

				float a = angle * sign;

				// 移動方向を回転
				float moveX =
					dirX * std::cos(a) -
					dirZ * std::sin(a);

				float moveZ =
					dirX * std::sin(a) +
					dirZ * std::cos(a);

				Vector3 candidate = oldPosition;

				candidate.x += moveX * distance;
				candidate.z += moveZ * distance;

				// フィールドから出ないようにする
				candidate.x = std::clamp(
					candidate.x, -95.0f, 95.0f
				);

				candidate.z = std::clamp(
					candidate.z, -95.0f, 95.0f
				);

				// 回り込んだ先が空いていれば移動
				if (!IsBlocked(i, candidate)) {

					enemy.SetPosition(candidate);
					moved = true;
					break;
				}
			}

			if (moved) {
				break;
			}
		}

		// 回り込めない場合はその場で停止
		if (!moved) {
			enemy.SetPosition(oldPosition);
		}
	}
}
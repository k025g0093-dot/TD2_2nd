#include "Enemy.h"

// 初期化
void Enemy::Initialize(int model, int animation, const Vector3& position) {

    mModel = model;
    mAnimation = animation;
    mPosition = position;
}

void Enemy::Update() {
	// 敵の更新処理（必要に応じて追加）
}

// 描画
void Enemy::Draw(ShizukuEngine* engine) {

    engine->DrawSkinnedModel(
        mModel,
        mAnimation,
        mPosition,
        { 0.0f, mRotationY, 0.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    );
}
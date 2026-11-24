#include "Enemy.h"

#include <cmath>

// 初期化
void Enemy::Initialize(int model, int animation, const Vector3& position, Type type) {

    mModel = model;
    mAnimation = animation;
    mPosition = position;
    mType = type;

    mRotationY = 0.0f;

    mShotTimer = 0.0f;
    mBullets.clear();

    if (mType == Type::kChase) {
        mSpeed = 4.0f;
    }
    else if (mType == Type::kRanged) {
        mSpeed = 2.5f;
    }
}

// 更新
void Enemy::Update(const Vector3& playerPosition, float dt) {

    switch (mType) {

    case Type::kChase:

        UpdateChase(playerPosition, dt);
        break;

    case Type::kRanged:

        UpdateRanged(playerPosition, dt);
        break;
    }

    UpdateBullets(dt);
}

// 近距離型の更新
void Enemy::UpdateChase(
    const Vector3& playerPosition,
    float dt) {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    float distance = std::sqrt(dx * dx + dz * dz);

    if (distance > 1.0f) {

        float dirX = dx / distance;
        float dirZ = dz / distance;

        mPosition.x += dirX * mSpeed * dt;
        mPosition.z += dirZ * mSpeed * dt;

        mRotationY = std::atan2(dirX, dirZ);
    }
}

// 遠距離型の更新
void Enemy::UpdateRanged(
    const Vector3& playerPosition,
    float dt) {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    float distance = std::sqrt(dx * dx + dz * dz);

    if (distance > 0.001f) {

        float dirX = dx / distance;
        float dirZ = dz / distance;

        // プレイヤーの方向を向く
        mRotationY = std::atan2(dirX, dirZ);

        // 遠い場合は近づく
        if (distance > mKeepDistance + 3.0f) {

            mPosition.x += dirX * mSpeed * dt;
            mPosition.z += dirZ * mSpeed * dt;
        }

        // 近すぎる場合は後退する
        else if (distance < mKeepDistance - 3.0f) {

            mPosition.x -= dirX * mSpeed * dt;
            mPosition.z -= dirZ * mSpeed * dt;
        }
    }

    // フィールドから出ないようにする
    mPosition.x = std::clamp(mPosition.x, -95.0f, 95.0f);
    mPosition.z = std::clamp(mPosition.z, -95.0f, 95.0f);

    // 攻撃タイマー
    mShotTimer += dt;

    if (mShotTimer >= mShotInterval) {

        mShotTimer = 0.0f;

        Shot(playerPosition);
    }
}

// 弾の発射
void Enemy::Shot(const Vector3& playerPosition) {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    float distance = std::sqrt(dx * dx + dz * dz);

    if (distance <= 0.001f) {
        return;
    }

    Bullet bullet;

    bullet.position = mPosition;
    bullet.position.y = 1.0f;

    bullet.direction.x = dx / distance;
    bullet.direction.y = 0.0f;
    bullet.direction.z = dz / distance;

    bullet.speed = 15.0f;
    bullet.lifeTime = 5.0f;

    mBullets.push_back(bullet);
}

// 弾の更新
void Enemy::UpdateBullets(float dt) {

    for (Bullet& bullet : mBullets) {

        bullet.position.x +=
            bullet.direction.x * bullet.speed * dt;

        bullet.position.z +=
            bullet.direction.z * bullet.speed * dt;

        bullet.lifeTime -= dt;
    }

    // 寿命が切れた弾を削除
    mBullets.erase(
        std::remove_if(
            mBullets.begin(),
            mBullets.end(),
            [](const Bullet& bullet) {
                return bullet.lifeTime <= 0.0f;
            }
        ),
        mBullets.end()
    );
}

// 描画
void Enemy::Draw(ShizukuEngine* engine) {

    // 敵本体
    if (mType == Type::kChase) {

        engine->DrawSkinnedModel(
            mModel,
            mAnimation,
            mPosition,
            { 0.0f, mRotationY, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        );
    }
    else if (mType == Type::kRanged) {

        // 遠距離型は色と大きさを変更
        engine->DrawSkinnedModel(
            mModel,
            mAnimation,
            mPosition,
            { 0.0f, mRotationY, 0.0f },
            { 1.0f, 1.0f, 1.0f },
            { 0.4f, 0.4f, 1.0f, 1.0f }
        );
    }

    // 弾の描画
    for (const Bullet& bullet : mBullets) {

        engine->DrawSkinnedModel(
            mModel,
            mAnimation,
            bullet.position,
            { 0.0f, 0.0f, 0.0f },
            { 0.2f, 0.2f, 0.2f },
            { 1.0f, 0.3f, 0.3f, 1.0f }
        );
    }
}
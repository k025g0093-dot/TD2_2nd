#include "Boss.h"

#include <algorithm>
#include <cmath>

// 初期化
void Boss::Initialize(int model, int animation, const Vector3& position) {

    mModel = model;
    mAnimation = animation;
    mPosition = position;

    mRotationY = 0.0f;
    mChargeDirection = Vector3{ 0.0f, 0.0f, 1.0f };

    mState = State::kMove;

    mAttackTimer = 0.0f;
    mWaitTimer = 0.0f;
    mChargeTimer = 0.0f;
    mRecoveryTimer = 0.0f;
}

// 更新
void Boss::Update(const Vector3& playerPosition, float dt) {

    switch (mState) {

    case State::kMove:

        Move(playerPosition, dt);
        break;

    case State::kWait:

        Wait(dt);
        break;

    case State::kCharge:

        Charge(dt);
        break;

    case State::kRecovery:

        Recovery(dt);
        break;
    }

    // フィールド外に出ないようにする
    mPosition.x = std::clamp(
        mPosition.x, -kFieldHalf, kFieldHalf
    );

    mPosition.z = std::clamp(
        mPosition.z, -kFieldHalf, kFieldHalf
    );
}

// 通常移動
void Boss::Move(const Vector3& playerPosition, float dt) {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    float distance = std::sqrt(dx * dx + dz * dz);

    if (distance > 0.001f) {

        float dirX = dx / distance;
        float dirZ = dz / distance;

        // プレイヤーの方向を向く
        mRotationY = std::atan2(dirX, dirZ);

        // 遠い場合は近づく
        if (distance > mKeepDistance + 2.0f) {

            mPosition.x += dirX * mMoveSpeed * dt;
            mPosition.z += dirZ * mMoveSpeed * dt;
        }

        // 近い場合は後退する
        else if (distance < mKeepDistance - 2.0f) {

            mPosition.x -= dirX * mMoveSpeed * dt;
            mPosition.z -= dirZ * mMoveSpeed * dt;
        }
    }

    // 攻撃までの時間
    mAttackTimer += dt;

    if (mAttackTimer >= mAttackInterval) {

        mAttackTimer = 0.0f;
        mWaitTimer = 0.0f;

        mState = State::kWait;
    }
}

// 突進準備
void Boss::Wait(float dt) {

    mWaitTimer += dt;

    if (mWaitTimer >= mWaitTime) {

        mWaitTimer = 0.0f;
        mChargeTimer = 0.0f;

		// 突進方向を設定
        mChargeDirection.x = std::sin(mRotationY);
        mChargeDirection.y = 0.0f;
        mChargeDirection.z = std::cos(mRotationY);

        mState = State::kCharge;
    }
}

// 突進攻撃
void Boss::Charge(float dt) {

    mPosition.x += mChargeDirection.x * mChargeSpeed * dt;
    mPosition.z += mChargeDirection.z * mChargeSpeed * dt;

    mChargeTimer += dt;

    if (mChargeTimer >= mChargeTime) {

        mChargeTimer = 0.0f;
        mRecoveryTimer = 0.0f;

        mState = State::kRecovery;
    }
}

// 攻撃後の硬直
void Boss::Recovery(float dt) {

    mRecoveryTimer += dt;

    if (mRecoveryTimer >= mRecoveryTime) {

        mRecoveryTimer = 0.0f;

        mState = State::kMove;
    }
}

// 描画
void Boss::Draw(ShizukuEngine* engine) {

    engine->DrawSkinnedModel(
        mModel,
        mAnimation,
        mPosition,
        { 0.0f, mRotationY, 0.0f },
        { 2.0f, 2.0f, 2.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    );
}
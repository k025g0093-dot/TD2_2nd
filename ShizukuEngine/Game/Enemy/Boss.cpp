#include "Boss.h"

#include <algorithm>
#include <cmath>

// 初期化
void Boss::Initialize(int model, int animation, const Vector3& position) {

    mModel = model;
    mAnimation = animation;
    mPosition = position;

    mRotationY = 0.0f;

    mChargeDirection = { 0.0f, 0.0f, 1.0f };
    mScale = { 2.0f, 2.0f, 2.0f };

    mState = State::kMove;

    mAttackTimer = 0.0f;
    mNextFarAttackCharge = true;

    mWaitTimer = 0.0f;
    mChargeTimer = 0.0f;

    mSweepWaitTimer = 0.0f;
    mSweepTimer = 0.0f;

    mSlamWaitTimer = 0.0f;

    mShockwavePosition = mPosition;
    mShockwaveRadius = 0.0f;
    mShockwaveTimer = 0.0f;

    mRecoveryTimer = 0.0f;
}

// プレイヤーまでの距離
float Boss::GetDistance(const Vector3& playerPosition) const {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    return std::sqrt(dx * dx + dz * dz);
}

// プレイヤーの方向を向く
void Boss::LookAtPlayer(const Vector3& playerPosition) {

    float dx = playerPosition.x - mPosition.x;
    float dz = playerPosition.z - mPosition.z;

    float distance = GetDistance(playerPosition);

    if (distance > 0.001f) {

        float dirX = dx / distance;
        float dirZ = dz / distance;

        mRotationY = std::atan2(dirX, dirZ);
    }
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

    case State::kSweepWait:
        SweepWait(dt);
        break;

    case State::kSweep:
        Sweep(dt);
        break;

    case State::kSlamWait:
        SlamWait(dt);
        break;

    case State::kShockwave:
        Shockwave(dt);
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

    float distance = GetDistance(playerPosition);

    if (distance > 0.001f) {

        float dirX = dx / distance;
        float dirZ = dz / distance;

        // プレイヤーの方向を向く
        mRotationY = std::atan2(dirX, dirZ);

        // 遠いときは近づく
        if (distance > mKeepDistance + 2.0f) {

            mPosition.x += dirX * mMoveSpeed * dt;
            mPosition.z += dirZ * mMoveSpeed * dt;
        }

        // 近すぎる場合は後退
        else if (distance < mKeepDistance - 2.0f) {

            mPosition.x -= dirX * mMoveSpeed * dt;
            mPosition.z -= dirZ * mMoveSpeed * dt;
        }
    }

    // 攻撃タイマー
    mAttackTimer += dt;

    if (mAttackTimer >= mAttackInterval) {

        mAttackTimer = 0.0f;

        LookAtPlayer(playerPosition);

        // 近距離なら薙ぎ払い
        if (GetDistance(playerPosition) <= mAttackDistance) {

            mSweepWaitTimer = 0.0f;
            mState = State::kSweepWait;
        }
        // 遠距離なら突進か波動
        else {

            if (mNextFarAttackCharge) {

                // 突進
                mWaitTimer = 0.0f;
                mState = State::kWait;
            }
            else {

                // 波動
                mSlamWaitTimer = 0.0f;
                mState = State::kSlamWait;
            }

            // 次回の遠距離攻撃を切り替える
            mNextFarAttackCharge = !mNextFarAttackCharge;
        }
    }
}

// 突進準備
void Boss::Wait(float dt) {

    mWaitTimer += dt;

    if (mWaitTimer >= mWaitTime) {

        mWaitTimer = 0.0f;
        mChargeTimer = 0.0f;

        // 突進方向を固定
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

// 薙ぎ払い準備
void Boss::SweepWait(float dt) {

    // 赤い扇形の予告を表示して停止
    mSweepWaitTimer += dt;

    if (mSweepWaitTimer >= mSweepWaitTime) {

        mSweepWaitTimer = 0.0f;
        mSweepTimer = 0.0f;

        mState = State::kSweep;
    }
}

// 薙ぎ払い攻撃
void Boss::Sweep(float dt) {

    // まだアニメーションと当たり判定はなし
    mSweepTimer += dt;

    if (mSweepTimer >= mSweepTime) {

        mSweepTimer = 0.0f;
        mRecoveryTimer = 0.0f;

        mState = State::kRecovery;
    }
}

// 波動攻撃の溜め
void Boss::SlamWait(float dt) {

    // 溜めている間は停止
    mSlamWaitTimer += dt;

    if (mSlamWaitTimer >= mSlamWaitTime) {

        mSlamWaitTimer = 0.0f;

        // 衝撃波の発生位置を固定
        mShockwavePosition = mPosition;

        mShockwaveRadius = 0.0f;
        mShockwaveTimer = 0.0f;

        mState = State::kShockwave;
    }
}

// 波動の更新
void Boss::Shockwave(float dt) {

    mShockwaveTimer += dt;

    float t = mShockwaveTimer / mShockwaveTime;
    t = std::clamp(t, 0.0f, 1.0f);

    // 円が外側へ広がる
    mShockwaveRadius = mShockwaveMaxRadius * t;

    if (mShockwaveTimer >= mShockwaveTime) {

        mShockwaveTimer = 0.0f;
        mShockwaveRadius = 0.0f;

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

// 薙ぎ払いの扇形予告
void Boss::DrawSweepWarning(ShizukuEngine* engine) {

    const float kPi = 3.14159265f;
    const float halfAngle = kPi / 3.0f;

    const int segments = 32;

    Vector3 center = mPosition;
    center.y += 0.1f;

    Vector4 color = { 1.0f, 0.1f, 0.1f, 1.0f };

    for (int i = 0; i < segments; i++) {

        float angle1 = mRotationY - halfAngle +
            (2.0f * halfAngle * i / segments);

        float angle2 = mRotationY - halfAngle +
            (2.0f * halfAngle * (i + 1) / segments);

        Vector3 point1 = {
            center.x + std::sin(angle1) * mSweepRange,
            center.y,
            center.z + std::cos(angle1) * mSweepRange
        };

        Vector3 point2 = {
            center.x + std::sin(angle2) * mSweepRange,
            center.y,
            center.z + std::cos(angle2) * mSweepRange
        };

        // 円弧
        engine->DrawLine(point1, point2, color);

        // 扇形の内側の線
        engine->DrawLine(center, point1, color);
    }

    // 最後の線
    float endAngle = mRotationY + halfAngle;

    Vector3 endPoint = {
        center.x + std::sin(endAngle) * mSweepRange,
        center.y,
        center.z + std::cos(endAngle) * mSweepRange
    };

    engine->DrawLine(center, endPoint, color);
}

// 円形の波動描画
void Boss::DrawShockwave(ShizukuEngine* engine) {

    const float kPi = 3.14159265f;
    const int segments = 64;

    Vector3 center = mShockwavePosition;
    center.y += 0.2f;

    Vector4 color = { 1.0f, 0.2f, 0.1f, 1.0f };

    const float waveWidth = 0.8f;

    float outerRadius = mShockwaveRadius;
    float innerRadius = std::max(
        0.0f, outerRadius - waveWidth
    );

    for (int i = 0; i < segments; i++) {

        float angle1 = 2.0f * kPi * i / segments;
        float angle2 = 2.0f * kPi * (i + 1) / segments;

        Vector3 outer1 = {
            center.x + std::sin(angle1) * outerRadius,
            center.y,
            center.z + std::cos(angle1) * outerRadius
        };

        Vector3 outer2 = {
            center.x + std::sin(angle2) * outerRadius,
            center.y,
            center.z + std::cos(angle2) * outerRadius
        };

        Vector3 inner1 = {
            center.x + std::sin(angle1) * innerRadius,
            center.y,
            center.z + std::cos(angle1) * innerRadius
        };

        Vector3 inner2 = {
            center.x + std::sin(angle2) * innerRadius,
            center.y,
            center.z + std::cos(angle2) * innerRadius
        };

        // 二重の円を描画
        engine->DrawLine(outer1, outer2, color);
        engine->DrawLine(inner1, inner2, color);
    }
}

// 描画
void Boss::Draw(ShizukuEngine* engine) {

    // 薙ぎ払いの予告
    if (mState == State::kSweepWait) {

        DrawSweepWarning(engine);
    }

    // 波動攻撃
    if (mState == State::kShockwave) {

        DrawShockwave(engine);
    }

    // ボス本体
    engine->DrawSkinnedModel(
        mModel,
        mAnimation,
        mPosition,
        { 0.0f, mRotationY, 0.0f },
        mScale,
        { 1.0f, 1.0f, 1.0f, 1.0f }
    );
}
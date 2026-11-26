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
    mFarAttackCount = 0;

    // 突進
    mWaitTimer = 0.0f;
    mChargeTimer = 0.0f;

    // 薙ぎ払い
    mSweepWaitTimer = 0.0f;
    mSweepTimer = 0.0f;

    // 波動
    mSlamWaitTimer = 0.0f;
    mShockwavePosition = mPosition;
    mShockwaveRadius = 0.0f;
    mShockwaveTimer = 0.0f;

    // ジャンプ
    mJumpStartPosition = mPosition;
    mJumpTargetPosition = mPosition;
    mJumpWaitTimer = 0.0f;
    mJumpTimer = 0.0f;

    // エリート召喚
    mSummonWaitTimer = 0.0f;
    mSummonRequested = false;

    mRecoveryTimer = 0.0f;
}

// GameSceneに召喚要求を渡す
bool Boss::ConsumeSummonRequest() {

    bool requested = mSummonRequested;

    mSummonRequested = false;

    return requested;
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

    case State::kJumpWait:
        JumpWait(dt);
        break;

    case State::kJump:
        Jump(dt);
        break;

    case State::kSummonWait:
        SummonWait(dt);
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

        // 遠い場合は近づく
        if (distance > mKeepDistance + 2.0f) {

            mPosition.x += dirX * mMoveSpeed * dt;
            mPosition.z += dirZ * mMoveSpeed * dt;
        }

        // 近い場合は後退
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

        // 距離に関係なく25%でエリート召喚
        std::uniform_int_distribution<int> summonRoll(0, 3);

        if (summonRoll(mRandom) == 0) {

            mSummonWaitTimer = 0.0f;
            mState = State::kSummonWait;
        }

        // 近距離なら薙ぎ払い
        else if (distance <= mAttackDistance) {

            mSweepWaitTimer = 0.0f;
            mState = State::kSweepWait;
        }

        // 遠距離なら突進・波動・ジャンプ
        else {

            switch (mFarAttackCount) {

            case 0:

                // 突進
                mWaitTimer = 0.0f;
                mState = State::kWait;
                break;

            case 1:

                // 波動
                mSlamWaitTimer = 0.0f;
                mState = State::kSlamWait;
                break;

            case 2:

                // ジャンプ攻撃
                mJumpWaitTimer = 0.0f;
                mJumpTimer = 0.0f;

                mJumpStartPosition = mPosition;

                // 着地点をプレイヤーの位置に固定
                mJumpTargetPosition = playerPosition;
                mJumpTargetPosition.y = mJumpStartPosition.y;

                mJumpTargetPosition.x = std::clamp(
                    mJumpTargetPosition.x,
                    -kFieldHalf,
                    kFieldHalf
                );

                mJumpTargetPosition.z = std::clamp(
                    mJumpTargetPosition.z,
                    -kFieldHalf,
                    kFieldHalf
                );

                mState = State::kJumpWait;
                break;
            }

            // 突進 → 波動 → ジャンプ
            mFarAttackCount = (mFarAttackCount + 1) % 3;
        }
    }
}

// 突進準備
void Boss::Wait(float dt) {

    mWaitTimer += dt;

    if (mWaitTimer >= mWaitTime) {

        mWaitTimer = 0.0f;
        mChargeTimer = 0.0f;

        mChargeDirection.x = std::sin(mRotationY);
        mChargeDirection.y = 0.0f;
        mChargeDirection.z = std::cos(mRotationY);

        mState = State::kCharge;
    }
}

// 突進攻撃
void Boss::Charge(float dt) {

    mPosition.x +=
        mChargeDirection.x * mChargeSpeed * dt;

    mPosition.z +=
        mChargeDirection.z * mChargeSpeed * dt;

    mChargeTimer += dt;

    if (mChargeTimer >= mChargeTime) {

        mChargeTimer = 0.0f;
        mRecoveryTimer = 0.0f;

        mState = State::kRecovery;
    }
}

// 薙ぎ払い準備
void Boss::SweepWait(float dt) {

    mSweepWaitTimer += dt;

    if (mSweepWaitTimer >= mSweepWaitTime) {

        mSweepWaitTimer = 0.0f;
        mSweepTimer = 0.0f;

        mState = State::kSweep;
    }
}

// 薙ぎ払い
void Boss::Sweep(float dt) {

    // アニメーションと当たり判定はまだなし
    mSweepTimer += dt;

    if (mSweepTimer >= mSweepTime) {

        mSweepTimer = 0.0f;
        mRecoveryTimer = 0.0f;

        mState = State::kRecovery;
    }
}

// 波動攻撃の溜め
void Boss::SlamWait(float dt) {

    mSlamWaitTimer += dt;

    if (mSlamWaitTimer >= mSlamWaitTime) {

        mSlamWaitTimer = 0.0f;

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

    // 円を広げる
    mShockwaveRadius = mShockwaveMaxRadius * t;

    if (mShockwaveTimer >= mShockwaveTime) {

        mShockwaveTimer = 0.0f;
        mShockwaveRadius = 0.0f;

        mRecoveryTimer = 0.0f;
        mState = State::kRecovery;
    }
}

// ジャンプ準備
void Boss::JumpWait(float dt) {

    mJumpWaitTimer += dt;

    if (mJumpWaitTimer >= mJumpWaitTime) {

        mJumpWaitTimer = 0.0f;
        mJumpTimer = 0.0f;

        mState = State::kJump;
    }
}

// ジャンプ攻撃
void Boss::Jump(float dt) {

    mJumpTimer += dt;

    float t = mJumpTimer / mJumpTime;

    t = std::clamp(t, 0.0f, 1.0f);

    // 開始地点から着地点まで移動
    mPosition.x =
        mJumpStartPosition.x +
        (mJumpTargetPosition.x - mJumpStartPosition.x) * t;

    mPosition.z =
        mJumpStartPosition.z +
        (mJumpTargetPosition.z - mJumpStartPosition.z) * t;

    // 放物線状に上昇・下降
    float height =
        4.0f * mJumpHeight * t * (1.0f - t);

    mPosition.y = mJumpStartPosition.y + height;

    if (mJumpTimer >= mJumpTime) {

        mPosition = mJumpTargetPosition;

        mJumpTimer = 0.0f;
        mRecoveryTimer = 0.0f;

        mState = State::kRecovery;
    }
}

// エリート召喚
void Boss::SummonWait(float dt) {

    mSummonWaitTimer += dt;

    if (mSummonWaitTimer >= mSummonWaitTime) {

        mSummonWaitTimer = 0.0f;

        // GameSceneへ召喚要求を渡す
        mSummonRequested = true;

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

    float endAngle = mRotationY + halfAngle;

    Vector3 endPoint = {
        center.x + std::sin(endAngle) * mSweepRange,
        center.y,
        center.z + std::cos(endAngle) * mSweepRange
    };

    engine->DrawLine(center, endPoint, color);
}

// 波動の描画
void Boss::DrawShockwave(ShizukuEngine* engine) {

    const float kPi = 3.14159265f;
    const int segments = 64;

    Vector3 center = mShockwavePosition;
    center.y += 0.2f;

    Vector4 color = { 1.0f, 0.2f, 0.1f, 1.0f };

    const float waveWidth = 0.8f;

    float outerRadius = mShockwaveRadius;

    float innerRadius = std::max(
        0.0f,
        outerRadius - waveWidth
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

        engine->DrawLine(outer1, outer2, color);
        engine->DrawLine(inner1, inner2, color);
    }
}

// ジャンプの円形予告
void Boss::DrawJumpWarning(ShizukuEngine* engine) {

    const float kPi = 3.14159265f;
    const int segments = 64;

    Vector3 center = mJumpTargetPosition;
    center.y += 0.2f;

    Vector4 color = { 1.0f, 0.1f, 0.1f, 1.0f };

    for (int i = 0; i < segments; i++) {

        float angle1 = 2.0f * kPi * i / segments;
        float angle2 = 2.0f * kPi * (i + 1) / segments;

        Vector3 point1 = {
            center.x + std::sin(angle1) * mJumpWarningRadius,
            center.y,
            center.z + std::cos(angle1) * mJumpWarningRadius
        };

        Vector3 point2 = {
            center.x + std::sin(angle2) * mJumpWarningRadius,
            center.y,
            center.z + std::cos(angle2) * mJumpWarningRadius
        };

        engine->DrawLine(point1, point2, color);
    }
}

// 描画
void Boss::Draw(ShizukuEngine* engine) {

    // 薙ぎ払い予告
    if (mState == State::kSweepWait) {
        DrawSweepWarning(engine);
    }

    // 波動
    if (mState == State::kShockwave) {
        DrawShockwave(engine);
    }

    // ジャンプ予告
    if (mState == State::kJumpWait ||
        mState == State::kJump) {

        DrawJumpWarning(engine);
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
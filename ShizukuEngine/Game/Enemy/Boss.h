#pragma once

#include "../../ShizukuEngine/ShizukuEngine.h"
#include <random>

class Boss
{
public:

    enum class State {
        kMove,
        // 突進
        kWait,
        kCharge,
        // 薙ぎ払い
        kSweepWait,
        kSweep,
        // 波動
        kSlamWait,
        kShockwave,
        // ジャンプ攻撃
        kJumpWait,
        kJump,
        // エリート召喚
        kSummonWait,
        // 硬直
        kRecovery
    };

public:

    void Initialize(int model, int animation, const Vector3& position);
    void Update(const Vector3& playerPosition, float dt);
    void Draw(ShizukuEngine* engine);

    // GameSceneに召喚要求を渡す
    bool ConsumeSummonRequest();

    const Vector3& GetPosition() const { return mPosition; }

    const Vector3& GetScale() const { return mScale; }

private:

    // 通常移動
    void Move(const Vector3& playerPosition, float dt);

    // 突進
    void Wait(float dt);
    void Charge(float dt);

    // 薙ぎ払い
    void SweepWait(float dt);
    void Sweep(float dt);

    // 波動
    void SlamWait(float dt);
    void Shockwave(float dt);

    // ジャンプ
    void JumpWait(float dt);
    void Jump(float dt);

    // エリート召喚
    void SummonWait(float dt);

    // 硬直
    void Recovery(float dt);

    // プレイヤーとの距離
    float GetDistance(const Vector3& playerPosition) const;

    // プレイヤーの方向を向く
    void LookAtPlayer(const Vector3& playerPosition);

    // 薙ぎ払い予告
    void DrawSweepWarning(ShizukuEngine* engine);

    // 波動描画
    void DrawShockwave(ShizukuEngine* engine);

    // ジャンプ予告
    void DrawJumpWarning(ShizukuEngine* engine);

private:

    int mModel = -1;
    int mAnimation = -1;

    Vector3 mPosition{};
    Vector3 mChargeDirection{};
    Vector3 mScale{};

    float mRotationY = 0.0f;

    State mState = State::kMove;

    // 通常移動
    float mMoveSpeed = 3.0f;
    float mKeepDistance = 12.0f;

    // 攻撃間隔
    float mAttackInterval = 3.0f;
    float mAttackTimer = 0.0f;

    // 距離による攻撃切り替え
    float mAttackDistance = 10.0f;

    // 遠距離攻撃の順番
    // 0:突進 1:波動 2:ジャンプ
    int mFarAttackCount = 0;

    // 突進準備
    float mWaitTime = 0.8f;
    float mWaitTimer = 0.0f;

    // 突進
    float mChargeSpeed = 25.0f;
    float mChargeTime = 1.0f;
    float mChargeTimer = 0.0f;

    // 薙ぎ払い準備
    float mSweepWaitTime = 0.8f;
    float mSweepWaitTimer = 0.0f;

    // 薙ぎ払い
    float mSweepTime = 0.35f;
    float mSweepTimer = 0.0f;
    float mSweepRange = 10.0f;

    // 波動攻撃の溜め
    float mSlamWaitTime = 2.0f;
    float mSlamWaitTimer = 0.0f;

    // 波動
    Vector3 mShockwavePosition{};

    float mShockwaveRadius = 0.0f;
    float mShockwaveMaxRadius = 50.0f;

    float mShockwaveTime = 2.0f;
    float mShockwaveTimer = 0.0f;

    // ジャンプ
    Vector3 mJumpStartPosition{};
    Vector3 mJumpTargetPosition{};

    float mJumpWaitTime = 0.8f;
    float mJumpWaitTimer = 0.0f;

    float mJumpTime = 0.9f;
    float mJumpTimer = 0.0f;

    float mJumpHeight = 12.0f;
    float mJumpWarningRadius = 5.0f;

    // エリート召喚
    std::mt19937 mRandom{ std::random_device{}() };

    bool mSummonRequested = false;

    float mSummonWaitTime = 1.0f;
    float mSummonWaitTimer = 0.0f;

    // 硬直
    float mRecoveryTime = 1.0f;
    float mRecoveryTimer = 0.0f;

    static constexpr float kFieldHalf = 95.0f;
};
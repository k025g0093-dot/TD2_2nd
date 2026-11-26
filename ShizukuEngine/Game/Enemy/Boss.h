#pragma once

#include "../../ShizukuEngine/ShizukuEngine.h"

class Boss
{
public:

    enum class State {
        kMove,
        kWait,
        kCharge,
        kSweepWait,
        kSweep,
        kSlamWait,
        kShockwave,
        kRecovery
    };

public:

    void Initialize(int model, int animation, const Vector3& position);

    void Update(const Vector3& playerPosition, float dt);

    void Draw(ShizukuEngine* engine);

    const Vector3& GetPosition() const { return mPosition; }

    const Vector3& GetScale() const { return mScale; }

private:

    // 状態ごとの更新
    void Move(const Vector3& playerPosition, float dt);

    // 突進攻撃
    void Wait(float dt);
    void Charge(float dt);

    // 薙ぎ払い攻撃
    void SweepWait(float dt);
    void Sweep(float dt);

    // 波動攻撃
    void SlamWait(float dt);
    void Shockwave(float dt);

    // 攻撃後の硬直
    void Recovery(float dt);

    // プレイヤーとの距離
    float GetDistance(const Vector3& playerPosition) const;

    // プレイヤーの方向を向く
    void LookAtPlayer(const Vector3& playerPosition);

    // 薙ぎ払いの予告
    void DrawSweepWarning(ShizukuEngine* engine);

    // 波動の描画
    void DrawShockwave(ShizukuEngine* engine);

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

    // 攻撃
    float mAttackInterval = 3.0f;
    float mAttackTimer = 0.0f;

    // 近距離と遠距離の切り替え
    float mAttackDistance = 10.0f;

    // 遠距離攻撃の切り替え
    bool mNextFarAttackCharge = true;

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

    // 硬直
    float mRecoveryTime = 1.0f;
    float mRecoveryTimer = 0.0f;

    static constexpr float kFieldHalf = 95.0f;
};
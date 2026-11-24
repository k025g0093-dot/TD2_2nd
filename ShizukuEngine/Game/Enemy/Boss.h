#pragma once

#include "../../ShizukuEngine/ShizukuEngine.h"

class Boss
{
public:

    enum class State {
        kMove,
        kWait,
        kCharge,
        kRecovery
    };

public:

    void Initialize(int model, int animation, const Vector3& position);

    void Update(const Vector3& playerPosition, float dt);

    void Draw(ShizukuEngine* engine);

    const Vector3& GetPosition() const { return mPosition; }

private:

    // 状態ごとの更新
    void Move(const Vector3& playerPosition, float dt);

    void Wait(float dt);

    void Charge(float dt);

    void Recovery(float dt);

private:

    int mModel = -1;
    int mAnimation = -1;

    Vector3 mPosition{};
    Vector3 mChargeDirection{};

    float mRotationY = 0.0f;

    State mState = State::kMove;

    // 通常移動
    float mMoveSpeed = 3.0f;
    float mKeepDistance = 12.0f;

    // 攻撃
    float mAttackInterval = 3.0f;
    float mAttackTimer = 0.0f;

    // 突進準備
    float mWaitTime = 0.8f;
    float mWaitTimer = 0.0f;

    // 突進
    float mChargeSpeed = 25.0f;
    float mChargeTime = 0.5f;
    float mChargeTimer = 0.0f;

    // 硬直
    float mRecoveryTime = 1.0f;
    float mRecoveryTimer = 0.0f;

    static constexpr float kFieldHalf = 95.0f;
};
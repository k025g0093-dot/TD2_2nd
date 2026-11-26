#pragma once

#include "../../ShizukuEngine/ShizukuEngine.h"
#include <vector>

class Enemy
{
public:

    // 敵の種類
    enum class Type {
        kChase,   // 近距離型
        kRanged,  // 遠距離型
        kElite    // エリート型
    };

    // 弾
    struct Bullet {
        Vector3 position{};
        Vector3 direction{};
        float speed = 15.0f;
        float lifeTime = 5.0f;
    };

public:

    void Initialize(
        int model,
        int animation,
        const Vector3& position,
        Type type = Type::kChase
    );

    void Update(const Vector3& playerPosition, float dt);

    void Draw(ShizukuEngine* engine);

    const Vector3& GetPosition() const { return mPosition; }

private:

    // 近距離型・エリート型
    void UpdateChase(
        const Vector3& playerPosition,
        float dt
    );

    // 遠距離型
    void UpdateRanged(
        const Vector3& playerPosition,
        float dt
    );

    // 弾の発射
    void Shot(const Vector3& playerPosition);

    // 弾の更新
    void UpdateBullets(float dt);

private:

    int mModel = -1;
    int mAnimation = -1;

    Vector3 mPosition{};

    float mRotationY = 0.0f;

    Type mType = Type::kChase;

    // 移動速度
    float mSpeed = 4.0f;

    // 遠距離型の設定
    float mKeepDistance = 10.0f;

    float mShotInterval = 2.0f;
    float mShotTimer = 0.0f;

    // 弾の管理
    std::vector<Bullet> mBullets;
};
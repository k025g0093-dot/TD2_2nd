#pragma once

#include "../../ShizukuEngine/ShizukuEngine.h"

class Enemy
{
public:

    void Initialize(int model, int animation, const Vector3& position);

    void Update();

    void Draw(ShizukuEngine* engine);

    const Vector3& GetPosition() const { return mPosition; }

private:

    int mModel = -1;
    int mAnimation = -1;

    Vector3 mPosition{};

    float mRotationY = 0.0f;
};
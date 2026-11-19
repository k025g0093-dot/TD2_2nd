#pragma once
#include "PlayerCommand.h"

class ShizukuEngine;

class Player
{
public:

	void Initialize(int model, int animation,const Vector3& position);

	void Update(const PlayerCommand& cmd,float yaw, float dt);

	void Draw(ShizukuEngine* engine);

	const Vector3& GetPosition() const { return mPosition; }
	float GetRotationY() const { return mRotationY; }
	int GetModel() const { return mModel; }
	bool IsShotThisFrame() const { return mShotThisFrame; }

private:

	int mModel = -1;
	int mAnimation = -1;
	Vector3 mPosition{};
	float mRotationY = 0.0f;

	float mSpeed = 10.0f;
	float mFireInterval = 0.1f;
	float mFireTimer = 0.0f;
	bool mShotThisFrame = false;

	// 回避
	bool mIsDodging = false;
	bool mPrevDodgeButton = false;

	Vector3 mDodgeDirection{};

	float mDodgeSpeed = 30.0f;
	float mDodgeDuration = 0.25f;
	float mDodgeTimer = 0.0f;

	float mDodgeCooldown = 0.5f;
	float mDodgeCooldownTimer = 0.0f;

	static constexpr float kFieldHalf = 99.0f; // 壁の内側が ±100、自機の半径ぶん内側

};


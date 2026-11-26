#include "Player.h"
#include "../../ShizukuEngine/ShizukuEngine.h"
#include <algorithm>
#include <cmath>

void Player::Initialize(int model, int animation, const Vector3& position) {

	mModel = model;
	mAnimation = animation;
	mPosition = position;

	mIsDodging = false;
	mPrevDodgeButton = false;

	mDodgeDirection = Vector3{};
	mDodgeTimer = 0.0f;
	mDodgeCooldownTimer = 0.0f;
}

void Player::Update(const PlayerCommand& cmd, float yaw, float dt) {

	// クールタイム更新
	if (mDodgeCooldownTimer > 0.0f) {
		mDodgeCooldownTimer -= dt;
	}

	// Bボタンを押した瞬間
	bool dodgePressed = cmd.isDodge && !mPrevDodgeButton;
	mPrevDodgeButton = cmd.isDodge;

	// 回避開始
	if (dodgePressed && !mIsDodging && mDodgeCooldownTimer <= 0.0f) {

		mIsDodging = true;
		mDodgeTimer = mDodgeDuration;
		mDodgeCooldownTimer = mDodgeCooldown;

		// 左スティック入力
		Vector3 dodgeMove = cmd.move;

		// 入力がない場合は後ろに回避
		if (dodgeMove.Length() < 0.2f) {
			dodgeMove = Vector3{ 0.0f, 0.0f, -1.0f };
		}

		// 方向を正規化
		dodgeMove = dodgeMove.Normalized();

		// カメラの向きを反映
		mDodgeDirection.x =
			std::sin(yaw) * dodgeMove.z +
			std::cos(yaw) * dodgeMove.x;

		mDodgeDirection.y = 0.0f;

		mDodgeDirection.z =
			std::cos(yaw) * dodgeMove.z -
			std::sin(yaw) * dodgeMove.x;
	}

	// 回避中
	if (mIsDodging) {

		mPosition += mDodgeDirection * (mDodgeSpeed * dt);

		mDodgeTimer -= dt;

		if (mDodgeTimer <= 0.0f) {
			mIsDodging = false;
			mDodgeTimer = 0.0f;
		}
	}
	else {
		// 通常移動
		Vector3 worldMove{};
		worldMove.x = (std::sin(yaw)) * cmd.move.z + (std::cos(yaw)) * cmd.move.x;
		worldMove.y = mPosition.y;
		worldMove.z = (std::cos(yaw)) * cmd.move.z - (std::sin(yaw)) * cmd.move.x;

		mPosition += worldMove * (mSpeed * dt);
	}

	mPosition.x = std::clamp(mPosition.x, -kFieldHalf, kFieldHalf);
	mPosition.z = std::clamp(mPosition.z, -kFieldHalf, kFieldHalf);

}

void Player::Draw(ShizukuEngine* engine) {
	engine->DrawSkinnedModel(
		mModel,
		mAnimation,
		mPosition,
		{ 0.0f, mRotationY, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);
}

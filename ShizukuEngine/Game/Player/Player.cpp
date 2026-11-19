#include "Player.h"
#include "../../ShizukuEngine/ShizukuEngine.h"
#include <algorithm>
#include <cmath>

void Player::Initialize(int model, int animation,const Vector3& position) {

	mModel = model;
	mAnimation = animation;
	mPosition = position;
}

void Player::Update(const PlayerCommand& cmd,float yaw, float dt) {
	Vector3 worldMove{};
	worldMove.x = (std::sin(yaw))*cmd.move.z+ (std::cos(yaw))*cmd.move.x ;
	worldMove.y = mPosition.y;
	worldMove.z = (std::cos(yaw))*cmd.move.z - (std::sin(yaw))* cmd.move.x;

	mPosition += worldMove* (mSpeed * dt);


	mPosition.x = std::clamp(mPosition.x, -kFieldHalf, kFieldHalf);
	mPosition.z = std::clamp(mPosition.z, -kFieldHalf, kFieldHalf);

}

void Player::Draw(ShizukuEngine* engine) {
	for (int i = 0; i < 30; i++) {
		engine->DrawSkinnedModel(
			mModel,
			mAnimation,
			mPosition+i,
			{ 0.0f, mRotationY, 0.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f }
		);
	}
}

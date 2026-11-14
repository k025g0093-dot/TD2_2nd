#include "Player.h"
#include <algorithm>
#include <cmath>

void Player::Initialize(int model, const Vector3& position) {

	mModel = model;
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


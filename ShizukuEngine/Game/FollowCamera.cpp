#include "FollowCamera.h"
#include <cmath>
#include <iostream>

void FollowCamera::Initialize(Vector3 targetPosition) {


	mNow.pos = targetPosition + mOffset.pos;
	mNow.rotate = {};
}

Vector3 FollowCamera::UpdatePos(
	Vector3 targetPosition
) {
	Vector3 pos;
	Vector3 desiredPosition;
	desiredPosition = targetPosition + mDistance;
	targetPosition.y=targetPosition.y + 1;
	//yawとpitchを使い、プレイヤーの周辺を回るカメラを作成
	pos.x = targetPosition.x - (std::sin(mYaw) * std::cos(mPitch)) * mDistance;
	pos.y = targetPosition.y + std::sin(mPitch) * mDistance;
	pos.z = targetPosition.z - (cos(mYaw) * cos(mPitch)) * mDistance;

	mNow.pos = pos;
	return pos;
}

Vector3 FollowCamera::UpdateRotate(
	Vector3 rotate
) {

	mPitch += rotate.x;
	mYaw += rotate.y;

	if (mPitch <= -0.3f) {
		mPitch = -0.3f;
	}
	else if (mPitch >= 1.2f) {
		mPitch = 1.2f;
	}

	return Vector3{ mPitch, mYaw,0.0f };
}
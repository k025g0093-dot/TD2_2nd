#pragma once
#include "../ShizukuEngine/Engine/Math/Vector3.h"

struct FollowCameraParams
{
	Vector3 pos;
	Vector3 rotate;
};

class FollowCamera
{
public:

	void Initialize(Vector3 targetPosition);

	Vector3 UpdatePos(Vector3 targetPosition );
	Vector3 UpdateRotate(Vector3 rotate);
	float GetYaw()const { return mYaw; }
private:
	FollowCameraParams mOffset;
	FollowCameraParams mNow;
	float kInterpolationRate = 0.02f;

	float mYaw=0.0f;
	float mPitch=0.3f;
	float mDistance=10.0f;

};


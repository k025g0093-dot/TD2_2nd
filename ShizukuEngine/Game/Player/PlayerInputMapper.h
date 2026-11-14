#pragma once
#include "PlayerCommand.h"
#include "../../ShizukuEngine/Engine/Core/Input.h"
class PlayerInputMapper
{
public:
	PlayerCommand Make(const Input& input,float dt)const;
private:
	float mPadLookSpeed = 2.0f;
};


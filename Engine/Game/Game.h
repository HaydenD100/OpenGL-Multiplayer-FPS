#pragma once
#include "Engine/core/Common/GameCommon.h"
#include "Engine/Pathfinding/Pathfinding.h"
namespace Game {
	enum GameNetworkingMode {
		SinglePlayer, 
		MultiPlayer
	};

	enum GameSettings {

	};

	void Init(GameNetworkingMode multiPlayerMode);
	void Update(float dt);
	void CheckDebugPress();
	void CleanUp();
}
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <fmod.hpp>
#include <fmod_errors.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sound {
public:
	Sound(const char* Path, std::string Name, glm::vec3 Position, float distance, float Volume);

	FMOD_VECTOR* GetPositionFmod();
	void SetPosition(glm::vec3 Position);
	FMOD::Sound* GetSound();
	std::string GetName();
	
	void SetVolume(float Volume);
	float GetVolume();
private:
	std::string name;
	FMOD_VECTOR position;
	FMOD::Sound* sound;
	float volume;
};

namespace AudioManager {
	void Init();
	void CleanUp();

	void AddSound(const char* Path, std::string Name, glm::vec3 Position, float distance, float Volume);
	Sound* GetSound(std::string name);
	void Update();
	void UpdateListener(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Velocity);
	FMOD::System* GetSystem();

	int PlaySound(std::string sound);
	int PlaySound(std::string sound,glm::vec3 Position);
	int PlaySound(std::string sound, glm::vec3 Position, int channel);

	int PlaySound(std::string sound, int channel);

	bool IsChannelPlaying(int channel);

	void PauseSound(std::string sound);
	void StopSound(std::string sound);
}

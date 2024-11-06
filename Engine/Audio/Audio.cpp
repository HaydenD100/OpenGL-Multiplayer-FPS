#include "Audio.h"

Sound::Sound(const char* Path, std::string Name, glm::vec3 Position, float Distance, float Volume) {
	position.x = Position.x;
	position.y = Position.y;
	position.z = Position.z;
	name = Name;
	FMOD_RESULT result = AudioManager::GetSystem()->createSound(Path, FMOD_3D, 0, &sound);
	if (result != FMOD_OK) {
		std::cerr << result << " " << FMOD_ErrorString(result) << std::endl;
	}
	result = sound->set3DMinMaxDistance(0.5f, Distance);
	if (result != FMOD_OK) {
		std::cerr << result << " " << FMOD_ErrorString(result) << std::endl;
	}
	volume = Volume;
}

FMOD::Sound* Sound::GetSound() {
	return sound;
}

void Sound::SetVolume(float Volume) {
	volume = Volume;
}

float Sound::GetVolume() {
	return volume;
}

FMOD_VECTOR* Sound::GetPositionFmod() {
	return &position;
}

void Sound::SetPosition(glm::vec3 Position) {
	position.x = Position.x;
	position.y = Position.y;
	position.z = Position.z;
}

std::string Sound::GetName() {
	return name;
}

namespace AudioManager {
	std::vector<Sound> sounds;
	std::vector<FMOD::Channel*> channels;
	int channelSize = 50;

	const float DISTANCEFACTOR = 1.0f;

	//Listener
	FMOD_VECTOR listenerpos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

	FMOD_RESULT result;
	FMOD::System* system = nullptr;
	FMOD::ChannelGroup* channelGroup = nullptr;

	bool succeededOrWarn(const std::string& message, FMOD_RESULT result) {
		if (result != FMOD_OK) {
			std::cerr << message << ": " << result << " " << FMOD_ErrorString(result) << std::endl;
			return false;
		}
		return true;
	}

	void AudioManager::Init() {
		for (int i = 0; i < channelSize; i++) {
			channels.push_back(0);
		}
		result = FMOD::System_Create(&system);
		if (!succeededOrWarn("FMOD: Failed to create system object", result))
			return;
		result = system->init(512, FMOD_INIT_NORMAL, nullptr);
		if (!succeededOrWarn("FMOD: Failed to create system object", result))
			return;
		result = system->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
		if (!succeededOrWarn("FMOD: Failed to create system object", result))
			return;
		result = system->createChannelGroup("channels", &channelGroup);
		if (!succeededOrWarn("FMOD: Failed to create system object", result))
			return;
		
		//thanks to livinamuk for a couple of these sounds
		AudioManager::AddSound("Assets/Audio/door_close.wav", "door_close", glm::vec3(0, 0, 0), 10, 1);
		AudioManager::AddSound("Assets/Audio/door_open.wav", "door_open", glm::vec3(0, 0, 0), 10, 0.8);
		AudioManager::AddSound("Assets/Audio/player_step_1.wav", "foot_step1", glm::vec3(0, 0, 0), 10, 0.3);
		AudioManager::AddSound("Assets/Audio/player_step_2.wav", "foot_step2", glm::vec3(0, 0, 0), 10, 0.3);
		AudioManager::AddSound("Assets/Audio/player_step_3.wav", "foot_step3", glm::vec3(0, 0, 0), 10, 0.3);
		AudioManager::AddSound("Assets/Audio/player_step_4.wav", "foot_step4", glm::vec3(0, 0, 0), 10, 0.3);
		AudioManager::AddSound("Assets/Audio/item_pick_up.wav", "item_pickup", glm::vec3(0, 0, 0), 10, 0.8);
		AudioManager::AddSound("Assets/Audio/glass_impact1.wav", "glass_impact1", glm::vec3(0, 0, 0), 10, 0.8);
		AudioManager::AddSound("Assets/Audio/glass_impact2.wav", "glass_impact2", glm::vec3(0, 0, 0), 10, 0.8);

		AudioManager::AddSound("Assets/Audio/spray_paint.wav", "spray_paint", glm::vec3(0, 0, 0), 10, 1);

		AudioManager::AddSound("Assets/Audio/bullet_impact_00.wav", "bullet_impact_0", glm::vec3(0, 0, 0), 6, 0.2);
		AudioManager::AddSound("Assets/Audio/bullet_impact_01.wav", "bullet_impact_1", glm::vec3(0, 0, 0),	6, 0.2);
		AudioManager::AddSound("Assets/Audio/bullet_impact_02.wav", "bullet_impact_2", glm::vec3(0, 0, 0), 6, 0.2);
		AudioManager::AddSound("Assets/Audio/bullet_impact_03.wav", "bullet_impact_3", glm::vec3(0, 0, 0), 6, 0.2);
		AudioManager::AddSound("Assets/Audio/bullet_impact_04.wav", "bullet_impact_4", glm::vec3(0, 0, 0), 6, 0.2);


	}

	void AudioManager::CleanUp() {
		channelGroup->release();
		system->release();
	}
	
	FMOD::System* AudioManager::GetSystem() {
		return system;
	}

	void AudioManager::AddSound(const char* Path, std::string Name, glm::vec3 Position, float distance, float Volume) {
		sounds.push_back(Sound(Path,Name,Position,distance,Volume));
	}
	
	Sound* AudioManager::GetSound(std::string name) {
		for (int i = 0; i < sounds.size(); i++) {
			if (sounds[i].GetName() == name)
				return &sounds[i];
		}
		return nullptr;
	}
	
	void UpdateListener(glm::vec3 Position, glm::vec3 Forward,glm::vec3 Velocity) {
		listenerpos.x = Position.x;
		listenerpos.y = Position.y;
		listenerpos.z = Position.z;

		forward.x = Forward.x;
		forward.y = Forward.y;
		forward.z = Forward.z;

		vel.x = Velocity.x;
		vel.y = Velocity.y;
		vel.z = Velocity.z;


		result = system->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);
		//if (!succeededOrWarn("FMOD: Failed to create system object", result))
			//return;

	}
	
	void AudioManager::Update() {
		system->update();
	}
	
	bool AudioManager::IsChannelPlaying(int channel) {
		if (channel < 0 || channel > channelSize)
			return false;
		bool isPlaying = false;
		channels[channel]->isPlaying(&isPlaying);
		return isPlaying;
	}

	int AudioManager::PlaySound(std::string sound) {
		bool isPlaying = false;
		for (int i = 0; i < channelSize; i++) {
			channels[i]->isPlaying(&isPlaying);
			if (!isPlaying) {
				
				result = system->playSound(GetSound(sound)->GetSound(), 0, true, &channels[i]);
				if (!succeededOrWarn("FMOD2: ", result))
					return Sound::DEFAULT;
				result = channels[i]->set3DAttributes(GetSound(sound)->GetPositionFmod(), &vel);
				if (!succeededOrWarn("FMOD3: ", result))
					return Sound::DEFAULT;
				result = channels[i]->setVolume(GetSound(sound)->GetVolume());
				if (!succeededOrWarn("FMOD1: ", result))
					return Sound::DEFAULT;
				result = channels[i]->setPaused(false);
				if (!succeededOrWarn("FMOD4: ", result))
					return Sound::DEFAULT;


				return i;
			}
		}		
	}
	
	int PlaySound(std::string sound, glm::vec3 Position) {
		bool isPlaying = false;
		if (GetSound(sound) == nullptr)
			return  Sound::DEFAULT;

		for (int i = 0; i < channelSize; i++) {
			channels[i]->isPlaying(&isPlaying);
			if (!isPlaying) {
				result = system->playSound(GetSound(sound)->GetSound(), 0, true, &channels[i]);
				if (!succeededOrWarn("Error1", result))
					return Sound::DEFAULT;
				GetSound(sound)->SetPosition(Position);


				FMOD_VECTOR position;
				position.x = Position.x;
				position.y = Position.y;
				position.z = Position.z;

				result = channels[i]->set3DAttributes(&position, &vel);
				if (!succeededOrWarn("Error2", result))
					return Sound::DEFAULT;
				result = channels[i]->setPaused(false);
				if (!succeededOrWarn("Error3", result))
					return Sound::DEFAULT;
				channels[i]->setVolume(GetSound(sound)->GetVolume());
				if (!succeededOrWarn("Error4", result))
					return Sound::DEFAULT;
				
				return i;
			}
		}
		
		return Sound::DEFAULT;
	}
	
	int AudioManager::PlaySound(std::string sound, glm::vec3 Position, int channel) {
		FMOD_VECTOR position;
		position.x = Position.x;
		position.y = Position.y;
		position.z = Position.z;

		channels[channel]->setVolume(GetSound(sound)->GetVolume());
		result = system->playSound(GetSound(sound)->GetSound(), 0, true, &channels[channel]);
		result = channels[channel]->set3DAttributes(&position, &vel);
		result = channels[channel]->setPaused(false);
		
		if (!succeededOrWarn("Error", result))
			return Sound::DEFAULT;



		return channel;
	}

	int AudioManager::PlaySound(std::string sound, int channel) {
		channels[channel]->setVolume(GetSound(sound)->GetVolume());
		result = system->playSound(GetSound(sound)->GetSound(), 0, true, &channels[channel]);
		result = channels[channel]->set3DAttributes(GetSound(sound)->GetPositionFmod(), &vel);
		result = channels[channel]->setPaused(false);
		if (!succeededOrWarn("Error", result))
			return Sound::DEFAULT;
		return channel;
	}

	void AudioManager::PauseSound(std::string sound) {

	}
	
	void AudioManager::StopSound(std::string sound) {

	}
}

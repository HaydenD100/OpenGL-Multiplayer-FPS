#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <queue>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#undef PlaySound
#undef near
#undef far


enum PacketType : uint8_t {
	MESSAGE = 0,
	PlAYERDATA = 1,
	SOUND = 2,
	OBJECTPOSTIONS = 4,
	CONTROL = 8,
	GUNSHOT = 16,
	DYNAMICOBJECT = 32,
	ANIMATION = 64,

};

struct Packet {
	//type of packet for payload 
	uint8_t type;
	//size of playload
	uint32_t size;

	// Define payload types
	struct PlayerData {
		float x;
		float y;
		float z;
		float rotation_x;
		float rotation_y;
		float rotation_z;

		uint8_t interactingWithSize;
		uint8_t currentGunSize;

		char interactingWith[128];
		char currentGun[64];

	};

	struct MessagePayload {
		char message[256];  // Fixed-size buffer for the message
	};

	struct AnimationData {
		uint8_t AnimationNameSize;
		uint8_t ObjectNameSize;

		char AnimationName[64];
		char ObjectName[128];
	};

	struct GunShotData {
		uint8_t ObjectNameSize;
		char ObjectName[128];
		uint8_t DecalNameSize;
		char DecalName[128];

		float worldhitpoint_x;
		float worldhitpoint_y;
		float worldhitpoint_z;

		float hitpointNormal_x;
		float hitpointNormal_y;
		float hitpointNormal_z;
		
		int32_t Damage;

		float force_x;
		float force_y;
		float force_z;

		float hitpointLocal_x;
		float hitpointLocal_y;
		float hitpointLocal_z;

	};

	union Payload {
		PlayerData player;
		MessagePayload message;
		AnimationData animation;
		GunShotData gunshotdata;
		
		Payload() {}
	} payload;
};

namespace NetworkManager
{
	int Init();
	void CleanUp();

	void LoadedIn();

	int InitServer();
	int RunServer();

	int CheckForDataFromClient();
	int SendDataToClient(const char* buffer);
	int WaitForClient();

	int CheckForDataFromServer();
	int SendDataToServer(const char* buffer);

	int RunClient();
	int InitClient();

	int IsServer();

	int SendPackets();
	void ReceivePackets(char recvbuf[DEFAULT_BUFLEN]);

	//packet sending

	void SendGunShotData(std::string objectname, std::string decalName, glm::vec3 worldhitpoint, glm::vec3 hitpointnormal, glm::vec3 hitpointlocal, int32_t damage, glm::vec3 force);
	void SendAnimation(std::string AnimationName, std::string ObjectName);
	void SendPlayerData(glm::vec3 postion, glm::vec3 rotation, std::string currentGun, std::string interactingWith);
	void SendPacketMessage(std::string message);

	void EvaulatePackets();

};


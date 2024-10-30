#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Core/AssetManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <queue>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

enum PacketType : uint8_t {
	PlAYERDATA = 1,
	SOUND = 2,
	OBJECTPOSTIONS = 4,
	CONTROL = 8,
	MESSAGE = 16
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
	};

	struct MessagePayload {
		char message[256];  // Fixed-size buffer for the message
	};

	union Payload {
		PlayerData player;
		MessagePayload message;
		Payload() {}
	} payload;
};

namespace NetworkManager
{
	int Init();
	void CleanUp();

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

	//packet sending
	void SendPlayerData(glm::vec3 postion, glm::vec3 rotation);
	void SendPacketMessage(std::string message);

	void EvaulatePackets();

};


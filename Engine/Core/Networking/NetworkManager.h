#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

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
	PlAYERPOSTION = 0b00000001,
	SOUND = 0b00000010,
	OBJECTPOSTIONS = 0b00000100,
	CONTROL = 0b10000000,
	MESSAGE = 0b00001000
};

struct Packet {
	//type of packet for payload 
	uint8_t type;
	//size of playload
	uint32_t size;

	// Define payload types
	struct PlayerPosition {
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
		PlayerPosition position;
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
	Packet SendPacketMessage(const char* message);

};


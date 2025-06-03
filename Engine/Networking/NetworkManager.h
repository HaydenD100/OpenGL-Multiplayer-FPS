#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Engine/Core/Common/Header.h"
#include "Engine/Core/Common.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#undef PlaySound
#undef near
#undef far


//TODO :: code is not mem safe gotta fix that
enum CODES : uint16_t {
	DISCONNECTED = 0,
	CONNECTED = 1,
	POSITION = 2,
	MESSAGE = 3
};
struct Packet {
	uint16_t ID;
	uint16_t code;
	char data[512];

	const char* Encode() {
		static char buffer[516]; // 2 + 2 + 512

		// Encode ID
		buffer[0] = ID & 0xFF;
		buffer[1] = (ID >> 8) & 0xFF;

		// Encode code
		buffer[2] = code & 0xFF;
		buffer[3] = (code >> 8) & 0xFF;

		// Copy data
		std::memcpy(buffer + 4, data, sizeof(data));

		return buffer;
	}
};

struct PlayerN
{
	uint16_t ID = 0;
	Transform transform;

};

namespace Client
{
	extern bool IsConnected;
	void Init(const char* IP);
	void CleanUp();
	void Run();
	
	void SendWorldPosition(glm::vec3 position, glm::vec3 rotation);
	void SendData(char buf[1024]);
};


#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

namespace Client
{
	extern bool IsConnected;
	void Init(const char* IP);
	void CleanUp();
	void Run();
	
	void SendWorldPosition(glm::vec3 position, glm::vec3 rotation);
	void SendData(char buf[1024]);
};


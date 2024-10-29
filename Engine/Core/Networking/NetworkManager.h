#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

namespace NetworkManager
{
	int Init();
	void CleanUp();

	int InitServer();
	//dont use this for now ill add this back with multithreading
	int RunServer();

	int CheckForDataFromClient();
	int SendDataToClient(std::string message);
	int WaitForClient();

	int CheckForDataFromServer();
	int SendDataToServer(std::string message);

	int RunClient(SOCKET ConnectSocket);
	int InitClient();

	int IsServer();



};


#include "NetworkManager.h"


namespace NetworkManager
{
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ConnectSocket = INVALID_SOCKET;
	int isServer = 0;
	int isRunning = 1;

	std::thread netowrking_thread;

	std::queue<Packet> out;
	std::queue<Packet> in;

	int Init() {
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}

	}
	void CleanUp() {
		isRunning = 0;
		if (!isServer) {
			// shutdown the connection for sending since no more data will be sent
			// the client can still use the ConnectSocket for receiving data
			iResult = shutdown(ConnectSocket, SD_SEND);
		}
		netowrking_thread.join();
		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	}


	int RunServer() {
		std::cout << "Server initialized \n";

		WaitForClient();

		char recvbuf[DEFAULT_BUFLEN];
		int iResult, iSendResult;
		int recvbuflen = DEFAULT_BUFLEN;

		// Receive until the peer shuts down the connection
		do {
			memset(recvbuf, 0, sizeof(recvbuf));
			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				Packet packet;

				switch (recvbuf[0])
				{
				case MESSAGE:
					packet.type = MESSAGE;
					std::memcpy(&packet.size, recvbuf + sizeof(packet.type), 2);
					std::memcpy(&packet.payload.message.message, recvbuf + sizeof(packet.type) + sizeof(packet.size), packet.size);

					in.push(packet);
					/*
					std::cout << "-------------MESSAGE FROM SERVER--------------- \n";
					std::cout << (int)packet.type << "\n";
					std::cout << (int)packet.size << "\n";
					std::cout << packet.payload.message.message << "\n";
					*/

					break;
				case PlAYERDATA:
					packet.type = PlAYERDATA;
					std::memcpy(&packet.size, recvbuf + sizeof(packet.type), sizeof(packet.size));

					//get floats
					std::memcpy(&packet.payload.player.x, recvbuf + sizeof(packet.type) + sizeof(packet.size), sizeof(float));
					std::memcpy(&packet.payload.player.y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 1, sizeof(float));
					std::memcpy(&packet.payload.player.z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 2, sizeof(float));

					std::memcpy(&packet.payload.player.rotation_x, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 3, sizeof(float));
					std::memcpy(&packet.payload.player.rotation_y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 4, sizeof(float));
					std::memcpy(&packet.payload.player.rotation_z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 5, sizeof(float));

					in.push(packet);

					/*
					std::cout << "-------------MESSAGE FROM SERVER--------------- \n";
					std::cout << (int)packet.type << "\n";
					std::cout << (int)packet.size << "\n";
					std::cout << "x: " << packet.payload.player.x << " y: " << packet.payload.player.y << " z:" << packet.payload.player.z << "\n";
					*/

					break;
				default:
					break;
				}
				

			}
		} while (isRunning);


		// shutdown the send half of the connection since no more data will be sent
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	}


	int InitServer() {
		isServer = 1;
		std::cout << "starting server \n";
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		ListenSocket = INVALID_SOCKET;
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (ListenSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			printf("Listen failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1; 
		}

		netowrking_thread = std::thread(RunServer);
	}


	int WaitForClient() {
		ClientSocket = INVALID_SOCKET;

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
	}

	int RunClient() {
		std::cout << "Starting Client \n";

		int recvbuflen = DEFAULT_BUFLEN;
		char message[DEFAULT_BUFLEN] = "Client Connected to server";
		char recvbuf[DEFAULT_BUFLEN];

		int iResult;

		// Send an initial buffer
		//iResult = send(ConnectSocket, message.c_str(), (int)strlen(message.c_str()), 0);
		//printf("Bytes Sent: %ld\n", iResult);

		iResult = SendDataToServer(message);

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Receive data until the server closes the connection
		while (isRunning) {
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				Packet packet;

				switch (recvbuf[0])
				{
				case MESSAGE:
					packet.type = MESSAGE;
					std::memcpy(&packet.size, recvbuf + sizeof(packet.type), 2);
					std::memcpy(&packet.payload.message.message, recvbuf + sizeof(packet.type) + sizeof(packet.size), packet.size);

					in.push(packet);
					/*
					std::cout << "-------------MESSAGE FROM SERVER--------------- \n";
					std::cout << (int)packet.type << "\n";
					std::cout << (int)packet.size << "\n";
					std::cout << packet.payload.message.message << "\n";
					*/

					break;
				case PlAYERDATA:
					packet.type = PlAYERDATA;
					std::memcpy(&packet.size, recvbuf + sizeof(packet.type), sizeof(packet.size));

					//get floats
					std::memcpy(&packet.payload.player.x, recvbuf + sizeof(packet.type) + sizeof(packet.size), sizeof(float));
					std::memcpy(&packet.payload.player.y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 1, sizeof(float));
					std::memcpy(&packet.payload.player.z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 2, sizeof(float));

					std::memcpy(&packet.payload.player.rotation_x, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 3, sizeof(float));
					std::memcpy(&packet.payload.player.rotation_y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 4, sizeof(float));
					std::memcpy(&packet.payload.player.rotation_z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 5, sizeof(float));

					in.push(packet);

					/*
					std::cout << "-------------MESSAGE FROM SERVER--------------- \n";
					std::cout << (int)packet.type << "\n";
					std::cout << (int)packet.size << "\n";
					std::cout << "x: " << packet.payload.player.x << " y: " << packet.payload.player.y << " z:" << packet.payload.player.z << "\n";
					*/

					break;
				default:
					break;
				}
			}

		}

	}

	int InitClient() {
		isServer = 0;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		char hostname[256];


		// Get the local host name
		if (gethostname(hostname, sizeof(hostname)) != 0) {
			fprintf(stderr, "gethostname failed.\n");
			WSACleanup();
			return 1;
		}

		// Resolve the server address and port
		//change hostname to the servers ip
		iResult = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}
		ConnectSocket = INVALID_SOCKET;

		// Attempt to connect to the first address returned by
		// the call to getaddrinfo
		ptr = result;
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
		// Should really try the next address returned by getaddrinfo
		// if the connect call failed
		// But for this simple example we just free the resources
		// returned by getaddrinfo and print an error message
		freeaddrinfo(result);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return 1;
		}

		std::cout << "Connected to server \n";
		netowrking_thread = std::thread(RunClient);
	}



	int CheckForDataFromClient() {
		char recvbuf[DEFAULT_BUFLEN];
		int iResult, iSendResult;
		int recvbuflen = DEFAULT_BUFLEN;

		memset(recvbuf, 0, sizeof(recvbuf));
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			std::cout << "Client: " << recvbuf << "\n";

			if (std::strcmp(recvbuf, "client_disconnect") == 0) {
				printf("client_disconnect");
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}



	}
	int SendDataToClient(const char* buffer) {
		int recvbuflen = DEFAULT_BUFLEN;
		int iResult;
		iResult = send(ClientSocket, buffer, recvbuflen, 0);

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		return iResult;
	}
	int SendDataToServer(const char* buffer) {
		int recvbuflen = DEFAULT_BUFLEN;
		int iResult;

		// Send an initial buffer
		iResult = send(ConnectSocket, buffer, recvbuflen, 0);
		printf("Bytes Sent: %ld\n", iResult);

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		return iResult;
	}

	/*
	int CheckForDataFromServer() {
		int iResult;
		int recvbuflen = DEFAULT_BUFLEN;
		char recvbuf[DEFAULT_BUFLEN];
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	}
	
	*/
	int IsServer() {
		return isServer;
	}

	int SendPackets() {
		while(out.size() > 0){
			Packet packet = out.front();
			out.pop();
			char sendbuf[DEFAULT_BUFLEN] = {0};
			std::memcpy(sendbuf, &packet.type, sizeof(packet.type));
			std::memcpy(sendbuf + sizeof(packet.type), &packet.size, sizeof(packet.size));
			switch (packet.type)
			{
			case MESSAGE:
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size),packet.payload.message.message, packet.size);

				break;
			case PlAYERDATA:
				// Calculate base offset for the payload
				// Copy player position coordinates to send buffer
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size), &packet.payload.player.x, sizeof(float));         // Offset for x
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float), &packet.payload.player.y, sizeof(float));     // Offset for y
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 2, &packet.payload.player.z, sizeof(float));     // Offset for z
				// Copy player rotation values to send buffer
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 3, &packet.payload.player.rotation_x, sizeof(float)); // Offset for rotation_x
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 4, &packet.payload.player.rotation_y, sizeof(float)); // Offset for rotation_y
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 5, &packet.payload.player.rotation_z, sizeof(float)); // Offset for rotation_z
				break;

			default:
				break;
			}


			if (isServer)
				SendDataToClient(sendbuf);

			else
				SendDataToServer(sendbuf);
		}

		return 0;
	}

	void SendPlayerData(glm::vec3 postion, glm::vec3 rotation) {
		Packet temp;
		temp.type = PlAYERDATA;
		temp.size = sizeof(float) * 6;
		temp.payload.player.x = postion.x;
		temp.payload.player.y = postion.y;
		temp.payload.player.z = postion.z;

		temp.payload.player.rotation_x = rotation.x;
		temp.payload.player.rotation_y = rotation.y;
		temp.payload.player.rotation_z = rotation.z;

		out.push(temp);
	}

	void SendPacketMessage(std::string message) {
		Packet temp;
		temp.type = MESSAGE;
		temp.size = static_cast<uint32_t>(message.size());
		strcpy(temp.payload.message.message, message.c_str());
		out.push(temp);
	}

	void EvaulatePackets() {
		//this is so it wont evaulate any more packets coming in
		int current_in_size = in.size();
		while (current_in_size > 0) {
			current_in_size--;
			Packet packet = in.front();
			in.pop();
			switch (packet.type)
			{
			case MESSAGE:

				break;
			case PlAYERDATA:
				AssetManager::GetGameObject("PlayerTwo")->setPosition(glm::vec3(packet.payload.player.x, packet.payload.player.y, packet.payload.player.z));
				break;

			default:
				break;
			}
		}
	}


};

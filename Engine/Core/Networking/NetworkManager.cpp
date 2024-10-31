#include "NetworkManager.h"
#include "Engine/Core/SkinnedAnimatior.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"


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
	int loadedIn = 0;
	int clientConnected = 0;

	void LoadedIn() {
		loadedIn = 1;
		if(!isServer)
			SendControl(CONNECTED);
	}

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
		if (!isServer) {
			// shutdown the connection for sending since no more data will be sent
			// the client can still use the ConnectSocket for receiving data
			SendControl(DISCONNECTED);
			iResult = shutdown(ConnectSocket, SD_SEND);
		}
		isRunning = 0;
		netowrking_thread.join();
		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	}


	int RunServer() {
		std::cout << "\n =================== Server initialized =================== \n";

		WaitForClient();
		char recvbuf[DEFAULT_BUFLEN];
		int iResult, iSendResult;
		int recvbuflen = DEFAULT_BUFLEN;

		// Receive until the peer shuts down the connection
		do {
			memset(recvbuf, 0, sizeof(recvbuf));
			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				ReceivePackets(recvbuf);
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
		std::cout << "\n===================== Waiting for Client to Connect =====================\n";

		while (ClientSocket == INVALID_SOCKET) {
			ClientSocket = accept(ListenSocket, NULL, NULL);
		}
		std::cout << "\n===================== Client Connected =====================\n";
		clientConnected = 1;
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
	}

	int RunClient() {
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
				ReceivePackets(recvbuf);
			}

		}

	}

	int InitClient(char hostname[256]) {
		isServer = 0;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;


		// Get the local host name if no ip was givving
		if (hostname[0] == NULL) {
			if (gethostname(hostname, sizeof(hostname)) != 0) {
				fprintf(stderr, "gethostname failed.\n");
				WSACleanup();
				return 1;
			}
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
		std::cout << "\n =============== Connected to server ===============\n";
		netowrking_thread = std::thread(RunClient);
	}


	//Redundant
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
			//printf("send failed: %d\n", WSAGetLastError());
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

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		return iResult;
	}

	int IsServer() {
		return isServer;
	}

	int SendPackets() {

		if (isServer && !clientConnected)
			return 1;

		while(out.size() > 0){
			Packet packet = out.front();
			out.pop();
			char sendbuf[DEFAULT_BUFLEN] = {0};
			std::memcpy(sendbuf, &packet.type, sizeof(packet.type));
			std::memcpy(sendbuf + sizeof(packet.type), &packet.size, sizeof(packet.size));

			size_t offset;
			switch (packet.type)
			{
			case MESSAGE:
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size),packet.payload.message.message, packet.size);

				break;
			case PlAYERDATA:
				// Calculate base offset for the payload
				// Copy player position coordinates to send buffer
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size), &packet.payload.player.x, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float), &packet.payload.player.y, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 2, &packet.payload.player.z, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 3, &packet.payload.player.rotation_x, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 4, &packet.payload.player.rotation_y, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 5, &packet.payload.player.rotation_z, sizeof(float));

				// Copy interactingWithSize and currentGunSize
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6, &packet.payload.player.interactingWithSize, sizeof(uint8_t));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + sizeof(uint8_t), &packet.payload.player.currentGunSize, sizeof(uint8_t));

				// Copy interactingWith and currentGun strings based on calculated offsets
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + sizeof(uint8_t) * 2, packet.payload.player.interactingWith, packet.payload.player.interactingWithSize);
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + sizeof(uint8_t) * 2 + packet.payload.player.interactingWithSize, packet.payload.player.currentGun, packet.payload.player.currentGunSize);

				break;
			case ANIMATION:
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size), &packet.payload.animation.AnimationNameSize, sizeof(float));
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + 1, &packet.payload.animation.ObjectNameSize, sizeof(float));

				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + 2, &packet.payload.animation.AnimationName, packet.payload.animation.AnimationNameSize);
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size) + 2 + packet.payload.animation.AnimationNameSize, &packet.payload.animation.ObjectName, packet.payload.animation.ObjectNameSize);
				break;

			case GUNSHOT:
				offset = sizeof(packet.type) + sizeof(packet.size);

				// Copy ObjectNameSize
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.ObjectNameSize, sizeof(uint8_t));
				offset += sizeof(uint8_t);

				// Copy ObjectName
				std::memcpy(sendbuf + offset, packet.payload.gunshotdata.ObjectName, packet.payload.gunshotdata.ObjectNameSize);
				offset += packet.payload.gunshotdata.ObjectNameSize;

				// Copy DecalNameSize
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.DecalNameSize, sizeof(uint8_t));
				offset += sizeof(uint8_t);

				// Copy DecalName
				std::memcpy(sendbuf + offset, packet.payload.gunshotdata.DecalName, packet.payload.gunshotdata.DecalNameSize);
				offset += packet.payload.gunshotdata.DecalNameSize;

				// Copy world hit point coordinates
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.worldhitpoint_x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.worldhitpoint_y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.worldhitpoint_z, sizeof(float));
				offset += sizeof(float);

				// Copy hit point normal coordinates
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointNormal_x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointNormal_y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointNormal_z, sizeof(float));
				offset += sizeof(float);

				// Copy damage
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.Damage, sizeof(int32_t));
				offset += sizeof(int32_t);

				// Copy force
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.force_x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.force_y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.force_z, sizeof(float));
				offset += sizeof(float);

				// Copy hit point local coordinates
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointLocal_x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointLocal_y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.gunshotdata.hitpointLocal_z, sizeof(float));
				offset += sizeof(float);

				break;

			case DYNAMICOBJECT:
				// Copy packet type
				offset = sizeof(packet.type) + sizeof(packet.size);

				// Copy ObjectNameSize and ObjectName
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.ObjectNameSize, sizeof(packet.payload.dynamicObjectData.ObjectNameSize));
				offset += sizeof(packet.payload.dynamicObjectData.ObjectNameSize);

				std::memcpy(sendbuf + offset, packet.payload.dynamicObjectData.ObjectName, packet.payload.dynamicObjectData.ObjectNameSize);
				offset += packet.payload.dynamicObjectData.ObjectNameSize;

				// Copy position data
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.z, sizeof(float));
				offset += sizeof(float);

				// Copy rotation data
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.rotation_x, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.rotation_y, sizeof(float));
				offset += sizeof(float);
				std::memcpy(sendbuf + offset, &packet.payload.dynamicObjectData.rotation_z, sizeof(float));

				break;

			case CONTROL:
				std::memcpy(sendbuf + sizeof(packet.type) + sizeof(packet.size), &packet.payload.control.flag, 1);
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
	void ReceivePackets(char recvbuf[DEFAULT_BUFLEN]) {
		Packet packet;
		size_t offset = 0;

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

			// Get floats
			std::memcpy(&packet.payload.player.x, recvbuf + sizeof(packet.type) + sizeof(packet.size), sizeof(float));
			std::memcpy(&packet.payload.player.y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float), sizeof(float));
			std::memcpy(&packet.payload.player.z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 2, sizeof(float));
			std::memcpy(&packet.payload.player.rotation_x, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 3, sizeof(float));
			std::memcpy(&packet.payload.player.rotation_y, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 4, sizeof(float));
			std::memcpy(&packet.payload.player.rotation_z, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 5, sizeof(float));

			// Get interactingWithSize and currentGunSize
			std::memcpy(&packet.payload.player.interactingWithSize, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6, 1);
			std::memcpy(&packet.payload.player.currentGunSize, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + 1, 1);

			// Get strings based on size
			std::memcpy(packet.payload.player.interactingWith, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + 2, packet.payload.player.interactingWithSize);
			std::memcpy(packet.payload.player.currentGun, recvbuf + sizeof(packet.type) + sizeof(packet.size) + sizeof(float) * 6 + 2 + packet.payload.player.interactingWithSize, packet.payload.player.currentGunSize);

			// Ensure strings are null-terminated
			packet.payload.player.interactingWith[packet.payload.player.interactingWithSize] = '\0';
			packet.payload.player.currentGun[packet.payload.player.currentGunSize] = '\0';
			in.push(packet);

			/*
			std::cout << "-------------MESSAGE FROM SERVER--------------- \n";
			std::cout << (int)packet.type << "\n";
			std::cout << (int)packet.size << "\n";
			std::cout << "x: " << packet.payload.player.x << " y: " << packet.payload.player.y << " z:" << packet.payload.player.z << "\n";
			std::cout << packet.payload.player.interactingWith << "\n";
			*/
			break;
			
		case ANIMATION:
			packet.type = ANIMATION;
			offset = sizeof(packet.type);

			// Extract packet size
			std::memcpy(&packet.size, recvbuf + offset, sizeof(packet.size));
			offset += sizeof(packet.size);

			// Extract `AnimationNameSize` and `ObjectNameSize`
			std::memcpy(&packet.payload.animation.AnimationNameSize, recvbuf + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);
			std::memcpy(&packet.payload.animation.ObjectNameSize, recvbuf + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);

			// Extract `AnimationName` and `ObjectName` strings based on their sizes
			std::memcpy(packet.payload.animation.AnimationName, recvbuf + offset, packet.payload.animation.AnimationNameSize);
			packet.payload.animation.AnimationName[packet.payload.animation.AnimationNameSize] = '\0'; // Null-terminate
			offset += packet.payload.animation.AnimationNameSize;

			std::memcpy(packet.payload.animation.ObjectName, recvbuf + offset, packet.payload.animation.ObjectNameSize);
			packet.payload.animation.ObjectName[packet.payload.animation.ObjectNameSize] = '\0'; // Null-terminate

			in.push(packet);

			break;
		case GUNSHOT:
			packet.type = GUNSHOT;
			offset = sizeof(packet.type);

			// Deserialize packet size
			std::memcpy(&packet.size, recvbuf + offset, sizeof(packet.size));
			offset += sizeof(packet.size);

			// Deserialize ObjectNameSize
			std::memcpy(&packet.payload.gunshotdata.ObjectNameSize, recvbuf + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);

			// Deserialize ObjectName
			std::memcpy(packet.payload.gunshotdata.ObjectName, recvbuf + offset, packet.payload.gunshotdata.ObjectNameSize);
			packet.payload.gunshotdata.ObjectName[packet.payload.gunshotdata.ObjectNameSize] = '\0'; // Null-terminate
			offset += packet.payload.gunshotdata.ObjectNameSize;

			// Deserialize DecalNameSize
			std::memcpy(&packet.payload.gunshotdata.DecalNameSize, recvbuf + offset, sizeof(uint8_t));
			offset += sizeof(uint8_t);

			// Deserialize DecalName
			std::memcpy(packet.payload.gunshotdata.DecalName, recvbuf + offset, packet.payload.gunshotdata.DecalNameSize);
			packet.payload.gunshotdata.DecalName[packet.payload.gunshotdata.DecalNameSize] = '\0'; // Null-terminate
			offset += packet.payload.gunshotdata.DecalNameSize;

			// Deserialize world hit point coordinates
			std::memcpy(&packet.payload.gunshotdata.worldhitpoint_x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.worldhitpoint_y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.worldhitpoint_z, recvbuf + offset, sizeof(float));
			offset += sizeof(float);

			// Deserialize hit point normal coordinates
			std::memcpy(&packet.payload.gunshotdata.hitpointNormal_x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.hitpointNormal_y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.hitpointNormal_z, recvbuf + offset, sizeof(float));
			offset += sizeof(float);

			// Deserialize damage
			std::memcpy(&packet.payload.gunshotdata.Damage, recvbuf + offset, sizeof(int32_t));
			offset += sizeof(int32_t);

			// Deserialize force vector
			std::memcpy(&packet.payload.gunshotdata.force_x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.force_y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.force_z, recvbuf + offset, sizeof(float));
			offset += sizeof(float);

			// Deserialize hit point local coordinates
			std::memcpy(&packet.payload.gunshotdata.hitpointLocal_x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.hitpointLocal_y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.gunshotdata.hitpointLocal_z, recvbuf + offset, sizeof(float));


			offset += sizeof(float);

			// Push packet to the input queue
			in.push(packet);
			/*
			std::cout << "------------- GUNSHOT DATA ---------------\n";
			std::cout << "Object Name: " << packet.payload.gunshotdata.ObjectName << " (Size: " << (int)packet.payload.gunshotdata.ObjectNameSize << ")\n";
			std::cout << "Decal Name: " << packet.payload.gunshotdata.DecalName << " (Size: " << (int)packet.payload.gunshotdata.DecalNameSize << ")\n";
			std::cout << "World Hit Point: ("
				<< packet.payload.gunshotdata.worldhitpoint_x << ", "
				<< packet.payload.gunshotdata.worldhitpoint_y << ", "
				<< packet.payload.gunshotdata.worldhitpoint_z << ")\n";
			std::cout << "Hit Point Normal: ("
				<< packet.payload.gunshotdata.hitpointNormal_x << ", "
				<< packet.payload.gunshotdata.hitpointNormal_y << ", "
				<< packet.payload.gunshotdata.hitpointNormal_z << ")\n";
			std::cout << "Damage: " << packet.payload.gunshotdata.Damage << "\n";
			std::cout << "Force: ("
				<< packet.payload.gunshotdata.force_x << ", "
				<< packet.payload.gunshotdata.force_y << ", "
				<< packet.payload.gunshotdata.force_z << ")\n";
			std::cout << "Local Hit Point: ("
				<< packet.payload.gunshotdata.hitpointLocal_x << ", "
				<< packet.payload.gunshotdata.hitpointLocal_y << ", "
				<< packet.payload.gunshotdata.hitpointLocal_z << ")\n";
			std::cout << "------------------------------------------\n";
			*/

			break;
		case DYNAMICOBJECT:
			std::memcpy(&packet.type, recvbuf + offset, sizeof(packet.type));
			offset = sizeof(packet.type);

			// Extract packet size
			std::memcpy(&packet.size, recvbuf + offset, sizeof(packet.size));
			offset += sizeof(packet.size);

			// Extract ObjectNameSize
			std::memcpy(&packet.payload.dynamicObjectData.ObjectNameSize, recvbuf + offset, sizeof(packet.payload.dynamicObjectData.ObjectNameSize));
			offset += sizeof(packet.payload.dynamicObjectData.ObjectNameSize);

			// Extract ObjectName based on its size
			std::memcpy(packet.payload.dynamicObjectData.ObjectName, recvbuf + offset, packet.payload.dynamicObjectData.ObjectNameSize);
			offset += packet.payload.dynamicObjectData.ObjectNameSize;

			// Extract position data
			std::memcpy(&packet.payload.dynamicObjectData.x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.dynamicObjectData.y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.dynamicObjectData.z, recvbuf + offset, sizeof(float));
			offset += sizeof(float);

			// Extract rotation data
			std::memcpy(&packet.payload.dynamicObjectData.rotation_x, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.dynamicObjectData.rotation_y, recvbuf + offset, sizeof(float));
			offset += sizeof(float);
			std::memcpy(&packet.payload.dynamicObjectData.rotation_z, recvbuf + offset, sizeof(float));

			in.push(packet);

			
			/*
			std::cout << "-------------Received Packet Info---------------\n";
			std::cout << "Object Name Size: " << static_cast<int>(packet.payload.dynamicObjectData.ObjectNameSize) << "\n";
			std::cout << "Object Name: " << packet.payload.dynamicObjectData.ObjectName << "\n";
			std::cout << "Packet Type: " << static_cast<int>(packet.type) << "\n";
			std::cout << "Packet Size: " << packet.size << "\n";
			std::cout << "Position: "
				<< "x: " << packet.payload.dynamicObjectData.x << ", "
				<< "y: " << packet.payload.dynamicObjectData.y << ", "
				<< "z: " << packet.payload.dynamicObjectData.z << "\n";

			std::cout << "Rotation: "
				<< "rotation_x: " << packet.payload.dynamicObjectData.rotation_x << ", "
				<< "rotation_y: " << packet.payload.dynamicObjectData.rotation_y << ", "
				<< "rotation_z: " << packet.payload.dynamicObjectData.rotation_z << "\n";
			std::cout << "------------------------------------------------\n";
			*/

			break;


		case CONTROL:
			packet.type = CONTROL;
			packet.size = 1;
			std::memcpy(&packet.payload.control.flag, recvbuf + 5, 1);
			in.push(packet);

		default:
			break;
		}
	}

	void SendControl(ControlFlag flag) {
		Packet packet;
		packet.type = CONTROL;
		packet.size = 1;
		packet.payload.control.flag = flag;

		out.push(packet);
	}


	void SendDyanmicObjectData(std::string objectname, glm::vec3 postion, glm::vec3 rotaion) {
		if (isServer && !clientConnected)
			return;

		Packet packet;
		packet.type = DYNAMICOBJECT;
		packet.size = objectname.size() + 6;

		packet.payload.dynamicObjectData.ObjectNameSize = static_cast<uint8_t>(objectname.size());
		std::memset(packet.payload.dynamicObjectData.ObjectName, 0, sizeof(packet.payload.dynamicObjectData.ObjectName));
		std::memcpy(packet.payload.dynamicObjectData.ObjectName, objectname.c_str(), packet.payload.dynamicObjectData.ObjectNameSize);

		packet.payload.dynamicObjectData.x = postion.x;
		packet.payload.dynamicObjectData.y = postion.y;
		packet.payload.dynamicObjectData.z = postion.z;

		packet.payload.dynamicObjectData.rotation_x = rotaion.x;
		packet.payload.dynamicObjectData.rotation_y = rotaion.y;
		packet.payload.dynamicObjectData.rotation_z = rotaion.z;

		out.push(packet);
	}

	void SendGunShotData(std::string objectname, std::string decalName, glm::vec3 worldhitpoint, glm::vec3 hitpointnormal, glm::vec3 hitpointlocal, int32_t damage, glm::vec3 force) {
		if (isServer && !clientConnected)
			return;

		Packet packet;
		packet.type = GUNSHOT;
		//this is wrong
		packet.size = 128;
		// Set ObjectName and DecalName with their respective sizes in packet.payload.gunshotdata
		packet.payload.gunshotdata.ObjectNameSize = static_cast<uint8_t>(objectname.size());
		std::memset(packet.payload.gunshotdata.ObjectName, 0, sizeof(packet.payload.gunshotdata.ObjectName));
		std::memcpy(packet.payload.gunshotdata.ObjectName, objectname.c_str(), packet.payload.gunshotdata.ObjectNameSize);

		packet.payload.gunshotdata.DecalNameSize = static_cast<uint8_t>(decalName.size());
		std::memset(packet.payload.gunshotdata.DecalName, 0, sizeof(packet.payload.gunshotdata.DecalName));
		std::memcpy(packet.payload.gunshotdata.DecalName, decalName.c_str(), packet.payload.gunshotdata.DecalNameSize);

		// Set world hit point coordinates
		packet.payload.gunshotdata.worldhitpoint_x = worldhitpoint.x;
		packet.payload.gunshotdata.worldhitpoint_y = worldhitpoint.y;
		packet.payload.gunshotdata.worldhitpoint_z = worldhitpoint.z;

		// Set hit point normal coordinates
		packet.payload.gunshotdata.hitpointNormal_x = hitpointnormal.x;
		packet.payload.gunshotdata.hitpointNormal_y = hitpointnormal.y;
		packet.payload.gunshotdata.hitpointNormal_z = hitpointnormal.z;

		// Set hit point local coordinates
		packet.payload.gunshotdata.hitpointLocal_x = hitpointlocal.x;
		packet.payload.gunshotdata.hitpointLocal_y = hitpointlocal.y;
		packet.payload.gunshotdata.hitpointLocal_z = hitpointlocal.z;

		// Set damage
		packet.payload.gunshotdata.Damage = damage;

		// Calculate force based on normal and damage (example: force = normal * damage)
		packet.payload.gunshotdata.force_x = force.x;
		packet.payload.gunshotdata.force_y = force.y;
		packet.payload.gunshotdata.force_z = force.z;

		out.push(packet);
	}

	void SendAnimation(std::string AnimationName, std::string ObjectName) {
		if (isServer && !clientConnected)
			return;

		Packet temp;
		temp.type = ANIMATION;
		temp.size = sizeof(AnimationName) + sizeof(ObjectName) + 2;

		temp.payload.animation.AnimationNameSize = sizeof(AnimationName);
		temp.payload.animation.ObjectNameSize = sizeof(ObjectName);

		strcpy(temp.payload.animation.AnimationName, AnimationName.c_str());
		strcpy(temp.payload.animation.ObjectName, ObjectName.c_str());

		out.push(temp);
	}

	void SendPlayerData(glm::vec3 postion, glm::vec3 rotation, std::string currentGun, std::string interactingWith) {
		if (isServer && !clientConnected)
			return;

		Packet temp;
		temp.type = PlAYERDATA;
		temp.size = sizeof(float) * 6;
		temp.payload.player.x = postion.x;
		temp.payload.player.y = postion.y;
		temp.payload.player.z = postion.z;

		temp.payload.player.rotation_x = rotation.x;
		temp.payload.player.rotation_y = rotation.y;
		temp.payload.player.rotation_z = rotation.z;

		temp.payload.player.interactingWithSize = sizeof(interactingWith);
		temp.payload.player.currentGunSize = sizeof(currentGun);
		
		strcpy(temp.payload.player.interactingWith, interactingWith.c_str());
		strcpy(temp.payload.player.currentGun, currentGun.c_str());

		out.push(temp);
	}

	void SendPacketMessage(std::string message) {
		if (isServer && !clientConnected)
			return;

		Packet temp;
		temp.type = MESSAGE;
		temp.size = static_cast<uint32_t>(message.size());
		strcpy(temp.payload.message.message, message.c_str());
		out.push(temp);
	}

	void EvaulatePackets() {
		//this is so it wont evaulate any more packets coming in
		int current_in_size = in.size();

		//needed
		std::string playerInteractWith = "nothing";
		std::string interact;
		std::string currentgun;
		std::string objectName;

		while (current_in_size > 0) {
			current_in_size--;
			Packet packet = in.front();
			in.pop();
			switch (packet.type)
			{
			case MESSAGE:

				break;
			case PlAYERDATA:
				currentgun = std::string(packet.payload.player.currentGun, std::strlen(packet.payload.player.currentGun));
				interact = std::string(packet.payload.player.interactingWith, std::strlen(packet.payload.player.interactingWith));
				if (interact != "nothing")
					playerInteractWith = packet.payload.player.interactingWith;

				PlayerTwo::SetData(interact, currentgun, glm::vec3(packet.payload.player.x, packet.payload.player.y, packet.payload.player.z), glm::vec3(packet.payload.player.rotation_x, packet.payload.player.rotation_y, packet.payload.player.rotation_z));
				break;
			case ANIMATION:
				//std::cout << "ANIMATION: " << packet.payload.animation.AnimationName << "\n";
				//std::cout << "ANIMATION: " << packet.payload.animation.ObjectName << "\n";
				Animator::PlayAnimation(AssetManager::GetSkinnedAnimation(packet.payload.animation.AnimationName), packet.payload.animation.ObjectName, false);
				break;

			case GUNSHOT:

				if (std::strcmp(packet.payload.gunshotdata.ObjectName, "PlayerTwo") == 0) {
					Player::TakeDamage(packet.payload.gunshotdata.Damage);
				}
				else {

					objectName = std::string(packet.payload.gunshotdata.ObjectName, packet.payload.gunshotdata.ObjectNameSize);
					GameObject* gameobject = AssetManager::GetGameObject(objectName);
					if (gameobject == nullptr)
						break;
					btRigidBody* body = gameobject->GetRigidBody();
					if(body != nullptr)
						body->applyImpulse(btVector3(packet.payload.gunshotdata.force_x, packet.payload.gunshotdata.force_y, packet.payload.gunshotdata.force_z), btVector3(packet.payload.gunshotdata.hitpointLocal_x, packet.payload.gunshotdata.hitpointLocal_y, packet.payload.gunshotdata.hitpointLocal_z));
					AssetManager::AddDecalInstance(glm::vec3(packet.payload.gunshotdata.worldhitpoint_x, packet.payload.gunshotdata.worldhitpoint_y, packet.payload.gunshotdata.worldhitpoint_z), glm::vec3(packet.payload.gunshotdata.hitpointNormal_x, packet.payload.gunshotdata.hitpointNormal_y, packet.payload.gunshotdata.hitpointNormal_z), AssetManager::GetDecal("bullet_hole"), gameobject);
				}
				break;
			case DYNAMICOBJECT: {
				objectName = std::string(packet.payload.dynamicObjectData.ObjectName, packet.payload.dynamicObjectData.ObjectNameSize);
				GameObject* dynamicObject = AssetManager::GetGameObject(objectName);
				if (dynamicObject == nullptr)
					continue;

				dynamicObject->setPosition(glm::vec3(packet.payload.dynamicObjectData.x, packet.payload.dynamicObjectData.y, packet.payload.dynamicObjectData.z));
				dynamicObject->setRotation(glm::vec3(packet.payload.dynamicObjectData.rotation_x, packet.payload.dynamicObjectData.rotation_y, packet.payload.dynamicObjectData.rotation_z));


				break;
			}
			case CONTROL:

				if (packet.payload.control.flag == CONNECTED)
					AssetManager::GetGameObject("PlayerTwo")->SetRender(true);
				else if (packet.payload.control.flag == DISCONNECTED)
					AssetManager::GetGameObject("PlayerTwo")->SetRender(false);

				break;


			default:
				break;
			}
		}
		PlayerTwo::SetIneractingWith(playerInteractWith);

	}


};

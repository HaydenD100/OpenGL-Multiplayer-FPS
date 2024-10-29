#include "NetworkManager.h"


namespace NetworkManager
{
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	WSADATA wsaData;
	int iResult;

	//server stuff
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	int isServer = 0;
	int isRunning = 1;

	std::thread netowrking_thread;



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
				printf("Bytes received: %d\n", iResult);
				std::cout << "Client: " << recvbuf << "\n";

				if (std::strcmp(recvbuf, "closeserver") == 0) {
					printf("Closeing Server");
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}

				/*
				// Echo the buffer back to the sender
				iSendResult = send(ClientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				printf("Bytes sent: %d\n", iSendResult);
				*/
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

	int RunClient(SOCKET ConnectSocket) {
		std::cout << "Starting Client \n";

		int recvbuflen = DEFAULT_BUFLEN;
		std::string message = "Client Connected to server";
		char recvbuf[DEFAULT_BUFLEN];

		int iResult;

		// Send an initial buffer
		iResult = send(ConnectSocket, message.c_str(), (int)strlen(message.c_str()), 0);
		printf("Bytes Sent: %ld\n", iResult);

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Receive data until the server closes the connection
		do {
			message.clear();
			std::cin >> message;
			iResult = send(ConnectSocket, message.c_str(), (int)strlen(message.c_str()), 0);
			printf("Bytes Sent: %ld\n", iResult);

			if (iResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
				printf("Bytes received: %d\n", iResult);
			else if (iResult == 0)
				printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());
		} while (iResult > 0 && isRunning);

		// shutdown the connection for sending since no more data will be sent
		// the client can still use the ConnectSocket for receiving data
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
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
		iResult = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}
		SOCKET ConnectSocket = INVALID_SOCKET;

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
		netowrking_thread = std::thread(RunClient, ConnectSocket);
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
	int SendDataToClient(std::string message) {
		int iSendResult;
		iSendResult = send(ClientSocket, message.c_str(), (int)strlen(message.c_str()), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
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
	int SendDataToServer(std::string message) {
		int recvbuflen = DEFAULT_BUFLEN;
		int iResult;

		// Send an initial buffer
		iResult = send(ConnectSocket, message.c_str(), (int)strlen(message.c_str()), 0);
		printf("Bytes Sent: %ld\n", iResult);

		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
	}
	*/
	int IsServer() {
		return isServer;
	}




};

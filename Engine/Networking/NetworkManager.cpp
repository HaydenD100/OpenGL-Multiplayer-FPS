#include "NetworkManager.h"
#include "Engine/Animation/SkinnedAnimatior.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Backend.h"
#include "Engine/Core/Scene/SceneManager.h"

namespace Client {

	bool IsConnected = 0;
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	sockaddr_in server;
	SOCKET out;

	std::thread networkThread;


	void Init(const char* IP) {
		std::cout << "Connecting to server... \n";


		int wsOk = WSAStartup(version, &data);
		if (wsOk != 0)
		{
			std::cout << "Can't start Winsock! " << wsOk;
			return;
		}

		// Create a hint structure for the server

		server.sin_family = AF_INET; // AF_INET = IPv4 addresses
		server.sin_port = htons(54000); // Little to big endian conversion
		inet_pton(AF_INET, IP, &server.sin_addr); // Convert from string to byte array
		// Socket creation, note that the socket type is datagram
		out = socket(AF_INET, SOCK_DGRAM, 0);

		const char* source = "Connected";
		char buf[1024];
		strncpy(buf, source, sizeof(buf) - 1);
		SendData(buf);


		networkThread = std::thread(Run);

	}
	void CleanUp() {
		networkThread.join();
		closesocket(out);
		WSACleanup();
	}

	void Run() {
		// Send a message to the server
		std::string s("Testing");
		int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));
		if (sendOk == SOCKET_ERROR) {
			std::cout << "That didn't work! " << WSAGetLastError() << std::endl;
			return;
		}

		// Buffer to receive data
		char buf[1024];
		sockaddr_in from;
		int fromLen = sizeof(from);

		while (IsConnected) {
			ZeroMemory(buf, 1024);

			int bytesIn = recvfrom(out, buf, 1024, 0, (sockaddr*)&from, &fromLen);
			if (bytesIn == SOCKET_ERROR) {
				std::cout << "Error receiving from server: " << WSAGetLastError() << std::endl;
				continue;
			}

			// Print the received message
			Packet packet;
			// Safely extract ID and code from first 4 bytes
			std::memcpy(&packet.ID, buf, sizeof(uint16_t));
			std::memcpy(&packet.code, buf + 2, sizeof(uint16_t));
			// Copy rest of the data (512 bytes)
			std::memcpy(packet.data, buf + 4, sizeof(packet.data));
			std::cout << "SERVER> " << packet.code << " : " << packet.data << "\n";
		}
	}
	void SendWorldPosition(glm::vec3 position, glm::vec3 rotation) {

	}

	void SendData(char buf[1024]) {
		int sendOk = sendto(out, buf, 1024 + 1, 0, (sockaddr*)&server, sizeof(server));
		if (sendOk == SOCKET_ERROR) {
			std::cout << "That didn't work! " << WSAGetLastError() << std::endl;
			return;
		}
	}

}
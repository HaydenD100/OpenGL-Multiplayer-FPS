#include "NetworkManager.h"
#include "Engine/Animation/SkinnedAnimatior.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Backend.h"


namespace Client {

	bool IsConnected = 0;
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	sockaddr_in server;
	SOCKET out;

	std::thread networkThread;

	ThreadSafeQueue<Packet> p_queue;


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
		SendData(buf, 1024);

		networkThread = std::thread(Run);

	}
	void CleanUp() {
		networkThread.join();
		closesocket(out);
		WSACleanup();
	}

	void Run() {
		// Send a message to the server

		std::string s("Connected To Server");
		int sendOk = sendto(out, s.c_str(), s.size(), 0, (sockaddr*)&server, sizeof(server));
		if (sendOk == SOCKET_ERROR) {
			std::cout << "That didn't work! " << WSAGetLastError() << std::endl;
			return;
		}

		std::cout << "Running\n";

		// Buffer to receive data
		char buf[1024];
		sockaddr_in from;
		int fromLen = sizeof(from);

		IsConnected = 1;
		while (IsConnected) {
			ZeroMemory(buf, 1024);

			int bytesIn = recvfrom(out, buf, 1024, 0, (sockaddr*)&from, &fromLen);
			if (bytesIn == SOCKET_ERROR) {
				std::cout << "Error receiving from server: " << WSAGetLastError() << std::endl;
				continue;
			}

			Packet t_packet;
			t_packet = Packet::Decode(buf);
			p_queue.enqueue(t_packet);
			std::cout << "SERVER> " << t_packet.code << " : ID :" << t_packet.ID << " : " << t_packet.data << "\n";
		}
	}
	void ExtractPositionRotation(const Packet& packet, glm::vec3& position, glm::vec3& rotation) {
		std::memcpy(&position, packet.data, sizeof(glm::vec3));
		std::memcpy(&rotation, packet.data + sizeof(glm::vec3), sizeof(glm::vec3));
	}

	void Evaluate() {
		Packet t_packet = p_queue.dequeue();

		switch (t_packet.code) {
		case POSITION:

			break;


		default:
			break;
		}
	}
	void SendWorldPosition(glm::vec3 position, glm::vec3 rotation) {
		Packet t_packet;
		t_packet.ID = 0;
		t_packet.code = POSITION;

		std::memcpy(t_packet.data, &position, sizeof(position));
		std::memcpy(t_packet.data + sizeof(position), &rotation, sizeof(rotation));

		const char* rawBuffer = t_packet.Encode();

		int sendOk = sendto(out, rawBuffer, Packet::EncodedSize(), 0, (sockaddr*)&server, sizeof(server));
		if (sendOk == SOCKET_ERROR) {
			std::cout << "Send failed! " << WSAGetLastError() << std::endl;
		}
	}

	void SendData(const char* buf, size_t len) {
		int sendOk = sendto(out, buf, static_cast<int>(len), 0, (sockaddr*)&server, sizeof(server));
		if (sendOk == SOCKET_ERROR) {
			std::cout << "Send failed! " << WSAGetLastError() << std::endl;
		}
	}
}
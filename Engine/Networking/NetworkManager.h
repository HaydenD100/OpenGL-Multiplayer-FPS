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
#include <condition_variable>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#undef PlaySound
#undef near
#undef far




template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

public:
    void enqueue(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cond_.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });
        T value = queue_.front();
        queue_.pop();
        return value;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};


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

    const char* Encode() const {
        static char buffer[sizeof(Packet)];

        buffer[0] = ID & 0xFF;
        buffer[1] = (ID >> 8) & 0xFF;

        buffer[2] = code & 0xFF;
        buffer[3] = (code >> 8) & 0xFF;

        std::memcpy(buffer + 4, data, sizeof(data));

        return buffer;
    }

    static Packet Decode(const char* buffer) {
        Packet pkt;

        pkt.ID = static_cast<uint8_t>(buffer[0]) |
            (static_cast<uint8_t>(buffer[1]) << 8);

        pkt.code = static_cast<uint8_t>(buffer[2]) |
            (static_cast<uint8_t>(buffer[3]) << 8);

        std::memcpy(pkt.data, buffer + 4, sizeof(pkt.data));

        return pkt;
    }
    static constexpr size_t EncodedSize() {
        return sizeof(Packet);
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
    void Evaluate();
	
	void SendWorldPosition(glm::vec3 position, glm::vec3 rotation);
    void SendData(const char* buf, size_t len);
    void ExtractPositionRotation(const Packet& packet, glm::vec3& position, glm::vec3& rotation);
};


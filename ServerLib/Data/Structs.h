#pragma once

constexpr uint32_t MaxSocketRecvBuffer = 256;
constexpr uint32_t MaxSocketSendBuffer = 4096;
constexpr uint32_t MaxSocketBuffer     = 1024;
constexpr int32_t  MaxThread           = 4;
constexpr uint64_t SessionWaitTimeSec  = 3;

struct FOverlappedEx
{
	WSAOVERLAPPED Overlapped; // Overlapped변수를 이 구조체로 형변환하려면 첫번째 멤버로 두자
	SOCKET        ClientSocket;
	WSABUF        WSABuffer;
	char          Buffer[MaxSocketBuffer];
	EIOOperation  OperationType;
};

struct FClientInfo
{
	SOCKET        ClientSocket = INVALID_SOCKET;
	FOverlappedEx RecvOverlappedEx;
	FOverlappedEx SendOverlappedEx;

	FClientInfo()
	{
		ZeroMemory(&RecvOverlappedEx, sizeof(FOverlappedEx));
		ZeroMemory(&SendOverlappedEx, sizeof(FOverlappedEx));
	}
};

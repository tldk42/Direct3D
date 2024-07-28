#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#include "Data/Enums.h"
#include "Data/Structs.h"

class IOCompletionPort
{
public:
	IOCompletionPort();
	~IOCompletionPort();

public:
	bool Initialize();
	bool BindNListen(const int32_t InPort);
	bool Run();
	void StopThread();

private:
	void SetClientPool(const uint32_t InMaxClientCount);
	bool CreateWorkerThread();
	bool CreateAcceptorThread();

	struct FClientInfo* GetEmptyClientInfo();

	bool BindIOCompletionPort(FClientInfo* InClientInfo);
	bool BindReceive(FClientInfo* InClientInfo);
	bool SendMSG(FClientInfo* InClientInfo, char* InMessage, int32_t InMessageLength);

	void WorkerThread();
	void AcceptorThread();

	void CloseSocket(FClientInfo* InClientInfo, bool bIsForce = false);

private:
	std::vector<FClientInfo> mClients;

	HANDLE  mIOCPHandle      = INVALID_HANDLE_VALUE;
	SOCKET  mListeningSocket = INVALID_SOCKET;
	int32_t mClientCount     = 0;

	std::vector<std::thread> mWorkerThreads;
	std::thread              mAcceptorThread;

	bool bIsWorkerRunning   = true;
	bool bIsAcceptorRunning = true;

};

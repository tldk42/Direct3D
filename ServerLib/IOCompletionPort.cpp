#include "IOCompletionPort.h"

IOCompletionPort::IOCompletionPort()
{
	WSADATA wsaData;
	int     returnValue = WSAStartup(
		MAKEWORD(2, 2), // major | minor
		&wsaData);

	if (returnValue == SOCKET_ERROR)
	{
		std::cout << "[에러] WSAStartup()함수 실패 : " << WSAGetLastError() << '\n';
	}
}

IOCompletionPort::~IOCompletionPort()
{
	WSACleanup();
}

bool IOCompletionPort::Initialize()
{
	mListeningSocket = WSASocket(
		AF_INET, // IPv4
		SOCK_STREAM, // TCP Socket
		IPPROTO_TCP,  // TCP Protocol
		nullptr,
		NULL,
		WSA_FLAG_OVERLAPPED // 비동기 중첩 I/O
	);

	if (mListeningSocket == INVALID_SOCKET)
	{
		std::cout << "[에러] socket()함수 실패 :" << WSAGetLastError() << '\n';
		return false;
	}

	std::cout << "소켓 초기화 성공" << '\n';
	return true;
}

bool IOCompletionPort::BindNListen(const int32_t InPort)
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_port        = htons(InPort);		// host to network short 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long

	if (bind(mListeningSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cout << "[에러] bind()함수 실패 : " << WSAGetLastError() << '\n';
		return false;
	}

	if (listen(mListeningSocket, 5) == SOCKET_ERROR)
	{
		std::cout << "[에러] listen()함수 실패 : " << WSAGetLastError() << '\n';
		return false;
	}
	std::cout << "서버 등록 성공.." << '\n';
	return true;
}

bool IOCompletionPort::Run()
{
	SetClientPool(10);

	mIOCPHandle = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		nullptr,
		NULL,
		4
	);
	if (mIOCPHandle == INVALID_HANDLE_VALUE)
	{
		printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
		return false;
	}

	if (!CreateWorkerThread())
	{
		return false;
	}

	if (!CreateAcceptorThread())
	{
		return false;
	}

	printf("서버 시작\n");
	return true;
}

void IOCompletionPort::StopThread()
{
	bIsWorkerRunning = false;
	CloseHandle(mIOCPHandle);

	for (auto& thread : mWorkerThreads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	//Acceptor 쓰레드를 종료한다.
	bIsAcceptorRunning = false;
	closesocket(mListeningSocket);

	if (mAcceptorThread.joinable())
	{
		mAcceptorThread.join();
	}
}

void IOCompletionPort::SetClientPool(const uint32_t InMaxClientCount)
{
	for (int32_t i = 0; i < InMaxClientCount; ++i)
	{
		mClients.emplace_back();
	}
}

bool IOCompletionPort::CreateWorkerThread()
{
	for (int32_t i = 0; i < 4; ++i)
	{
		mWorkerThreads.emplace_back([this](){ WorkerThread(); });
	}

	printf("Work Thread 시작..\n");
	return true;
}

bool IOCompletionPort::CreateAcceptorThread()
{
	mAcceptorThread = std::thread([this](){ AcceptorThread(); });

	printf("Accept Thread 시작..\n");
	return true;
}

FClientInfo* IOCompletionPort::GetEmptyClientInfo()
{
	// auto it = std::find_if(mClients.begin(), mClients.end(), [](){});

	for (auto& client : mClients)
	{
		if (client.ClientSocket == INVALID_SOCKET)
		{
			return &client;
		}
	}

	return nullptr;
}

bool IOCompletionPort::BindIOCompletionPort(FClientInfo* InClientInfo)
{
	HANDLE IOCPHandle = CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(InClientInfo->ClientSocket),
		mIOCPHandle,
		reinterpret_cast<ULONG_PTR>(InClientInfo),
		0);

	if (IOCPHandle == nullptr || IOCPHandle != mIOCPHandle)
	{
		std::cout << "[에러] CreateIoCompletionPort()함수 실패: " << GetLastError() << '\n';
		return false;
	}
	return true;
}

bool IOCompletionPort::BindReceive(FClientInfo* InClientInfo)
{
	DWORD flag         = 0;
	DWORD recvNumBytes = 0;

	InClientInfo->RecvOverlappedEx.WSABuffer.len = MaxSocketBuffer;
	InClientInfo->RecvOverlappedEx.WSABuffer.buf = InClientInfo->RecvOverlappedEx.Buffer;
	InClientInfo->RecvOverlappedEx.OperationType = EIOOperation::Receive;

	int32_t returnValue = WSARecv(
		InClientInfo->ClientSocket,
		&InClientInfo->RecvOverlappedEx.WSABuffer,
		1,
		&recvNumBytes,
		&flag,
		reinterpret_cast<LPWSAOVERLAPPED>(&InClientInfo->RecvOverlappedEx),
		nullptr);

	if (returnValue == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool IOCompletionPort::SendMSG(FClientInfo* InClientInfo, char* InMessage, int32_t InMessageLength)
{
	DWORD recvNumBytes = 0;

	CopyMemory(InClientInfo->SendOverlappedEx.Buffer, InMessage, InMessageLength);

	InClientInfo->SendOverlappedEx.WSABuffer.len = InMessageLength;
	InClientInfo->SendOverlappedEx.WSABuffer.buf = InClientInfo->SendOverlappedEx.Buffer;
	InClientInfo->SendOverlappedEx.OperationType = EIOOperation::Send;

	int32_t returnValue = WSASend(
		InClientInfo->ClientSocket,
		&InClientInfo->SendOverlappedEx.WSABuffer,
		1,
		&recvNumBytes,
		0,
		reinterpret_cast<LPWSAOVERLAPPED>(&InClientInfo->SendOverlappedEx),
		nullptr);

	if (returnValue == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void IOCompletionPort::WorkerThread()
{
	FClientInfo* clientInfo = nullptr;
	BOOL         bSuccess   = FALSE;
	DWORD        ioSize     = 0;
	LPOVERLAPPED overlapped = nullptr;

	while (bIsWorkerRunning)
	{
		bSuccess = GetQueuedCompletionStatus(
			mIOCPHandle, // I/O 완료 포트 핸들
			&ioSize, // 전송된 바이트 
			reinterpret_cast<PULONG_PTR>(&clientInfo), // 완료된 I/O 작업과 연관된 키값
			&overlapped, // 데이터
			INFINITE // 반환될때 까지 무기한 대기
		);

		if (bSuccess == TRUE && ioSize == 0 && overlapped == nullptr)
		{
			bIsWorkerRunning = false;
			continue;
		}

		if (overlapped == nullptr)
		{
			continue;
		}

		if (bSuccess == FALSE || (ioSize == 0 && bSuccess == TRUE))
		{
			printf("socket(%d) 접속 끊김\n", static_cast<int>(clientInfo->ClientSocket));
			CloseSocket(clientInfo);
			continue;
		}

		FOverlappedEx* overlappedEx = reinterpret_cast<FOverlappedEx*>(overlapped);

		switch (overlappedEx->OperationType)
		{
		case EIOOperation::Accept:
			break;
		case EIOOperation::Receive:
			printf("[수신] bytes : %d , msg : %s\n", ioSize, overlappedEx->Buffer + 5);
			overlappedEx->Buffer[ioSize] = 0;

			SendMSG(clientInfo, overlappedEx->Buffer, ioSize);
			BindReceive(clientInfo);
			break;
		case EIOOperation::Send:
			printf("[송신] bytes : %d , msg : %s\n", ioSize, overlappedEx->Buffer + 5);
			break;
		default:
			printf("socket(%d)에서 예외상황\n", static_cast<int>(clientInfo->ClientSocket));
			break;
		}
	}
}

void IOCompletionPort::AcceptorThread()
{
	SOCKADDR_IN clientAddr;
	int32_t     addrLen = sizeof(SOCKADDR_IN);

	while (bIsAcceptorRunning)
	{
		FClientInfo* clientInfo = GetEmptyClientInfo();
		if (clientInfo == nullptr)
		{
			printf("[에러] Client Full\n");
			return;
		}

		clientInfo->ClientSocket = accept(mListeningSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);
		if (clientInfo->ClientSocket == INVALID_SOCKET)
		{
			continue;
		}

		bool returnValue = BindIOCompletionPort(clientInfo);
		if (!returnValue)
		{
			return;
		}

		returnValue = BindReceive(clientInfo);
		if (!returnValue)
		{
			return;
		}

		char clientIP[32] = {0,};
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, 32 - 1);
		printf("클라이언트 접속 : IP(%s) SOCKET(%d)\n", clientIP, static_cast<int>(clientInfo->ClientSocket));

		++mClientCount;
	}
}

void IOCompletionPort::CloseSocket(FClientInfo* InClientInfo, bool bIsForce)
{
	linger linger = {0, 0};

	if (bIsForce)
	{
		linger.l_onoff = 1;
	}

	shutdown(InClientInfo->ClientSocket, SD_BOTH);

	setsockopt(InClientInfo->ClientSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&linger), sizeof(linger));

	closesocket(InClientInfo->ClientSocket);

	InClientInfo->ClientSocket = INVALID_SOCKET;
}

#include "IOCompletionPort.h"

IOCompletionPort::IOCompletionPort()
{
	WSADATA wsaData;
	int     returnValue = WSAStartup(
		MAKEWORD(2, 2), // major | minor
		&wsaData);

	if (returnValue == SOCKET_ERROR)
	{
		std::cout << "[����] WSAStartup()�Լ� ���� : " << WSAGetLastError() << '\n';
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
		WSA_FLAG_OVERLAPPED // �񵿱� ��ø I/O
	);

	if (mListeningSocket == INVALID_SOCKET)
	{
		std::cout << "[����] socket()�Լ� ���� :" << WSAGetLastError() << '\n';
		return false;
	}

	std::cout << "���� �ʱ�ȭ ����" << '\n';
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
		std::cout << "[����] bind()�Լ� ���� : " << WSAGetLastError() << '\n';
		return false;
	}

	if (listen(mListeningSocket, 5) == SOCKET_ERROR)
	{
		std::cout << "[����] listen()�Լ� ���� : " << WSAGetLastError() << '\n';
		return false;
	}
	std::cout << "���� ��� ����.." << '\n';
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
		printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
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

	printf("���� ����\n");
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

	//Acceptor �����带 �����Ѵ�.
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

	printf("Work Thread ����..\n");
	return true;
}

bool IOCompletionPort::CreateAcceptorThread()
{
	mAcceptorThread = std::thread([this](){ AcceptorThread(); });

	printf("Accept Thread ����..\n");
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
		std::cout << "[����] CreateIoCompletionPort()�Լ� ����: " << GetLastError() << '\n';
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
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
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
		printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
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
			mIOCPHandle, // I/O �Ϸ� ��Ʈ �ڵ�
			&ioSize, // ���۵� ����Ʈ 
			reinterpret_cast<PULONG_PTR>(&clientInfo), // �Ϸ�� I/O �۾��� ������ Ű��
			&overlapped, // ������
			INFINITE // ��ȯ�ɶ� ���� ������ ���
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
			printf("socket(%d) ���� ����\n", static_cast<int>(clientInfo->ClientSocket));
			CloseSocket(clientInfo);
			continue;
		}

		FOverlappedEx* overlappedEx = reinterpret_cast<FOverlappedEx*>(overlapped);

		switch (overlappedEx->OperationType)
		{
		case EIOOperation::Accept:
			break;
		case EIOOperation::Receive:
			printf("[����] bytes : %d , msg : %s\n", ioSize, overlappedEx->Buffer + 5);
			overlappedEx->Buffer[ioSize] = 0;

			SendMSG(clientInfo, overlappedEx->Buffer, ioSize);
			BindReceive(clientInfo);
			break;
		case EIOOperation::Send:
			printf("[�۽�] bytes : %d , msg : %s\n", ioSize, overlappedEx->Buffer + 5);
			break;
		default:
			printf("socket(%d)���� ���ܻ�Ȳ\n", static_cast<int>(clientInfo->ClientSocket));
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
			printf("[����] Client Full\n");
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
		printf("Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)\n", clientIP, static_cast<int>(clientInfo->ClientSocket));

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

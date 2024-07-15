#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#include <thread>
#pragma comment(lib, "ws2_32.lib");

SOCKET sock;

bool InitWinSoc();
void DelWinSock();

void SendMSG_Implement();

int main(int argc, char* argv[])
{
	InitWinSoc();

	char sZip[30];
	std::cout << "접속할 서버 주소: ";
	std::cin >> sZip;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	try
	{
		if (sock == INVALID_SOCKET)
			throw "Failed create socket";

		SOCKADDR_IN addr;
		addr.sin_family      = AF_INET;
		addr.sin_port        = htons(10000);
		addr.sin_addr.s_addr = inet_addr(sZip);
		if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		{
			throw "Error connect";
		}

		u_long iNonSocket = TRUE;
		int    iMode      = ioctlsocket(sock, FIONBIO, &iNonSocket);

		std::thread sendThread(SendMSG_Implement);

		std::string Recvbuf;
		Recvbuf.resize(256);

		while (true)
		{
			int RecvByte = recv(sock, &Recvbuf[0], Recvbuf.size(), 0);
			if (RecvByte == 0)
			{
				std::cout << "서버 종료!" << std::endl;
				break;
			}
			if (RecvByte < 0)
			{
				int iError = WSAGetLastError();
				if (iError != WSAEWOULDBLOCK)
				{
					std::cout << "비정상 서버 종료!" << std::endl;
					break;
				}
			}
			else
			{
				Recvbuf[RecvByte] = 0;
				std::cout << Recvbuf.c_str() << std::endl;
			}
		}
	}
	catch (char* ErrorMSG)
	{
		std::cout << ErrorMSG << '\n';

		LPVOID lpOSMSG;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
					  FORMAT_MESSAGE_FROM_SYSTEM |
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  nullptr, WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  reinterpret_cast<LPTSTR>(&lpOSMSG), 0, nullptr);

		std::cout << "[" << static_cast<char*>(lpOSMSG) << "]" << '\n';

		LocalFree(lpOSMSG);
	}

	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
	}

	DelWinSock();
	return 0;
}


bool InitWinSoc()
{
	WSADATA wsaData;
	int     returnValue = WSAStartup(
		MAKEWORD(2, 2), // major | minor
		&wsaData);
	if (returnValue == SOCKET_ERROR)
	{
		std::cout << "윈도우 소켓 초기화 실패" << '\n';
	}

	return returnValue;
}

void DelWinSock()
{
	WSACleanup();
}

void SendMSG_Implement()
{
	std::string UserName = "[임재강]";
	std::string Sendbuf;
	Sendbuf.reserve(256);
	std::string KeyDatabuf;
	KeyDatabuf.reserve(256);

	while (true)
	{
		Sendbuf.clear();
		Sendbuf = UserName;
		std::cout << "데이터입력(종료:엔터) : ";
		std::getline(std::cin, KeyDatabuf);
		if (KeyDatabuf.empty())
		{
			continue;
		}
		Sendbuf += KeyDatabuf;

		int SendByte = send(sock, Sendbuf.c_str(), Sendbuf.size(), 0);
		if (SendByte < 0)
		{
			int iError = WSAGetLastError();
			if (iError != WSAEWOULDBLOCK)
			{
				break;
			}
			continue;
		}
		std::cout << "보내고" << SendByte << Sendbuf.c_str() << std::endl;
	}
}

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib");


bool InitWinSoc();
void DelWinSock();

int main(int argc, char* argv[])
{
	InitWinSoc();

	char sZip[30];
	std::cout << "접속할 서버 주소: ";
	std::cin >> sZip;

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

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

		char szTime[100] = {0};
		recv(sock, szTime, sizeof(szTime), 0);

		std::cout << "Server Time: " << szTime << '\n';

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
	WSADATA wsaData     ;
	int       returnValue = WSAStartup(
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

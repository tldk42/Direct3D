#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>


#pragma comment(lib, "ws2_32.lib");

constexpr int32_t MAX_QUEUE = 256;

bool InitWinSoc();
void DelWinSock();

int main(int argc, char* argv[])
{
	InitWinSoc();


	/** socket
	 * 서버에서 최초로 생성하는 소켓은 통신을 위한 소켓이 아님
	 *  서버에서는 통신소켓을 직접 생성하는 것이 아니라 접속 요청이 왔을 때
	 *  운영체제에서 생성한다.
	 *  지금 생성하는 이 소켓은 클리이언트로부터의 접속 요청을 받기 위한 listen(대기) 소켓이다.
	 *
	 *  -----------------------------------------------------------------------------
	 *				통신을 위한 소켓								대기 소켓
	 *   		- 프로토콜									 - 프로토콜
	 *   		- Local Port Num							 - Local Port Num
	 *   		- Local IP Addr								 - Local IP Addr
	 *   		- Foreign Port Num							 
	 *   		- Foreign IP Addr							 
	 */
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	try
	{
		if (sock == INVALID_SOCKET)
			throw "Socket(listen) Creation Failed";

		/** bind
	 * 생성된 socket에 Local Port Num, Local IP Addr을 연결한다.
	 * 이렇게 bind를 하고 나면 대기(listen)를 위한 소켓의 조건이 만족된다.
	 * bind함수는 실패 가능성이 높으므로 항상 리턴값을 확인한다.
	 */
		SOCKADDR_IN sockAddr; // SOCKADDR_IN은 TCP/IP 전용 주소 구조체
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 본인 IP를 넣어주므로 특정 호스트(특정 컴퓨터IP)에 종속된다. INADDY_ANY는 Local IP를 명시한다.
		sockAddr.sin_port        = htons(10000);
		sockAddr.sin_family      = AF_INET;

		if (bind(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) == SOCKET_ERROR)
		{
			// TODO: Handle Error
			std::cout << "Socket Binding Error" << '\n';
		}

		/** listen
		 * 이전에 bind로 설정한 IP와 포트번호로 설정한 소켓을 대기 소켓이라는 것을 운영체제에게 알림
		 *
		 *	listen에서는 소켓 개체를 대기 상태로 변경시킨다. (첫번째 인자가 소켓)
		 *	listen이 호출 된 이후부터 해당 소켓 개체로의 접속 요청이 있으면 백그라운드로의 접속을 받아들일 수 있게 된다.
		 *	접속을 받아들일 때에는 최대 대기 큐(backlog)가 존재하는데 이 대기 큐의 최댓값은 listen의 두번째 인자로 들어간다.
		 */
		if (listen(sock, MAX_QUEUE) == SOCKET_ERROR)
		{
			throw "Socket listen Error";
		}

		/** accept
		 * 접속 대기 큐로부터 접속된 소켓 개체를 가져오기 위해 호출
		 *  여기서 반환된 소켓은 대기큐에서 제거된다.
		 *
		 *  소켓에는 명시적으로 접속 요청을 받는 메서드가 존재하지 않음
		 *  - 접속을 받는 행위는 운영체제에서 담당
		 *  - 접속 요건(port, ipaddr)이 충족되면 새로운 소켓 개체가 생성됨
		 *  - 생성된 소켓 개체는 대기 큐로 들어가게 됨
		 *
		 *  일반적으로 accept는 루프 안에서 호출된다.
		 *   - 여러 클라이언트가 지속적으로 서버에 접속 요청을 보낸다
		 *   - 대기 큐로부터 통신 소켓을 반복적으로 뽑아내야한다.
		 *   - accept함수가 호출된 시점에 대기큐에 아무 소켓이 존재하지 않는다?
		 *    - accept함수가 블로킹되고 새로운 접속이 올 때까지 whileloop에서 벗어나지 않는다(대기).
		 *
		 *  두번째 인자는 클라이언트의 소켓 주소 정보
		 *   - 이 클라이언트를 거부할 수 있다.
		 *   - 클라이언트 접속을 기록할 수 있다.
		 */
		while (true)
		{
			SOCKADDR_IN clientAddr;
			int32_t     clientAddrLen = sizeof(clientAddr);
			SOCKET      commSock      = accept(sock, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

			std::cout << "connected from[" << inet_ntoa(clientAddr.sin_addr) << "]" << '\n';

			time_t longTime;
			tm*    newTime;
			char   timeSize[100];

			time(&longTime);
			newTime = localtime(&longTime);

			sprintf_s(timeSize, "%.19s, zzz\n", asctime(newTime));
			send(commSock, timeSize, static_cast<int>(strlen(timeSize)), 0);
			closesocket(commSock);
		}
	}
	catch (char* ErrorMSG)
	{
		std::cout << ErrorMSG;

		LPVOID OSMessage;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
					  FORMAT_MESSAGE_FROM_SYSTEM |
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  nullptr, WSAGetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  reinterpret_cast<LPTSTR>(&OSMessage), 0, nullptr);

		std::cout << "[" << static_cast<char*>(OSMessage) << "]" << '\n';

		LocalFree(OSMessage);
	}

	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
	}

	DelWinSock();

	/** recv()
	 * 수신 메서드가 아니다
	 * 수신 버퍼에서 데이터를 읽는 메서드
	 * 데이터를 수신하고 수신버퍼에 채우는 일? -> 프로토콜 계층에서 담당
	 *
	 * 이 메서드는 임의로 호출하는 것이 아니라 소켓 버퍼에 데이터가 수신된 정확한 시점에서 호출한다.
	 * - 데이터가 수신되지 않은 상태에서 호출하면 블로킹된다 (정지)
	 *
	 * @param1,2,3 send와 같다
	 * @param4 flag값 하나가 추가 되었는데 MSG_PEEK수신 버퍼의 데이터 삭제를 보류)
	 */

	/** send()
	 * 데이터를 바로 보내는게 아니다
	 * 1. 소켓 내부에 있는 송신버퍼에 복사
	 * 2. send함수 반환 후 나중에 송신 시작
	 *
	 * @param1 송신하는 소켓 개체 핸들
	 * @param2 송신 버퍼 주소 (char* 이지만 캐스팅만 가능하다면 어떤 타입이던 상관없다)
	 * @param3 송신 데이터 크기
	 * @param4 flag값 0(일반 데이터 채널) | MSG_OBB(긴급 데이터 채널)
	 *
	 * @return 실제 송신된 데이터의 바이트 크기 (버퍼에 복사된 크기... 이유는 위2번에 나온다)
	 */


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

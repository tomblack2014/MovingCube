
#include <winsock2.h>
#pragma comment(lib, "Wsock32.lib")
#include <WS2tcpip.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <mutex>
#include <vector>

struct pos
{
	float p[3];
};

std::vector<pos> posData;
int m_status = 0;
std::mutex m_lock;

void DataReverse(void* ptr, size_t len)
{
	char* lhs = ((char*)ptr);
	char* rhs = ((char*)ptr) + len - 1;
	while (lhs < rhs)
		std::swap(*lhs++, *rhs--);
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET m_conn = (SOCKET)lpParameter;

	while (true) {
		const int BUFFER_SIZE = 1024;
		char recvBuf[BUFFER_SIZE];

		char command[16] = "Holo/SceneGraph";
		memset(recvBuf, 0, sizeof(recvBuf));

		int recvLen = 0;
		recvLen = recv(m_conn, recvBuf, sizeof(recvBuf), 0);
		if (recvLen < 0) {
			std::cout << "recv error, num = " << recvLen << std::endl;
			break;
		}
		
		if (memcmp(command, recvBuf, sizeof(command)) == 0) {
			std::cout << "new data request!" << std::endl;
			int num = posData.size();
			int res = send(m_conn, (char*)&num, sizeof(num), 0);
			m_lock.lock();
			res = send(m_conn, (char*)posData.data(), posData.size() * sizeof(pos), 0);
			m_lock.unlock();
			if (res < 0) {
				std::cout << "send error, num = " << res << std::endl;
				break;
			}
		}
		else {
			std::cout << "new upload data!" << std::endl;
			int num = 0;
			int res = recv(m_conn, (char*)&num, sizeof(num), 0);
			pos upData;
			recvLen = recv(m_conn, (char*)&upData, sizeof(upData), 0);
			m_lock.lock();
			if (num < posData.size())
				posData[num] = upData;
			m_lock.unlock();
		}
		std::cout << "send back data!" << std::endl;
	}
	std::cout << "close a connect!" << std::endl;
	return 0;
}

int main()
{
	pos p[4];

	for (int i = 0; i < 4; i++) {
		p[i].p[0] = i % 2 == 0 ? -0.5 : 0.5;
		p[i].p[1] = i % 4 == 2 || i % 4 == 3 ? 0.5 : -0.5;
		p[i].p[2] = -2;
		posData.push_back(p[i]);
	}

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// Æô¶¯socket api  
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	// ´´½¨socket  
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == serverSocket)
	{
		err = WSAGetLastError();
		return -1;
	}

	SOCKADDR_IN addrSrv;
	int port = 2235;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);

	int retVal = bind(serverSocket, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));

	char tmp[32];

	if (retVal == SOCKET_ERROR) {
		//printf("Failed bind:%d\n", WSAGetLastError());
		sprintf_s(tmp, "TCP Failed Bind:%d", WSAGetLastError());
		std::cout << tmp << std::endl;
		return -1;
	}

	retVal = listen(serverSocket, 10);
	if (retVal == SOCKET_ERROR) {
		sprintf_s(tmp, "TCP Failed Listen:%d", WSAGetLastError());
		std::cout << tmp << std::endl;
		return -1;
	}

	while (true) {
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN addrClient;
		SOCKET m_conn = accept(serverSocket, (SOCKADDR *)&addrClient, &len);
		if (m_conn == INVALID_SOCKET) {
			std::cerr << "Failed to accept client!Error code: " << ::WSAGetLastError() << "\n";
			::WSACleanup();
			system("pause");
			exit(1);
		}

		HANDLE h_thread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)m_conn, 0, nullptr);
		if (h_thread == NULL) {
			std::cerr << "Failed to create a new thread!Error code: " << ::WSAGetLastError() << "\n";
			::WSACleanup();
			system("pause");
			exit(1);
		}
		std::cout << "new connect!" << std::endl;
	}

	return 0;
}
#pragma once

#include <list>
#include <WinSock2.h>
#include <vector>

class Client
{
public:
	~Client() {
		closesocket(m_socket);
		WSACleanup();
	};

	void Connect();

	bool AskData(std::vector<char>& data);

	void SetUploadData(std::vector<char> data);

private:
	SOCKET m_socket;
};
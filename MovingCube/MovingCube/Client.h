#pragma once

#include <list>
#include <WinSock2.h>

class Client
{
public:
	~Client() {
		closesocket(m_socket);
		WSACleanup();
	};

	void Connect();

	bool AskData(std::list<Windows::Foundation::Numerics::float3>&);

	void SetUploadData(Windows::Foundation::Numerics::float3 data, int num);

private:
	SOCKET m_socket;
};
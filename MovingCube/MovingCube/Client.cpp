#include "pch.h"
#include "Client.h"


#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <string>
#include <mutex>

std::mutex m_mutex;
std::list<Windows::Foundation::Numerics::float3> m_data;

std::mutex m_mutex2;
bool m_isUploadData = false;
Windows::Foundation::Numerics::float3 m_uploadData;
int posNum;

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET m_socket = (SOCKET)lpParameter;

	while (true) {
		Sleep(50);

		m_mutex2.lock();
		if (m_isUploadData) {
			char cmd[] = "Holo/UploadData";
			int res = send(m_socket, cmd, sizeof(cmd), 0); //发送失败则返回SOCKET_ERROR

			res = send(m_socket, (char*)&posNum, sizeof(posNum), 0);

			float upData[3];
			upData[0] = m_uploadData.x; upData[1] = m_uploadData.y; upData[2] = m_uploadData.z;
			res = send(m_socket, (char*)upData, sizeof(upData), 0);
			m_isUploadData = false;
		}
		m_mutex2.unlock();

		char cmd[] = "Holo/SceneGraph";
		int res = send(m_socket, cmd, sizeof(cmd), 0); //发送失败则返回SOCKET_ERROR

		if (res != sizeof(cmd))
			return false;

		int posNum = 0;
		res = recv(m_socket, (char*)&posNum, sizeof(posNum), 0);
		if (res != sizeof(posNum))
			return false;

		std::list<Windows::Foundation::Numerics::float3> tmp;
		for (int i = 0; i < posNum; i++) {
			Windows::Foundation::Numerics::float3 pos;
			for (int j = 0; j < 3; j++) {
				float posData = 0.f;
				res = recv(m_socket, (char*)&posData, sizeof(posData), 0);
				if (res != sizeof(posData))
					return false;
				switch (j)
				{
				case 0:
					pos.x = posData;
					break;
				case 1:
					pos.y = posData;
					break;
				case 2:
					pos.z = posData;
					break;
				default:
					break;
				}
			}
			tmp.push_back(pos);
		}
		m_mutex.lock();
		m_data = tmp;
		m_mutex.unlock();
	}
}

void Client::Connect()
{
	WSADATA wsa_data;
	int ret;
	ret = WSAStartup(MAKEWORD(2, 2), &wsa_data); // 返回非0失败

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET; //Internet协议
	server_addr.sin_port = htons(2235);
	server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.31.12");

	while (SOCKET_ERROR == connect(m_socket, (SOCKADDR *)&server_addr, sizeof(SOCKADDR))) //连接失败则返回SOCKET_ERROR
	{
		Sleep(100);
	}

	::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)m_socket, 0, nullptr);
}

bool Client::AskData(std::list<Windows::Foundation::Numerics::float3>& data)
{
	m_mutex.lock();
	data = m_data;
	m_mutex.unlock();
	return true;

	char cmd[] = "Holo/SceneGraph";
	int res = send(m_socket, cmd, sizeof(cmd), 0); //发送失败则返回SOCKET_ERROR

	if (res != sizeof(cmd))
		return false;

	int posNum = 0;
	res = recv(m_socket, (char*)&posNum, sizeof(posNum), 0);
	if (res != sizeof(posNum))
		return false;

	std::list<Windows::Foundation::Numerics::float3> tmp;
	for (int i = 0; i < posNum; i++) {
		Windows::Foundation::Numerics::float3 pos;
		for (int j = 0; j < 3; j++) {
			float posData = 0.f;
			res = recv(m_socket, (char*)&posData, sizeof(posData), 0);
			if (res != sizeof(posData))
				return false;
			switch (j)
			{
			case 0:
				pos.x = posData;
				break;
			case 1:
				pos.y = posData;
				break;
			case 2:
				pos.z = posData;
				break;
			default:
				break;
			}
		}
		tmp.push_back(pos);
	}

	data = tmp;
	return true;
}

void Client::SetUploadData(Windows::Foundation::Numerics::float3 data, int num)
{
	m_mutex2.lock();
	m_uploadData = data;
	m_isUploadData = true;
	posNum = num;
	m_mutex2.unlock();
}

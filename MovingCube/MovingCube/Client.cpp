#include "pch.h"
#include "Client.h"


#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <string>
#include <mutex>

std::mutex m_mutex;
//std::list<Windows::Foundation::Numerics::float3> m_data;
bool m_isDataUpdate = false;
std::vector<char> m_data;

std::mutex m_mutex2;
bool m_isUploadData = false;
std::vector<char> m_uploadData;
int posNum;

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET m_socket = (SOCKET)lpParameter;

	while (true) {
		Sleep(200);

		m_mutex2.lock();
		if (m_isUploadData) {
			char cmd[] = "Holo/UploadData";
			int res = send(m_socket, cmd, sizeof(cmd), 0); //发送失败则返回SOCKET_ERROR

			int size = m_uploadData.size();
			res = send(m_socket, (char*)&size, sizeof(size), 0);

			res = send(m_socket, m_uploadData.data(), m_uploadData.size(), 0);
			m_isUploadData = false;
		}
		m_mutex2.unlock();

		char cmd[] = "Holo/SceneGraph";
		int res = send(m_socket, cmd, sizeof(cmd), 0); //发送失败则返回SOCKET_ERROR

		if (res != sizeof(cmd))
			return false;

		int size = 0;
		res = recv(m_socket, (char*)&size, sizeof(size), 0);

		m_data.resize(size);
		m_mutex.lock();
		int count = 0;
		while (count != size) {
			res = recv(m_socket, m_data.data() + count, size - count, 0);
			count += res;
		}
		m_isDataUpdate = true;
		m_mutex.unlock();

		/*int posNum = 0;
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
		}*/

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

bool Client::AskData(std::vector<char>& data)
{
	if (m_isDataUpdate) {
		m_mutex.lock();
		data = m_data;
		m_mutex.unlock();
		m_isDataUpdate = false;
	}
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

	//data = tmp;
	return true;
}

void Client::SetUploadData(std::vector<char> data)
{
	m_mutex2.lock();
	m_uploadData = data;
	m_isUploadData = true;
	m_mutex2.unlock();
}

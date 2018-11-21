
#include <winsock2.h>
#pragma comment(lib, "Wsock32.lib")
#include <WS2tcpip.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <fstream>

#include "Sphere.h"
#include "Cube.h"
#include "Model.h"
#include "Root.h" 

struct pos
{
	float p[3];
};

NamedObj* m_root;
std::vector<NamedObj*> m_objs;
std::vector<NamedObj*> m_models;
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

	int totalRecvLen = 0;
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
		else if (recvLen == 0) {
			std::cout << " Connection Closed " << std::endl;
			break;
		}
		
		if (memcmp(command, recvBuf, sizeof(command)) == 0) {
			//std::cout << "new data request!" << std::endl;
			char* dataPtr = nullptr;
			int size = m_root->Pack(&dataPtr);
			int res = send(m_conn, (char*)&size, sizeof(size), 0);
			m_lock.lock();
			res = send(m_conn, dataPtr, size, 0);

			std::cout << res << " Bytes Sent" << std::endl;
			std::ofstream sDatafile;
			sDatafile.open("sendData.txt");
			sDatafile << res;
			sDatafile.close();

			m_lock.unlock();
			if (res < 0) {
				std::cout << "send error, num = " << res << std::endl;
				break;
			}
		}
		else {
			std::cout << "new upload data!" << std::endl;
			int size = 0;
			int res = recv(m_conn, (char*)&size, sizeof(size), 0);
			std::vector<char> upData(size);
			int count = 0;
			while (count != size) {
				recvLen = recv(m_conn, upData.data() + count, size - count, 0);
				count += recvLen;
			}

			std::cout << recvLen << " Bytes Received" << std::endl;
			std::ofstream rDatafile;
			rDatafile.open("rcvData.txt");
			rDatafile << recvLen;
			rDatafile.close();

			m_lock.lock();
			if (upData.size() > 0)
				NamedObj::UnPack(upData.size(), upData.data(), m_objs);

			m_lock.unlock();
		}
		std::cout << "send back data!" << std::endl;
	}
	std::cout << "close a connect!" << std::endl;
	return 0;
}

const static std::vector<VertexPositionColor> cubeVertices =
{ 
	{ XMFLOAT3(-0.1f, -0.1f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
{ XMFLOAT3(-0.1f, -0.1f,  0.1f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
{ XMFLOAT3(-0.1f,  0.1f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
{ XMFLOAT3(-0.1f,  0.1f,  0.1f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
{ XMFLOAT3(0.1f, -0.1f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
{ XMFLOAT3(0.1f, -0.1f,  0.1f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
{ XMFLOAT3(0.1f,  0.1f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.1f,  0.1f,  0.1f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	 };

const static std::vector<unsigned short> cubeIndices =
{ {
		2,1,0, // -x
		2,3,1,

		6,4,5, // +x
		6,5,7,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	} };

const static std::vector<VertexPositionColor> pyramidVertices =
{ 
	{ XMFLOAT3(-0.1f, -0.1f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
{ XMFLOAT3(-0.1f,  -0.1f, 0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.1f, -0.1f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
{ XMFLOAT3(0.1f,  -0.1f, 0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
{ XMFLOAT3(0.f,  0.1f,  0.f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	 };

const static std::vector<unsigned short> pyramidIndices =
{ {
		2,0,1,//floor
		2,1,3,

		4,1,0,//side
		4,0,2,
		4,3,1,
		4,2,3,
	} };

void makeCraftModel(std::string filename)
{
	
}

void Init()
{
	//init root node(empty)
	auto root = new Root();
	m_objs.push_back(root);

	//init cube
	float pos[3];
	pos[2] = -2;

	pos[0] = 0.f; pos[1] = 1.5f;
	auto cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = -1.f; pos[1] = -.5f; pos[2] = 0.f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = -0.5f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = 0.f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = 0.5f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = -0.25f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = 0.25f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = 1.f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = -0.5f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	pos[0] = 0.5f; pos[1] = -0.5f;
	cube = new Cube(pos);
	m_objs.push_back(cube);
	

	m_objs[6]->Connect(m_objs[5]);
	m_objs[7]->Connect(m_objs[5]);
	m_objs[3]->Connect(m_objs[2]);
	m_objs[4]->Connect(m_objs[2]);
	m_objs[5]->Connect(m_objs[2]);
	m_objs[2]->Connect(m_objs[1]);
	m_objs[8]->Connect(m_objs[1]);
	m_objs[9]->Connect(m_objs[8]);
	m_objs[10]->Connect(m_objs[8]);

	//init model
	auto model = new Model(cubeVertices, cubeIndices);
	m_models.push_back(model);
	m_objs.push_back(model);
	model = new Model(pyramidVertices, pyramidIndices);
	m_models.push_back(model);
	m_objs.push_back(model);
	std::vector<VertexPositionColor> sphereVertices;
	std::vector<unsigned short> sphereIndices;
	Sphere::GetSphere(sphereVertices, sphereIndices);
	model = new Model(sphereVertices, sphereIndices);
	m_models.push_back(model);
	m_objs.push_back(model);

	m_objs[1]->Connect(root);
	m_root = root;

	m_models[0]->Connect(root);
	m_models[1]->Connect(root);
	m_models[2]->Connect(root);
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

	Init();

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// 启动socket api  
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

	// 创建socket  
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
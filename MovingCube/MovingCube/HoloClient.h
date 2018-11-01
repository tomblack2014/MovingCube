#pragma once

#include <ppltasks.h>
#include <sstream>
#include <list>

class HoloClient
{
private:
	Windows::Networking::Sockets::StreamSocketListener^ streamSocketListener;
	Windows::Networking::Sockets::StreamSocket^ streamSocket;

	bool m_connected;
	Windows::Foundation::IAsyncAction^ m_status;

	std::list<Windows::Foundation::Numerics::float3> m_posList;
public:
	HoloClient();

	bool Start();

	bool Connect();

	inline bool IsConnected() { return m_connected; };

	inline int GetStatus() { return (int)m_status->Status; };

	inline std::list<Windows::Foundation::Numerics::float3> GetPosList() { return m_posList; };
	inline void SetPosList(std::list<Windows::Foundation::Numerics::float3> pl) { m_posList = pl; };

	int Send();
};
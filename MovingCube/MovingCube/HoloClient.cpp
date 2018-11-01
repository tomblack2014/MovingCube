#include "pch.h"
#include "HoloClient.h"

using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

HoloClient::HoloClient()
{
	m_connected = false;
}

bool HoloClient::Start()
{
	try
	{
		// Create the StreamSocket and establish a connection to the echo server.
		this->streamSocket = ref new Windows::Networking::Sockets::StreamSocket();

		// The server hostname that we will be establishing a connection to. In this example, the server and client are in the same process.
		auto hostName = ref new Windows::Networking::HostName(L"192.168.31.12");
		//IAsyncAction^ connRes;
		Concurrency::create_task(this->streamSocket->ConnectAsync(hostName, L"1337")).then(
			[=](Concurrency::task< void >)
		{
			// Send a request to the echo server.
			auto dataWriter = ref new DataWriter(this->streamSocket->OutputStream);
			auto request = ref new Platform::String(L"Hello, World!");
			int len = request->Length();
			dataWriter->WriteUInt32(request->Length());
			dataWriter->WriteString(request);

			Concurrency::create_task(dataWriter->StoreAsync()).then(
				[=](Concurrency::task< unsigned int >)
			{
				Concurrency::create_task(dataWriter->FlushAsync()).then(
					[=](Concurrency::task< bool >)
				{
					dataWriter->DetachStream();

					// Read data from the echo server.
					auto dataReader = ref new DataReader(this->streamSocket->InputStream);
					Concurrency::create_task(dataReader->LoadAsync(sizeof(unsigned int))).then(
						[=](unsigned int bytesLoaded)
					{
						unsigned int stringLength = dataReader->ReadUInt32();
						Concurrency::create_task(dataReader->LoadAsync(stringLength)).then(
							[=](unsigned int bytesLoaded)
						{
						});
					});
				});
			});
		});
	}
	catch (Platform::Exception^ ex)
	{
		Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
		m_connected = false;
	}

	m_connected = true;
	return m_connected;
}

bool HoloClient::Connect()
{
	try
	{
		// Create the StreamSocket and establish a connection to the echo server.
		this->streamSocket = ref new Windows::Networking::Sockets::StreamSocket();

		// The server hostname that we will be establishing a connection to. In this example, the server and client are in the same process.
		auto hostName = ref new Windows::Networking::HostName(L"192.168.31.12");

		try
		{
			m_status = this->streamSocket->ConnectAsync(hostName, L"1337");

			while (m_status->Status == Windows::Foundation::AsyncStatus::Started) {

			}
			/*Concurrency::create_task(m_status = this->streamSocket->ConnectAsync(hostName, L"1337")).then(
				[](Concurrency::task<void> prevTask) {

			}
			).then([](Concurrency::task<void> t) {
				try {
					t.get();
				}
				catch (Platform::Exception^ exception) {
					auto ex = exception;
				}
				catch (Platform::COMException^ exception) {
					auto ex = exception;
				}
			});*/
		}
		catch (Platform::COMException^ exception) {
			auto ex = exception;
		}
		//Sleep(5000);
		//m_connected = true;
		//task.then(
		//	[=,&task](Concurrency::task<void>) {
		//	try {
		//		task.get();
		//	}
		//	catch (Platform::Exception^ exception) {
		//	//catch (const std::exception& exception){
		//		auto ex = exception;
		//	}
		//}).then(
		//	[=](Concurrency::task< void >)
		//{
		//	m_connected = true;
		//});

		/*Concurrency::create_task(this->streamSocket->ConnectAsync(hostName, L"1337")).then(
			[this](Concurrency::task<bool> prevTask) {
			try {
				prevTask.get();

			}
			catch (Platform::Exception^ exception) {

			}
		}).then(
			[=](Concurrency::task< bool >)
		{
			m_connected = true;
		});*/
	}
	catch (Platform::Exception^ ex)
	{
		Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
		m_connected = false;
	}
	m_connected = true;
	return m_connected;
}

int HoloClient::Send()
{
	auto dataReader = ref new DataReader(this->streamSocket->InputStream);
	Concurrency::create_task(dataReader->LoadAsync(sizeof(unsigned int))).then(
		[=](unsigned int bytesLoaded)
	{
		//first read the num of cubes
		unsigned int stringLength = dataReader->ReadUInt32();
		Concurrency::create_task(dataReader->LoadAsync(stringLength * 3 * sizeof(unsigned int))).then(
			[=](unsigned int bytesLoaded)
		{
			std::list<Windows::Foundation::Numerics::float3> pos;
			for (int i = 0; i < stringLength; i++) {
				Windows::Foundation::Numerics::float3 p;
				for (int j = 0; j < 3; j++) {
					switch (j)
					{
					case 0:
						p.x = dataReader->ReadInt32();
						break;
					case 1:
						p.y = dataReader->ReadInt32();
						break;
					case 2:
						p.z = dataReader->ReadInt32();
						break;
					default:
						break;
					}

				}
				pos.push_back(p);
			}
			this->SetPosList(pos);
		});
	});

	return 0;

	auto dataWriter = ref new DataWriter(this->streamSocket->OutputStream);
	auto request = ref new Platform::String(L"Holo/SceneGraph");
	int len = request->Length();
	try
	{
		//dataWriter->WriteUInt32(request->Length());
		dataWriter->WriteString(request);

		Concurrency::create_task(dataWriter->StoreAsync()).then(
			[=](Concurrency::task< unsigned int >)
		{
			Concurrency::create_task(dataWriter->FlushAsync()).then(
				[=](Concurrency::task< bool >)
			{
				dataWriter->DetachStream();

				//// Read data from the echo server.
				auto dataReader = ref new DataReader(this->streamSocket->InputStream);
				Concurrency::create_task(dataReader->LoadAsync(sizeof(unsigned int))).then(
					[=](unsigned int bytesLoaded)
				{
					//first read the num of cubes
					unsigned int stringLength = dataReader->ReadUInt32();
					Concurrency::create_task(dataReader->LoadAsync(stringLength)).then(
						[=](unsigned int bytesLoaded)
					{
						std::list<Windows::Foundation::Numerics::float3> pos;
						for (int i = 0; i < stringLength; i++) {
							Windows::Foundation::Numerics::float3 p;
							for (int j = 0; j < 3; j++) {
								switch (j)
								{
								case 0:
									p.x = dataReader->ReadInt32();
									break;
								case 1:
									p.y = dataReader->ReadInt32();
									break;
								case 2:
									p.z = dataReader->ReadInt32();
									break;
								default:
									break;
								}
								
							}
							pos.push_back(p);
						}
						this->SetPosList(pos);
					});
				});
			});
		});
	}
	catch (Platform::Exception^ ex)
	{
		Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
		m_connected = false;
	}

	return len;
}
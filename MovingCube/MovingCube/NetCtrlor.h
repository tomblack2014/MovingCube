//#pragma once
//
//#include <ppltasks.h>
//#include <sstream>
//
//using namespace Windows::Foundation;
//using namespace Windows::Storage::Streams;
//using namespace Windows::UI::Core;
//using namespace Windows::UI::Xaml::Navigation;
//
//class NetCtrlor
//{
//private:
//	Windows::Networking::Sockets::StreamSocketListener^ streamSocketListener;
//	Windows::Networking::Sockets::StreamSocket^ streamSocket;
//
//private:
//	void StartServer()
//	{
//		try
//		{
//			this->streamSocketListener = ref new Windows::Networking::Sockets::StreamSocketListener();
//
//			// The ConnectionReceived event is raised when connections are received.
//			streamSocketListener->ConnectionReceived += ref new TypedEventHandler<Windows::Networking::Sockets::StreamSocketListener^, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^>(this->StreamSocketListener_ConnectionReceived);
//
//			// Start listening for incoming TCP connections on the specified port. You can specify any port that's not currently in use.
//			// Every protocol typically has a standard port number. For example, HTTP is typically 80, FTP is 20 and 21, etc.
//			// For this example, we'll choose an arbitrary port number.
//			Concurrency::create_task(streamSocketListener->BindServiceNameAsync(L"1337")).then(
//				[=]
//			{
//				this->serverListBox->Items->Append(L"server is listening...");
//			});
//		}
//		catch (Platform::Exception^ ex)
//		{
//			Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
//			this->serverListBox->Items->Append(webErrorStatus.ToString() != L"Unknown" ? webErrorStatus.ToString() : ex->Message);
//		}
//	}
//
//	void StreamSocketListener_ConnectionReceived(Windows::Networking::Sockets::StreamSocketListener^ sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ args)
//	{
//		try
//		{
//			auto dataReader = ref new DataReader(args->Socket->InputStream);
//
//			Concurrency::create_task(dataReader->LoadAsync(sizeof(unsigned int))).then(
//				[=](unsigned int bytesLoaded)
//			{
//				unsigned int stringLength = dataReader->ReadUInt32();
//				Concurrency::create_task(dataReader->LoadAsync(stringLength)).then(
//					[=](unsigned int bytesLoaded)
//				{
//					Platform::String^ request = dataReader->ReadString(bytesLoaded);
//					this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
//						[=]
//					{
//						std::wstringstream wstringstream;
//						wstringstream << L"server received the request: \"" << request->Data() << L"\"";
//						this->serverListBox->Items->Append(ref new Platform::String(wstringstream.str().c_str()));
//					}));
//
//					// Echo the request back as the response.
//					auto dataWriter = ref new DataWriter(args->Socket->OutputStream);
//					dataWriter->WriteUInt32(request->Length());
//					dataWriter->WriteString(request);
//					Concurrency::create_task(dataWriter->StoreAsync()).then(
//						[=](unsigned int)
//					{
//						dataWriter->DetachStream();
//
//						this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
//							[=]()
//						{
//							std::wstringstream wstringstream;
//							wstringstream << L"server sent back the response: \"" << request->Data() << L"\"";
//							this->serverListBox->Items->Append(ref new Platform::String(wstringstream.str().c_str()));
//						}));
//
//						delete this->streamSocketListener;
//						this->streamSocketListener = nullptr;
//
//						this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]() {this->serverListBox->Items->Append(L"server closed its socket"); }));
//					});
//				});
//			});
//		}
//		catch (Platform::Exception^ ex)
//		{
//			Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
//			this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]() {this->serverListBox->Items->Append(webErrorStatus.ToString() != L"Unknown" ? webErrorStatus.ToString() : ex->Message); }));
//		}
//	}
//
//	void StartClient()
//	{
//		try
//		{
//			// Create the StreamSocket and establish a connection to the echo server.
//			this->streamSocket = ref new Windows::Networking::Sockets::StreamSocket();
//
//			// The server hostname that we will be establishing a connection to. In this example, the server and client are in the same process.
//			auto hostName = ref new Windows::Networking::HostName(L"localhost");
//
//			this->clientListBox->Items->Append(L"client is trying to connect...");
//
//			Concurrency::create_task(this->streamSocket->ConnectAsync(hostName, L"1337")).then(
//				[=](Concurrency::task< void >)
//			{
//				this->clientListBox->Items->Append(L"client connected");
//
//				// Send a request to the echo server.
//				auto dataWriter = ref new DataWriter(this->streamSocket->OutputStream);
//				auto request = ref new Platform::String(L"Hello, World!");
//				dataWriter->WriteUInt32(request->Length());
//				dataWriter->WriteString(request);
//
//				Concurrency::create_task(dataWriter->StoreAsync()).then(
//					[=](Concurrency::task< unsigned int >)
//				{
//					std::wstringstream wstringstream;
//					wstringstream << L"client sent the request: \"" << request->Data() << L"\"";
//					this->clientListBox->Items->Append(ref new Platform::String(wstringstream.str().c_str()));
//
//					Concurrency::create_task(dataWriter->FlushAsync()).then(
//						[=](Concurrency::task< bool >)
//					{
//						dataWriter->DetachStream();
//
//						// Read data from the echo server.
//						auto dataReader = ref new DataReader(this->streamSocket->InputStream);
//						Concurrency::create_task(dataReader->LoadAsync(sizeof(unsigned int))).then(
//							[=](unsigned int bytesLoaded)
//						{
//							unsigned int stringLength = dataReader->ReadUInt32();
//							Concurrency::create_task(dataReader->LoadAsync(stringLength)).then(
//								[=](unsigned int bytesLoaded)
//							{
//								Platform::String^ response = dataReader->ReadString(bytesLoaded);
//								this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
//									[=]
//								{
//									std::wstringstream wstringstream;
//									wstringstream << L"client received the response: \"" << response->Data() << L"\"";
//									this->clientListBox->Items->Append(ref new Platform::String(wstringstream.str().c_str()));
//
//									delete this->streamSocket;
//									this->streamSocket = nullptr;
//
//									this->clientListBox->Items->Append(L"client closed its socket");
//								}));
//							});
//						});
//					});
//				});
//			});
//		}
//		catch (Platform::Exception^ ex)
//		{
//			Windows::Networking::Sockets::SocketErrorStatus webErrorStatus = Windows::Networking::Sockets::SocketError::GetStatus(ex->HResult);
//			this->serverListBox->Items->Append(webErrorStatus.ToString() != L"Unknown" ? webErrorStatus.ToString() : ex->Message);
//		}
//	}
//};
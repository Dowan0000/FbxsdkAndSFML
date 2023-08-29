#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <vector>

#include <Windows.h>
#include <tchar.h>

#include <fbxsdk.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")

using namespace std;


void ConnectFbxReader(SOCKET recvMotionSocket, SOCKET EventSocket, int FbxReaderIdx, int actorIdx, char* RecvBuf)
{
	SOCKET FbxReaderSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN FbxReaderAddr;
	ZeroMemory(&FbxReaderAddr, sizeof(FbxReaderAddr));
	FbxReaderAddr.sin_family = AF_INET;
	FbxReaderAddr.sin_port = htons(FbxReaderIdx);
	FbxReaderAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(FbxReaderSocket, (SOCKADDR*)&FbxReaderAddr, sizeof(FbxReaderAddr)) == SOCKET_ERROR)
		cout << "FbxReaderSocket Connect Error" << endl;
	cout << "FbxReaderSocket Connect Success" << endl;
	cout << "Connect Port : " << FbxReaderIdx << endl;

	char SendActorIdxBuf[4] = { 0, 0, 0, actorIdx };
	send(FbxReaderSocket, SendActorIdxBuf, sizeof(SendActorIdxBuf), 0);

	while (true)
	{
		char recvBuf[4];
		recv(recvMotionSocket, recvBuf, sizeof(recvBuf), 0);

		// recvBuf[0] : 0 -> Motion
		// recvBuf[0] : 2 -> Event

		if (recvBuf[0] == 0)
		{
			send(FbxReaderSocket, recvBuf, sizeof(recvBuf), 0);
		}
		else if (recvBuf[0] == 2)
		{
			char SendEventBuf[] = { 80,67,76,83,48,48,48,52,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,0,25,RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 30, 0, 0, 0, 40, 0 };

			if (recvBuf[1] == 1)
			{
				SendEventBuf[49] = 11;
				send(EventSocket, SendEventBuf, sizeof(SendEventBuf), 0);
				cout << "Event Num : 11 " << endl;
			}
			else if (recvBuf[1] == 2)
			{
				SendEventBuf[49] = 12;
				send(EventSocket, SendEventBuf, sizeof(SendEventBuf), 0);
				cout << "Event Num : 12 " << endl;
			}
			else if (recvBuf[1] == 3)
			{
				SendEventBuf[49] = 13;
				send(EventSocket, SendEventBuf, sizeof(SendEventBuf), 0);
				cout << "Event Num : 13 " << endl;
			}
			else
				return;
		}
	}

}

int main()
{
	// FbxReader exe


	//TCHAR exePath[] = _T("C:/Project2/FbxReader/FbxReader.exe");

	vector<wstring> exePaths;
	exePaths.push_back(_T("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader.exe"));
	exePaths.push_back(_T("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader2.exe"));
	exePaths.push_back(_T("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader3.exe"));
	exePaths.push_back(_T("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader4.exe"));
	//exePaths.push_back(_T("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader5.exe"));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	for (auto& Path : exePaths)
	{
		const wchar_t* exePath = Path.c_str();

		if (CreateProcess(nullptr, (LPWSTR)exePath, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			cout << "CreateProcess Success" << endl;
		}
		else
		{
			cout << "CreateProcess Error" << endl;
			return 0;
		}
	}
	


	/*--------------------------------*/


	WSAData wasData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wasData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(20001); //20001
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		cout << "Connect Error" << endl;

	cout << "Connect Success" << endl;

	char SendPCLSBuf[] = { 80,67,76,83,48,48,48,48,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,0,0 };
	send(ServerSocket, SendPCLSBuf, sizeof(SendPCLSBuf), 0);
	cout << "SendLen : " << sizeof(SendPCLSBuf) << endl;

	char RecvBuf[46];
	int RecvLen = recv(ServerSocket, RecvBuf, sizeof(RecvBuf), 0);
	cout << "RecvBuf : " << RecvBuf << endl;

	int32_t PCLS_connection_number = (static_cast<int32_t>(RecvBuf[34]) << 24) |
		(static_cast<int32_t>(RecvBuf[35]) << 16) |
		(static_cast<int32_t>(RecvBuf[36]) << 8) |
		static_cast<int32_t>(RecvBuf[37]);

	cout << "PCLS_connection_number : " << PCLS_connection_number << endl;

	
	uint16_t PCLS_event_data_service_port16 = (uint16_t(RecvBuf[42]) << 8) | uint16_t(RecvBuf[43]);
	int32_t PCLS_event_data_service_port = static_cast<int32_t>(PCLS_event_data_service_port16);

	//cout << "PCLS_event_data_service_port : " << PCLS_event_data_service_port << endl;

	/* LSCS Event */

	SOCKET EventSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN EventAddr;
	ZeroMemory(&EventAddr, sizeof(EventAddr));
	EventAddr.sin_family = AF_INET;
	EventAddr.sin_port = htons(23001);
	EventAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(EventSocket, (SOCKADDR*)&EventAddr, sizeof(EventAddr)) == SOCKET_ERROR)
		cout << "Connect Error" << endl;

	cout << "Event Connect Success" << endl;



	// Listen Socket

	SOCKET recvListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN recvMotionAddr;
	ZeroMemory(&recvMotionAddr, sizeof(recvMotionAddr));
	recvMotionAddr.sin_family = AF_INET;
	recvMotionAddr.sin_port = htons(9999);
	recvMotionAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(recvListenSocket, (SOCKADDR*)&recvMotionAddr, sizeof(recvMotionAddr));
	listen(recvListenSocket, SOMAXCONN);


	int actorIdx = 1;
	int FbxReaderIdx = 5551;

	vector<thread> motionThreads;

	while (true)
	{
		SOCKADDR_IN ClientSockAddr;
		memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
		int ClientSockAddrSize = sizeof(ClientSockAddr);
		SOCKET recvMotionSocket = accept(recvListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrSize);
		cout << "Socket Name : " << recvMotionSocket << endl;
		char SocketNumBuf[] = { actorIdx };
		send(recvMotionSocket, SocketNumBuf, 1, 0);

		motionThreads.push_back(thread(ConnectFbxReader, recvMotionSocket, EventSocket, FbxReaderIdx, actorIdx, RecvBuf));

		actorIdx++;
		FbxReaderIdx++;
	}

	for(auto &thread : motionThreads)
		thread.join();

	


	


	return 0;
}
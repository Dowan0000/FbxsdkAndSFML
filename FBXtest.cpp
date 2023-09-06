#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <vector>

#include <cstdlib>

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
	WSAData wasData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wasData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(2400); //20001
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
	EventAddr.sin_port = htons(2402);
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

	SOCKADDR_IN PerformerSockAddr;
	memset(&PerformerSockAddr, 0, sizeof(PerformerSockAddr));
	int ClientSockAddrSize = sizeof(PerformerSockAddr);
	SOCKET PerformerSocket = accept(recvListenSocket, (SOCKADDR*)&PerformerSockAddr, &ClientSockAddrSize);

	char PerformerRecvBuf[12];
	int recvBytes = recv(PerformerSocket, PerformerRecvBuf, sizeof(PerformerRecvBuf), 0);

	cout << "recvBytes : " << recvBytes << endl;

	for(int i = 0; i < recvBytes; i++)
		cout << "PerformerRecvBuf[" << i << "] : " << (int)PerformerRecvBuf[i] << endl;

	
	/*     FbxReader exe     */

	vector<const char*> exePaths;
	exePaths.push_back("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader.exe");
	exePaths.push_back("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader2.exe");
	exePaths.push_back("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader3.exe");
	exePaths.push_back("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader4.exe");
	exePaths.push_back("C:/Users/monsterslab/Desktop/Project2/FbxReader/FbxReader5.exe");

	for (int i = 0; i < PerformerRecvBuf[0]; i++)
	{
		// Convert narrow char path to wide char path
		int widePathLen = MultiByteToWideChar(CP_UTF8, 0, exePaths[i], -1, NULL, 0);
		wchar_t* wideExePath = new wchar_t[widePathLen];
		MultiByteToWideChar(CP_UTF8, 0, exePaths[i], -1, wideExePath, widePathLen);

		STARTUPINFO startupInfo = { sizeof(startupInfo) };
		PROCESS_INFORMATION processInfo;

		if (CreateProcess(NULL, wideExePath, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
			std::wcout << L"External program started successfully." << std::endl;
			// You can proceed with your main program logic here while the external program runs.

			// Close handles to avoid resource leaks
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}
		else {
			std::wcerr << L"Failed to start external program." << std::endl;
		}

		// Clean up the wide string memory
		delete[] wideExePath;
	}


	/* Connect to FbxReader */

	int FbxReaderIdx = 5551;

	//vector<SOCKET> sockets;

	for (int i = 0; i < (int)PerformerRecvBuf[0]; i++)
	{
		SOCKET recvPerformerSocket = socket(AF_INET, SOCK_STREAM, 0);

		SOCKADDR_IN recvPerformerAddr;
		ZeroMemory(&recvPerformerAddr, sizeof(recvPerformerAddr));
		recvPerformerAddr.sin_family = AF_INET;
		recvPerformerAddr.sin_port = htons(FbxReaderIdx);
		recvPerformerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (connect(recvPerformerSocket, (SOCKADDR*)&recvPerformerAddr, sizeof(recvPerformerAddr)) == SOCKET_ERROR)
			cout << "Connect Error" << endl;

		cout << "Connect Success" << endl;

		char PerformerSendBuf[2] = { PerformerRecvBuf[2 + (i*2)], PerformerRecvBuf[3 + (i*2)]};
		send(recvPerformerSocket, PerformerSendBuf, sizeof(PerformerSendBuf), 0);

		//sockets.push_back(recvPerformerSocket);
		FbxReaderIdx++;
	}

	while (true)
	{

	}

	
	


	


	return 0;
}
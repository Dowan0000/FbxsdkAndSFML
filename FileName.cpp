#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "WS2_32.lib")

using namespace std;


void RecvThread()
{
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.205");
	ServerSockAddr.sin_port = htons(10000);

	if (SOCKET_ERROR == connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr)))
	{
		cout << "Connect Error" << endl;
		return;
	}

	char byteArray[] = { 67, 76, 83, 77, 48, 48, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 0, 0 };

	int SendBytes = send(ServerSocket, byteArray, sizeof(byteArray), 0);

	cout << "SendBytes : " << SendBytes << endl;


	char Buffer[1024];

	int RecvBytes = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
	cout << Buffer << endl;


	std::string lscs_ip = std::to_string(Buffer[34]) + "." + std::to_string(Buffer[35]) + "." +
		std::to_string(Buffer[36]) + "." + std::to_string(Buffer[37]);

	uint16_t lscs_port16 = static_cast<uint16_t>(Buffer[38]) << 8;
	lscs_port16 |= Buffer[39];
	int32_t lscs_port = static_cast<int32_t>(lscs_port16);

	std::string azs_ip = std::to_string(Buffer[40]) + "." + std::to_string(Buffer[41]) + "." +
		std::to_string(Buffer[42]) + "." + std::to_string(Buffer[43]);

	uint16_t azs_port16 = static_cast<uint16_t>(Buffer[44]) << 8;
	azs_port16 |= Buffer[45];
	int32_t azs_port = static_cast<int32_t>(azs_port16);

	// Output the value
	std::cout << "lscs_ip = " << lscs_ip << std::endl;

	std::cout << "lscs_port = " << lscs_port << std::endl;

	std::cout << "azs_ip = " << azs_ip << std::endl;

	std::cout << "azs_port = " << azs_port << std::endl;


	/* Lscs */

	SOCKET LscsSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN LscsSockAddr;
	memset(&LscsSockAddr, 0, sizeof(LscsSockAddr));
	LscsSockAddr.sin_family = PF_INET;
	LscsSockAddr.sin_addr.S_un.S_addr = inet_addr(lscs_ip.c_str());
	LscsSockAddr.sin_port = htons(lscs_port);

	if (SOCKET_ERROR == connect(LscsSocket, (SOCKADDR*)&LscsSockAddr, sizeof(LscsSockAddr)))
	{
		cout << "Connect Error" << endl;
		return;
	}


	/* Azs */

	SOCKET AzsSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN AzsSockAddr;
	memset(&AzsSockAddr, 0, sizeof(AzsSockAddr));
	AzsSockAddr.sin_family = PF_INET;
	AzsSockAddr.sin_addr.S_un.S_addr = inet_addr(azs_ip.c_str());
	AzsSockAddr.sin_port = htons(azs_port);

	if (SOCKET_ERROR == connect(AzsSocket, (SOCKADDR*)&AzsSockAddr, sizeof(AzsSockAddr)))
	{
		cout << "Connect Error" << endl;
		return;
	}


	/* LscsData */

	char LscsByte[] = { 65, 67, 67, 83, 48, 48, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 0, 0 };

	

	int LscsBytes = send(LscsSocket, LscsByte, sizeof(LscsByte), 0);

	cout << "LscsBytes : " << LscsBytes << endl;

	char LscsBuffer[1024];

	int LscsRecvBytes = recv(LscsSocket, LscsBuffer, sizeof(LscsBuffer), 0);
	cout << LscsBuffer << endl;

	int32_t Lscs_connection_number = (LscsBuffer[34] << 24) | (LscsBuffer[35] << 16) | (LscsBuffer[36] << 8) | LscsBuffer[37];

	uint16_t motion_data_service_port16 = static_cast<uint16_t>(LscsBuffer[38]) << 8;

	motion_data_service_port16 |= LscsBuffer[39];

	int32_t motion_data_service_port = static_cast<int32_t>(motion_data_service_port16);

	std::cout << "motion_data_service_port = " << motion_data_service_port << std::endl;


	/* LscsMotionData */

	SOCKET LscsMotionSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN LscsMoptionSockAddr;
	memset(&LscsMoptionSockAddr, 0, sizeof(LscsMoptionSockAddr));
	LscsMoptionSockAddr.sin_family = PF_INET;
	LscsMoptionSockAddr.sin_addr.S_un.S_addr = inet_addr(lscs_ip.c_str());
	LscsMoptionSockAddr.sin_port = htons(motion_data_service_port);

	if (SOCKET_ERROR == connect(LscsMotionSocket, (SOCKADDR*)&LscsMoptionSockAddr, sizeof(LscsMoptionSockAddr)))
	{
		cout << "Connect Error" << endl;
		return;
	}

	cout << "Lscs Motion Socket Connect" << endl;





	/* AzsData */

	char AzsByte[] = {65, 67, 90, 83, 48, 48, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 0, 0 };

	int AzsBytes = send(AzsSocket, AzsByte, sizeof(AzsByte), 0);

	cout << "AzsBytes : " << AzsBytes << endl;

	char AzsBuffer[1024];

	int AzsRecvBytes = recv(AzsSocket, AzsBuffer, sizeof(AzsBuffer), 0);
	cout << "AzsBuffer : " << AzsBuffer << endl;

	int32_t Azs_connection_number = (static_cast<int32_t>(AzsBuffer[34]) << 24) |
		(static_cast<int32_t>(AzsBuffer[35]) << 16) |
		(static_cast<int32_t>(AzsBuffer[36]) << 8) |
		static_cast<int32_t>(AzsBuffer[37]);

	uint16_t Azs_motion_data_service_port16 = (uint16_t(AzsBuffer[40]) << 8) | uint16_t(AzsBuffer[41]);
	int32_t Azs_motion_data_service_port = static_cast<int32_t>(Azs_motion_data_service_port16);

	std::cout << "connection_number = " << Azs_connection_number << std::endl;
	std::cout << "Azs_motion_data_service_port = " << Azs_motion_data_service_port << std::endl;


	/* AzsMotionData */

	SOCKET AzsMotionSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN AzsMoptionSockAddr;
	memset(&AzsMoptionSockAddr, 0, sizeof(AzsMoptionSockAddr));
	AzsMoptionSockAddr.sin_family = PF_INET;
	AzsMoptionSockAddr.sin_addr.S_un.S_addr = inet_addr(azs_ip.c_str());
	AzsMoptionSockAddr.sin_port = htons(Azs_motion_data_service_port);

	if (SOCKET_ERROR == connect(AzsMotionSocket, (SOCKADDR*)&AzsMoptionSockAddr, sizeof(AzsMoptionSockAddr)))
	{
		cout << "Connect Error" << endl;
		return;
	}

	cout << "Azs Motion Socket Connect" << endl;

	INT64 request_time = 0;

	char AzsMotionByte[38] = { 65, 67, 90, 83, 48, 48, 49, 52, 0, 0, 0, 0, 0, 0, 0, 0, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 0, 4, 0, 0, 0, 0 };

	// AzsMotionByte[8] ~ AzsMotionByte[15]에 request_time을 넣는다
	AzsMotionByte[8] = (request_time >> 56) & 0xFF;
	AzsMotionByte[9] = (request_time >> 48) & 0xFF;
	AzsMotionByte[10] = (request_time >> 40) & 0xFF;
	AzsMotionByte[11] = (request_time >> 32) & 0xFF;
	AzsMotionByte[12] = (request_time >> 24) & 0xFF;
	AzsMotionByte[13] = (request_time >> 16) & 0xFF;
	AzsMotionByte[14] = (request_time >> 8) & 0xFF;
	AzsMotionByte[15] = request_time & 0xFF;

	// Azs_connection_number를 바이트로 변환해서 AzsMotionByte[34] ~ AzsMotionByte[37]에 넣는다
	AzsMotionByte[34] = (Azs_connection_number >> 24) & 0xFF;
	AzsMotionByte[35] = (Azs_connection_number >> 16) & 0xFF;
	AzsMotionByte[36] = (Azs_connection_number >> 8) & 0xFF;
	AzsMotionByte[37] = Azs_connection_number & 0xFF;
	
	
	int AzsMotionBytes = send(AzsMotionSocket, AzsMotionByte, sizeof(AzsMotionByte), 0);

	request_time++;

	cout << "AzsMotionSendBytes : " << AzsMotionBytes << endl;
	cout << "AzsMotionSendMessage : " << AzsMotionByte << endl;


	char AzsMotionRecvByte[1024];
	recv(AzsMotionSocket, AzsMotionRecvByte, sizeof(AzsMotionRecvByte), 0);

	cout << "AzsMotionRecvByte : " << AzsMotionRecvByte << endl;
	cout << "AzsMotionRecvMessage : " << AzsMotionRecvByte << endl;


	char AzsHeaderBuffer[34] = { 65, 67, 90, 83, 48, 48, 48, 51, 0, 0, 0, 0, 0, 0, 0, 0, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 1, 184 };

	/*
	while (true)
	{
		char AzsMotionSendBuffer[474];
		memset(AzsMotionSendBuffer, 0, sizeof(AzsMotionSendBuffer));
		
		// AzsMotionSendBuffer[0] ~ AzsMotionSendBuffer[33] 에 65, 67, 90, 83, 48, 48, 48, 51, 0, 0, 0, 0, 0, 0, 0, 1, 50, 48, 50, 50, 49, 49, 50, 50, 49, 51, 52, 53, 50, 53, 0, 0, 1, 184, 를 넣는다
		for (int i = 0; i < 34; i++)
		{
			AzsMotionSendBuffer[i] = AzsHeaderBuffer[i];
		}

		// AzsMotionSendBuffer[8] ~ AzsMotionSendBuffer[15]에 request_time을 넣는다
		AzsMotionSendBuffer[8] = (request_time >> 56) & 0xFF;
		AzsMotionSendBuffer[9] = (request_time >> 48) & 0xFF;
		AzsMotionSendBuffer[10] = (request_time >> 40) & 0xFF;
		AzsMotionSendBuffer[11] = (request_time >> 32) & 0xFF;
		AzsMotionSendBuffer[12] = (request_time >> 24) & 0xFF;
		AzsMotionSendBuffer[13] = (request_time >> 16) & 0xFF;
		AzsMotionSendBuffer[14] = (request_time >> 8) & 0xFF;
		AzsMotionSendBuffer[15] = request_time & 0xFF;

		// AzsMotionSendBuffer[34] ~ AzsMotionSendBuffer[37] 에 connection_number를 넣는다
		AzsMotionSendBuffer[34] = (Azs_connection_number >> 24) & 0xFF;
		AzsMotionSendBuffer[35] = (Azs_connection_number >> 16) & 0xFF;
		AzsMotionSendBuffer[36] = (Azs_connection_number >> 8) & 0xFF;
		AzsMotionSendBuffer[37] = Azs_connection_number & 0xFF;

		// AzsMotionSendBuffer[38], AzsMotionSendBuffer[39]에 0, 22를 넣는다
		AzsMotionSendBuffer[38] = 0;
		AzsMotionSendBuffer[39] = 22;

		// AzsMotionSendBuffer[40] 부터 352개의 데이터에 0을 넣는다
		for (int i = 40; i < 392; i++)
		{
			AzsMotionSendBuffer[i] = 0;
		}

		//AzsMotionSendBuffer[393] ~ AzsMotionSendBuffer[395]에 string "PRE"를 넣는다
		//AzsMotionSendBuffer[393] = 80;
		//AzsMotionSendBuffer[394] = 82;
		//AzsMotionSendBuffer[395] = 69;

		// AzsMotionSendBuffer[396] 부터 나머지 데이터에 0을 넣는다
		for (int i = 393; i < 474; i++)
		{
			AzsMotionSendBuffer[i] = 0;
		}

		int AzsMotionSendSize = send(AzsMotionSocket, AzsMotionSendBuffer, sizeof(AzsMotionSendBuffer), 0);

		request_time++;

		cout << "AzsMotionSendSize : " << AzsMotionSendSize << endl;


		char AzsMotionRecvBuffer[1024];

		int AzsMotionRecvBytes = recv(AzsMotionSocket, AzsMotionRecvBuffer, sizeof(AzsMotionRecvBuffer), 0);
		cout << "AzsRecvMotionBuffer : " << AzsMotionRecvBuffer << endl;
		cout << "AzsRecvMotionRecvBytes : " << AzsMotionRecvBytes << endl;
		

		this_thread::sleep_for(1s);
	}
	*/

	
	while (true)
	{
		char LscsMotionBuffer[1024];
		recv(LscsMotionSocket, LscsMotionBuffer, sizeof(LscsMotionBuffer), 0);

		cout << LscsMotionBuffer << endl;
	}
	
}



int main_1()
{
	
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);
	
	vector<thread> threads;
	
	for(int i = 0; i < 5; i++)
		threads.push_back(thread(RecvThread));
	
	for (auto& th : threads)
		th.join();
	
	

	WSACleanup();

	return 0;
}
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>
#include <thread>
#include <mutex>

#include <fbxsdk.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")


using namespace std;

int Reset = 0;

void SendMotion(FbxScene* scene, SOCKET MotionSocket, char* RecvBuf)
{
	int Reset = 0;
	char SendMotionBuf[] = { 80,67,76,83,48,48,48,50,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,1,100,RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37],0,1,197,169,24,0,70,17,148,0,199,6,147,0,0,21,  61,143,92,41,128,0,0,0,63,128,0,0,60,163,215,10,189,184,81,236,62,5,30,184,63,125,112,164,188,35,215,10,61,35,215,10,62,66,143,92,63,122,225,72,61,143,92,41,60,35,215,10,60,245,194,143,63,128,0,0,61,184,81,236,189,184,81,236,189,245,194,143,63,125,112,164,61,76,204,205,61,117,194,143,190,66,143,92,63,122,225,72,60,245,194,143,0,0,0,0,189,143,92,41,63,128,0,0,60,163,215,10,62,56,81,236,188,245,194,143,63,122,225,72,60,245,194,143,62,66,143,92,188,163,215,10,63,122,225,72,60,245,194,143,62,76,204,205,188,163,215,10,63,122,225,72,61,35,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,25,153,154,60,35,215,10,63,125,112,164,60,35,215,10,62,76,204,205,190,56,81,236,63,117,194,143,60,35,215,10,60,245,194,143,191,33,71,174,63,69,30,184,61,143,92,41,62,128,0,0,191,25,153,154,63,53,194,143,190,133,30,184,61,184,81,236,190,194,143,92,63,102,102,102,190,66,143,92,62,97,71,174,62,15,92,41,63,117,194,143,61,184,81,236,62,235,133,31,63,12,204,205,63,25,153,154,62,174,20,123,62,97,71,174,63,35,215,10,63,48,163,215,62,138,61,113,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr)
		return;


	int childCount = rootNode->GetChildCount();
	std::cout << "childCount : " << childCount << endl;


	//cout << "<<<<<" << childNode->GetName() << ">>>>>" << endl;

	FbxMesh* mesh = rootNode->GetMesh();
	if (mesh == nullptr)
	{
		std::cout << "No Mesh" << endl;
	}


	// �ִϸ��̼� ����
	FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(0);
	if (animStack)
		std::cout << "animStack : " << animStack->GetName() << endl;
	else
	{
		std::cout << "animStack : " << "No AnimStack" << endl;
		return;
	}

	FbxString animStackName = animStack->GetName();
	FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	FbxLongLong duration = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30) + 1;
	std::cout << "duration : " << duration << endl;

	int num = 0;


	for (FbxLongLong frame = start.GetFrameCount(FbxTime::eFrames30); frame <= end.GetFrameCount(FbxTime::eFrames30); ++frame)
	{
		// Rotation value per born
		FbxTime time;
		time.SetFrame(frame, FbxTime::eFrames30);


		// 1. Get Mesh
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* childNode = rootNode->GetChild(i);
			//cout << "<<<<<" << childNode->GetName() << ">>>>>" << endl;

			FbxMesh* mesh = childNode->GetMesh();
			if (mesh == nullptr)
			{
				//cout << "No Mesh" << endl;
				continue;
			}


			int deformerCount = mesh->GetDeformerCount();
			for (int j = 0; j < deformerCount; ++j)
			{
				FbxDeformer* deformer = mesh->GetDeformer(j);
				FbxSkin* skin = (FbxSkin*)deformer;
				if (skin == nullptr)
					continue;

				
				//std::cout << num;

				//num++;
				
				
				int clusterCount = skin->GetClusterCount();
				//cout << "clusterCount : " << clusterCount << endl;


				for (int k = 0; k < clusterCount; ++k)
				{
					FbxCluster* cluster = skin->GetCluster(k);
					FbxNode* linkNode = cluster->GetLink();
					if (linkNode == nullptr)
						continue;

					FbxAMatrix matrix = cluster->GetLink()->EvaluateGlobalTransform(time);
					FbxVector4 translation = matrix.GetT();
					FbxVector4 rotation = matrix.GetR();
					
					

					// matrix�� GetQ ���ϱ�
					FbxQuaternion quaternion = matrix.GetQ();

					//cout << "Translation : " << translation[0] << ", " << translation[1] << ", " << translation[2] << endl;
					//cout << "Rotation : " << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << endl;

					//printf("Frame : %d , nodeName : %s, Translation : %f, %f, %f\n", (int)frame, linkNode->GetName(), translation[0], translation[1], translation[2]);
					//printf("Num : %d , Frame : %d , nodeName : %s, Rotation : %.2f,%.2f,%.2f\n", k, (int)frame, linkNode->GetName(), rotation[0], rotation[1], rotation[2]);
					//printf("Frame : %d , nodeName : %s, Quaternion : %f, %f, %f, %f\n", (int)frame, linkNode->GetName(), quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
					
					// k == 1 || k == 3 || k == 4 || k == 5 || k == 18 || k == 19 || k == 21 || k == 33 || k == 34 || k == 35 || k == 36 || k == 37 || k == 38 || k == 49 || k == 50 || k == 51 || k == 55 || k == 77 || k == 78 || k == 79 || k == 83
					// k == 2 || k == 4 || k == 5 || k == 6 || k == 28 || k == 29 || k == 32 || k == 52 || k == 53 || k == 54 || k == 55 || k == 56 || k == 57 || k == 73 || k == 74 || k == 75 || k == 81 || k == 111 || k == 112 || k == 113 || k == 119
					// k == 0 || k == 1 || k == 2 || k == 3 || k == 4 || k == 5 || k == 6 || k == 7 || k == 8 || k == 9 || k == 10 || k == 31 || k == 32 || k == 33 || k == 34 || k == 55 || k == 56 || k == 57 || k == 60 || k == 61 || k == 62
					// k == 1 || k == 2 || k == 3 || k == 4 || k == 5 || k == 6 || k == 7 || k == 8 || k == 9 || k == 10 || k == 11 || k == 32 || k == 33 || k == 34 || k == 35 || k == 56 || k == 57 || k == 58 || k == 61 || k == 62 || k == 63
					if(k == 2 || k == 3 || k == 4 || k == 5 || k == 6 || k == 7 || k == 8 || k == 9 || k == 27 || k == 28 || k == 66 || k == 67 || k == 68 || k == 69 || k == 89 || k == 90 || k == 92 || k == 95 || k == 96 || k == 98 || k == 100)
					{
						//printf("nodeName : %s, Quaternion : %f, %f, %f, %f\n", linkNode->GetName(), quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
						if (k == 2)
						{
							//printf("%.2f,%.2f,%.2f", translation[0], translation[1], translation[2]);
							//printf("Frame : %d , nodeName : %s, Translation : %f, %f, %f\n", (int)frame, linkNode->GetName(), translation[0], translation[1], translation[2]);
						
							for (int i = 0; i < 3; i++)
							{
								float Trans = translation[i];
								unsigned char Bytes[4];
								memcpy(Bytes, &Trans, 4);

								//SendBuf�� 40~51��° ����Ʈ�� ����
								SendMotionBuf[43 + i * 4] = Bytes[0];
								SendMotionBuf[42 + i * 4] = Bytes[1];
								SendMotionBuf[41 + i * 4] = Bytes[2];
								SendMotionBuf[40 + i * 4] = Bytes[3];

							}							

							//for (int i = 0; i < 4; i++)
							//{
							//	//cout << "quaternion : " << quaternion[i] << endl;
							//	float Quat = quaternion[i];
							//	unsigned char Bytes[4];
							//	memcpy(Bytes, &Quat, 4);


							//	//SendBuf�� 54~389��° ����Ʈ�� ����
							//	SendBuf[54 + i * 4 + (Reset * 16)] = Bytes[0];
							//	SendBuf[55 + i * 4 + (Reset * 16)] = Bytes[1];
							//	SendBuf[56 + i * 4 + (Reset * 16)] = Bytes[2];
							//	SendBuf[57 + i * 4 + (Reset * 16)] = Bytes[3];


							//	unsigned int combinedInt = 0;
							//	combinedInt |= Bytes[0];
							//	combinedInt |= (Bytes[1] << 8);
							//	combinedInt |= (Bytes[2] << 16);
							//	combinedInt |= (Bytes[3] << 24);

							//	float restoredFloat = *reinterpret_cast<float*>(&combinedInt);

							//	//std::cout << "Restored float: " << restoredFloat << std::endl;
							//}

						}

						//printf(",%.4f,%.4f,%.4f,%.4f\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
						//printf("Num : %d , Frame : %d , nodeName : %s, Rotation : %.2f,%.2f,%.2f\n", k, (int)frame, linkNode->GetName(), rotation[0], rotation[1], rotation[2]);
						
						//quaternion�� 4����Ʈ�� �ɰ� �� SendBuf�� 54~389�� ����Ʈ�� ����
						for (int i = 0; i < 4; i++)
						{
							float Quat = quaternion[i];
							unsigned char Bytes[4];
							memcpy(Bytes, &Quat, 4);

							
							//SendBuf�� 54~389��° ����Ʈ�� ����
							SendMotionBuf[57 + i * 4 + (Reset * 16)] = Bytes[0];
							SendMotionBuf[56 + i * 4 + (Reset * 16)] = Bytes[1];
							SendMotionBuf[55 + i * 4 + (Reset * 16)] = Bytes[2];
							SendMotionBuf[54 + i * 4 + (Reset * 16)] = Bytes[3];


							unsigned int combinedInt = 0;
							combinedInt |= Bytes[0];
							combinedInt |= (Bytes[1] << 8);
							combinedInt |= (Bytes[2] << 16);
							combinedInt |= (Bytes[3] << 24);

							float restoredFloat = *reinterpret_cast<float*>(&combinedInt);

							//std::cout << "Restored float: " << restoredFloat << std::endl;

							//printf("Num : %d , Frame : %d , nodeName : %s, Rotation : %.2f,%.2f,%.2f\n", k, (int)frame, linkNode->GetName(), rotation[0], rotation[1], rotation[2]);
						}

						//printf(",%.2f,%.2f,%.2f", rotation[0], rotation[1], rotation[2]);
						//printf("Frame : %d , nodeName : %s, Rotation : %.2f,%.2f,%.2f\n", (int)frame, linkNode->GetName(), rotation[0], rotation[1], rotation[2]);
					
						Reset = Reset + 1;
					}
					
					
					
					//printf(",%.1f,%.1f,%.1f", rotation[0], rotation[1], rotation[2]);
					//printf(",%.1f, %.1f, %.1f", translation[0], translation[1], translation[2]);
					
					
				}
				
				send(MotionSocket, SendMotionBuf, sizeof(SendMotionBuf), 0);
				//cout << "SendBuf : " << SendBuf << endl;

				Reset = 0;

				this_thread::sleep_for(10ms);

				//std::cout << endl;

			}
		}
	}
}

void SendFacial(FbxScene* scene, SOCKET FacialSocket, char* RecvBuf)
{
	int Reset = 0;
	char SendFacialBuf[] = { 80,67,76,83,48,48,48,53,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,0,96,  RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37],0,0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == nullptr)
		return;


	int childCount = rootNode->GetChildCount();
	std::cout << "childCount : " << childCount << endl;


	//cout << "<<<<<" << childNode->GetName() << ">>>>>" << endl;

	FbxMesh* mesh = rootNode->GetMesh();
	if (mesh == nullptr)
	{
		std::cout << "No Mesh" << endl;
	}


	// �ִϸ��̼� ����
	FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(0);
	if (animStack)
		std::cout << "animStack : " << animStack->GetName() << endl;
	else
	{
		std::cout << "animStack : " << "No AnimStack" << endl;
		return;
	}

	FbxString animStackName = animStack->GetName();
	FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	FbxLongLong duration = end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30) + 1;
	std::cout << "duration : " << duration << endl;

	int num = 0;


	for (FbxLongLong frame = start.GetFrameCount(FbxTime::eFrames30); frame <= end.GetFrameCount(FbxTime::eFrames30); ++frame)
	{
		// Rotation value per born
		FbxTime time;
		time.SetFrame(frame, FbxTime::eFrames30);


		// 1. Get Mesh
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* childNode = rootNode->GetChild(i);
			
			//cout << "<<<<<" << childNode->GetName() << ">>>>>" << endl;

			FbxMesh* mesh = childNode->GetMesh();
			if (mesh == nullptr)
			{
				//cout << "No Mesh" << endl;
				continue;
			}


			FbxBlendShape* blendShape = static_cast<FbxBlendShape*>(mesh->GetDeformer(0, FbxDeformer::eBlendShape));

			// Get the number of channels (shapes) within the blend shape
			int channelCount = blendShape->GetBlendShapeChannelCount();

			// Loop through the channels (shapes) and extract vertex positions
			for (int channelIndex = 0; channelIndex < channelCount; ++channelIndex)
			{
				// fbxanimlayer
				FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();

				// shape�� value�� ������
				FbxAnimCurve* animCurve = mesh->GetShapeChannel(0, channelIndex, animLayer, true);
				if (animCurve)
				{
					int keyCount = animCurve->KeyGetCount();

					FbxAnimCurveKey key = animCurve->KeyGet(frame);
					float value = key.GetValue();

					//cout << "value : " << value << endl;

					if (channelIndex == 1 || channelIndex == 2 || channelIndex == 4 || channelIndex == 6 || channelIndex == 7 || channelIndex == 8 || channelIndex == 9 || channelIndex == 10 || channelIndex == 11 || channelIndex == 12 || channelIndex == 13 || channelIndex == 14 || channelIndex == 15 || channelIndex == 16 || channelIndex == 17 || channelIndex == 18 || channelIndex == 19 || channelIndex == 22 || channelIndex == 23 || channelIndex == 24 || channelIndex == 25 || channelIndex == 26 || channelIndex == 27 || channelIndex == 28 || channelIndex == 29 || channelIndex == 30 || channelIndex == 31 || channelIndex == 32 || channelIndex == 33 || channelIndex == 34 || channelIndex == 35 || channelIndex == 36 || channelIndex == 37 || channelIndex == 38 || channelIndex == 39 || channelIndex == 40 || channelIndex == 41 || channelIndex == 42 || channelIndex == 43 || channelIndex == 44 || channelIndex == 45 || channelIndex == 46 || channelIndex == 47 || channelIndex == 52)
					{
						float FacialData = value;
						unsigned char Bytes[4];
						memcpy(Bytes, &FacialData, 4);

						//SendBuf�� 54~��° ����Ʈ�� ����
						SendFacialBuf[57 + (Reset * 4)] = Bytes[0];
						SendFacialBuf[56 + (Reset * 4)] = Bytes[1];
						SendFacialBuf[55 + (Reset * 4)] = Bytes[2];
						SendFacialBuf[54 + (Reset * 4)] = Bytes[3];

						Reset = Reset + 1;

					}
				}

				int SendFacialBytes = send(FacialSocket, SendFacialBuf, sizeof(SendFacialBuf), 0);
				//cout << SendFacialBytes << endl;
				Reset = 0;

				this_thread::sleep_for(10ms);
			}

			
			////����Ÿ�� ������ ����

			//int morphTargetCount = mesh->GetDeformerCount(FbxDeformer::eBlendShape);
			//cout << "morphTargetCount : " << morphTargetCount << endl;

			//// Loop through the Morph Targets (blend shapes) and extract vertex positions
			//for (int morphIndex = 0; morphIndex < morphTargetCount; ++morphIndex)
			//{
			//	FbxBlendShape* blendShape = static_cast<FbxBlendShape*>(mesh->GetDeformer(morphIndex, FbxDeformer::eBlendShape));

			//	// Get the number of channels (shapes) within the blend shape
			//	int channelCount = blendShape->GetBlendShapeChannelCount();

			//	// Loop through the channels (shapes) and extract vertex positions
			//	for (int channelIndex = 0; channelIndex < channelCount; ++channelIndex)
			//	{
			//		//FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(channelIndex);

			//		// Get the channel (shape) name (morph target name)
			//		//const char* morphTargetName = channel->GetName();
			//		//cout << "morphTargetName : " << morphTargetName << endl;

			//		// Get the weight of the channel (shape)
			//		//double weight = channel->DeformPercent;
			//		//cout << "weight : " << weight << endl;

			//		// Get the number of control points (vertices) in the channel (shape)
			//		//int vertexCount = channel->GetTargetShapeCount();

			//		// fbxanimlayer
			//		FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>();

			//		// shape�� value�� ������
			//		FbxAnimCurve* animCurve = mesh->GetShapeChannel(morphIndex, channelIndex, animLayer, true);
			//		if (animCurve)
			//		{
			//			int keyCount = animCurve->KeyGetCount();

			//			FbxAnimCurveKey key = animCurve->KeyGet(frame);
			//			float value = key.GetValue();

			//			//cout << "value : " << value << endl;

			//			if (channelIndex == 1 || channelIndex == 2 || channelIndex == 4 || channelIndex == 6 || channelIndex == 7 || channelIndex == 8 || channelIndex == 9 || channelIndex == 10 || channelIndex == 11 || channelIndex == 12 || channelIndex == 13 || channelIndex == 14 || channelIndex == 15 || channelIndex == 16 || channelIndex == 17 || channelIndex == 18 || channelIndex == 19 || channelIndex == 22 || channelIndex == 23 || channelIndex == 24 || channelIndex == 25 || channelIndex == 26 || channelIndex == 27 || channelIndex == 28 || channelIndex == 29 || channelIndex == 30 || channelIndex == 31 || channelIndex == 32 || channelIndex == 33 || channelIndex == 34 || channelIndex == 35 || channelIndex == 36 || channelIndex == 37 || channelIndex == 38 || channelIndex == 39 || channelIndex == 40 || channelIndex == 41 || channelIndex == 42 || channelIndex == 43 || channelIndex == 44 || channelIndex == 45 || channelIndex == 46 || channelIndex == 47 || channelIndex == 52)
			//			{
			//				float FacialData = value;
			//				unsigned char Bytes[4];
			//				memcpy(Bytes, &FacialData, 4);

			//				//SendBuf�� 54~��° ����Ʈ�� ����
			//				SendFacialBuf[57 + (Reset * 4)] = Bytes[0];
			//				SendFacialBuf[56 + (Reset * 4)] = Bytes[1];
			//				SendFacialBuf[55 + (Reset * 4)] = Bytes[2];
			//				SendFacialBuf[54 + (Reset * 4)] = Bytes[3];

			//				Reset = Reset + 1;

			//			}
			//		}

			//	}

			//	int SendFacialBytes = send(FacialSocket, SendFacialBuf, sizeof(SendFacialBuf), 0);
			//	//cout << SendFacialBytes << endl;
			//	Reset = 0;

			//	this_thread::sleep_for(10ms);
			//}
			
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

	uint16_t PCLS_motion_data_service_port16 = (uint16_t(RecvBuf[38]) << 8) | uint16_t(RecvBuf[39]);
	int32_t PCLS_motion_data_service_port = static_cast<int32_t>(PCLS_motion_data_service_port16);

	cout << "PCLS_motion_data_service_port : " << PCLS_motion_data_service_port << endl;

	/* LSCS Motion */

	SOCKET MotionSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN MotionAddr;
	ZeroMemory(&MotionAddr, sizeof(MotionAddr));
	MotionAddr.sin_family = AF_INET;
	MotionAddr.sin_port = htons(PCLS_motion_data_service_port);
	MotionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(MotionSocket, (SOCKADDR*)&MotionAddr, sizeof(MotionAddr)) == SOCKET_ERROR)
		cout << "Connect Error" << endl;

	cout << "Motion Connect Success" << endl;

	/*while (true)
	{
		char SendBuf[] = { 80,67,76,83,48,48,48,50,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,1,100,RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37],0,1,197,169,24,0,70,17,148,0,199,6,147,0,0,21,  61,143,92,41,128,0,0,0,63,128,0,0,60,163,215,10,189,184,81,236,62,5,30,184,63,125,112,164,188,35,215,10,61,35,215,10,62,66,143,92,63,122,225,72,61,143,92,41,60,35,215,10,60,245,194,143,63,128,0,0,61,184,81,236,189,184,81,236,189,245,194,143,63,125,112,164,61,76,204,205,61,117,194,143,190,66,143,92,63,122,225,72,60,245,194,143,0,0,0,0,189,143,92,41,63,128,0,0,60,163,215,10,62,56,81,236,188,245,194,143,63,122,225,72,60,245,194,143,62,66,143,92,188,163,215,10,63,122,225,72,60,245,194,143,62,76,204,205,188,163,215,10,63,122,225,72,61,35,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,25,153,154,60,35,215,10,63,125,112,164,60,35,215,10,62,76,204,205,190,56,81,236,63,117,194,143,60,35,215,10,60,245,194,143,191,33,71,174,63,69,30,184,61,143,92,41,62,128,0,0,191,25,153,154,63,53,194,143,190,133,30,184,61,184,81,236,190,194,143,92,63,102,102,102,190,66,143,92,62,97,71,174,62,15,92,41,63,117,194,143,61,184,81,236,62,235,133,31,63,12,204,205,63,25,153,154,62,174,20,123,62,97,71,174,63,35,215,10,63,48,163,215,62,138,61,113,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		send(MotionSocket, SendBuf, sizeof(SendBuf), 0);
		cout << "SendLen : " << sizeof(SendBuf) << endl;
		
		this_thread::sleep_for(1s);
	}*/

	/* ------------------------- */


	/* LSCS Facial*/

	uint16_t PCLS_Facial_data_service_port16 = (uint16_t(RecvBuf[44]) << 8) | uint16_t(RecvBuf[45]);
	int32_t PCLS_Facial_data_service_port = static_cast<int32_t>(PCLS_Facial_data_service_port16);

	//cout << "PCLS_Facial_data_service_port : " << PCLS_Facial_data_service_port << endl;

	/* LSCS Event */

	SOCKET FacialSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN FacialAddr;
	ZeroMemory(&FacialAddr, sizeof(FacialAddr));
	FacialAddr.sin_family = AF_INET;
	FacialAddr.sin_port = htons(24001);
	FacialAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(FacialSocket, (SOCKADDR*)&FacialAddr, sizeof(FacialAddr)) == SOCKET_ERROR)
	{
		cout << "Facial Connect Error" << endl;
		//return 0;
	}

	cout << "Facial Connect Success" << endl;







	/* ----------------------------- */

	
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





	//char SendMotionBuf[] = { 80,67,76,83,48,48,48,50,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,1,100,RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37],0,1,197,169,24,0,70,17,148,0,199,6,147,0,0,21,  61,143,92,41,128,0,0,0,63,128,0,0,60,163,215,10,189,184,81,236,62,5,30,184,63,125,112,164,188,35,215,10,61,35,215,10,62,66,143,92,63,122,225,72,61,143,92,41,60,35,215,10,60,245,194,143,63,128,0,0,61,184,81,236,189,184,81,236,189,245,194,143,63,125,112,164,61,76,204,205,61,117,194,143,190,66,143,92,63,122,225,72,60,245,194,143,0,0,0,0,189,143,92,41,63,128,0,0,60,163,215,10,62,56,81,236,188,245,194,143,63,122,225,72,60,245,194,143,62,66,143,92,188,163,215,10,63,122,225,72,60,245,194,143,62,76,204,205,188,163,215,10,63,122,225,72,61,35,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,46,20,123,128,0,0,0,63,122,225,72,60,163,215,10,62,25,153,154,60,35,215,10,63,125,112,164,60,35,215,10,62,76,204,205,190,56,81,236,63,117,194,143,60,35,215,10,60,245,194,143,191,33,71,174,63,69,30,184,61,143,92,41,62,128,0,0,191,25,153,154,63,53,194,143,190,133,30,184,61,184,81,236,190,194,143,92,63,102,102,102,190,66,143,92,62,97,71,174,62,15,92,41,63,117,194,143,61,184,81,236,62,235,133,31,63,12,204,205,63,25,153,154,62,174,20,123,62,97,71,174,63,35,215,10,63,48,163,215,62,138,61,113,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
	char SendEventBuf[] = { 80,67,76,83,48,48,48,52,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,0,25,RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 65, 0, 0, 0, 75, 0 };

	//char SendFacialBuf[] = { 80,67,76,83,48,48,48,53,0,0,0,0,0,0,0,0,50,48,50,51,48,56,48,50,49,55,53,52,52,53,0,0,0,96,  RecvBuf[34],RecvBuf[35],RecvBuf[36],RecvBuf[37],0,0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	
	/* event test */
	/*while (true)
	{
		send(EventSocket, SendEventBuf, sizeof(SendEventBuf), 0);
		cout << "Send Event Buf : " << SendEventBuf << endl;
		cout << "Send Event Len : " << sizeof(SendEventBuf) << endl;
		
		this_thread::sleep_for(1s);
	}*/


	/* facial test */
	/*while (true)
	{
		send(FacialSocket, SendFacialBuf, sizeof(SendFacialBuf), 0);
		cout << "Send Facial Buf : " << SendFacialBuf << endl;

		this_thread::sleep_for(1s);
	}*/


	// recv 
	SOCKET recvMotionSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN recvMotionAddr;
	ZeroMemory(&recvMotionAddr, sizeof(recvMotionAddr));
	recvMotionAddr.sin_family = AF_INET;
	recvMotionAddr.sin_port = htons(9999);
	recvMotionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(recvMotionSocket, (SOCKADDR*)&recvMotionAddr, sizeof(recvMotionAddr));
	listen(recvMotionSocket, SOMAXCONN);
	SOCKET recvMotion = accept(recvMotionSocket, NULL, NULL);
	if(recvMotion == INVALID_SOCKET)
		cout << "Accept Fail" << endl;

	cout << "Accept Success" << endl;


	FbxManager* manager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);
	FbxImporter* importer = FbxImporter::Create(manager, "");
	FbxScene* scene = FbxScene::Create(manager, "");


	while (true)
	{
		char recvMotionBuf[1024] = { 0, };
		recv(recvMotion, recvMotionBuf, sizeof(recvMotionBuf), 0);

		if (recvMotionBuf[0] == 1)
		{
			cout << "recvMotionBuf[1] == 1" << endl;
			const char* fbxFilePath = "C:/FBXtest/20230821_face_dance_anim_Anim.FBX";
			if (importer->Initialize(fbxFilePath, -1, manager->GetIOSettings()))
			{
				cout << "20230821_face_dance_anim.FBX" << endl;
				importer->Import(scene);
			}
			else
				return 0;
		}

		
		//thread motionThread(SendMotion, scene, MotionSocket, RecvBuf);
		thread facialThread(SendFacial, scene, FacialSocket, RecvBuf);

		//motionThread.join();
		facialThread.join();
	}

	


	scene->Destroy();
	importer->Destroy();
	manager->Destroy();


	return 0;
}
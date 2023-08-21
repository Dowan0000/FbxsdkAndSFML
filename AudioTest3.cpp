#include <iostream>
#include <thread>
#include <mutex>
#include <WinSock2.h>

#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-audio-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-network-d.lib")

using namespace std;
using namespace sf;


sf::SoundBufferRecorder recorder;
sf::Packet packet;

mutex m;
atomic<bool> canSend(false);

void TestFunc()
{
    while (true)
    {
        recorder.start();

        sf::sleep(sf::seconds(1.f));
        recorder.stop();
        
        const sf::SoundBuffer& soundBuffer = recorder.getBuffer();

        // Convert sound buffer data to an array of samples
        const sf::Int16* samples = soundBuffer.getSamples();
        std::size_t sampleCount = soundBuffer.getSampleCount();

        const sf::Uint8* bytes = reinterpret_cast<const sf::Uint8*>(samples);
        std::size_t byteCount = sampleCount * sizeof(sf::Int16);

        {
            lock_guard<mutex> lock(m);
            packet.append(bytes, byteCount);

        }
    }
    
}

int main_5()
{
    WSAData wasData;
    int Result = WSAStartup(MAKEWORD(2, 2), &wasData);

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN ServerAddr;
    ZeroMemory(&ServerAddr, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(6544); //20001
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
    
    listen(ListenSocket, SOMAXCONN);

    SOCKADDR_IN ClientAddr;
    int ClientAddrSize = sizeof(ClientAddr);
    SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

    cout << "Client connected" << endl;



    thread t1(TestFunc);

    while (true)
    {
        /*sf::SoundBufferRecorder recorder;
        recorder.start();

        sf::sleep(sf::seconds(1.f));
        recorder.stop();*/



        /*const sf::SoundBuffer& soundBuffer = recorder.getBuffer();


        // Convert sound buffer data to an array of samples
        const sf::Int16* samples = soundBuffer.getSamples();
        std::size_t sampleCount = soundBuffer.getSampleCount();

        if (isRecording == true)
            continue;

        if(sampleCount < 1)
            continue;

        // Convert the array of samples to a byte array
        const sf::Uint8* bytes = reinterpret_cast<const sf::Uint8*>(samples);
        std::size_t byteCount = sampleCount * sizeof(sf::Int16);

        // Create an sf::Packet and append the byte array
        sf::Packet packet;
        packet.clear();
        packet.append(bytes, byteCount);*/
        
        /*int sendBytes = send(ClientSocket, (char*)packet.getData(), packet.getDataSize(), 0);
        cout << "sendBytes : " << sendBytes << endl;*/
        
        {
            lock_guard<mutex> lock(m);
            if (packet.getDataSize() > 0)
            {
                int sendBytes = send(ClientSocket, (char*)packet.getData(), packet.getDataSize(), 0);
                packet.clear();

            }
            
        }

    }

    t1.join();

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
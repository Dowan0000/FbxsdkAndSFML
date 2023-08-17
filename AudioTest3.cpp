#include <iostream>
#include <thread>
#include <WinSock2.h>

#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sfml-audio-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-network-d.lib")

using namespace std;
using namespace sf;

int main() 
{
    WSAData wasData;
    int Result = WSAStartup(MAKEWORD(2, 2), &wasData);

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN ServerAddr;
    ZeroMemory(&ServerAddr, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(12345); //20001
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
    
    listen(ListenSocket, SOMAXCONN);

    SOCKADDR_IN ClientAddr;
    int ClientAddrSize = sizeof(ClientAddr);
    SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrSize);

    cout << "Client connected" << endl;


    while (true)
    {
        sf::SoundBufferRecorder recorder;
        recorder.start();

        this_thread::sleep_for(1s);

        recorder.stop();


        if(recorder.getBuffer().getSampleCount() < 1)
			continue;

        const sf::SoundBuffer& soundBuffer = recorder.getBuffer();


        // Convert sound buffer data to an array of samples
        const sf::Int16* samples = soundBuffer.getSamples();
        std::size_t sampleCount = soundBuffer.getSampleCount();

        // Convert the array of samples to a byte array
        const sf::Uint8* bytes = reinterpret_cast<const sf::Uint8*>(samples);
        std::size_t byteCount = sampleCount * sizeof(sf::Int16);

        // Create an sf::Packet and append the byte array
        sf::Packet packet;
        packet.append(bytes, byteCount);

        //// Create an sf::UdpSocket and send the packet
        //sf::UdpSocket socket;
        //sf::IpAddress remoteAddress = "127.0.0.1"; // Replace with the destination address
        //unsigned short remotePort = 12345; // Replace with the destination port
        //socket.send(packet, remoteAddress, remotePort);

        int sendBytes = send(ClientSocket, (char*)packet.getData(), packet.getDataSize(), 0);
        cout << "sendBytes : " << sendBytes << endl;
    }

    

    

    return 0;
}
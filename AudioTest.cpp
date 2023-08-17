#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>

#pragma comment(lib, "sfml-audio-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")

using namespace std;
using namespace sf;


int main_3()
{
    /*std::string fileName = "C:/AudioTest/Test1.wav";
    sf::Music musicPlayer;

    if (musicPlayer.openFromFile(fileName))
    {
        musicPlayer.play();
        
        while (musicPlayer.getStatus() != sf::Music::Stopped)
        {
            cout << "Playing..." << endl;

		}
    }*/


    if (!sf::SoundBufferRecorder::isAvailable())
    {
		std::cout << "Sorry, audio capture is not supported by your system" << std::endl;
		return EXIT_FAILURE;
	}

    sf::SoundBufferRecorder recorder;

    cout << "Start recording..." << endl;
    recorder.start();

    //TODO
    this_thread::sleep_for(10s);

    cout << "Stop recording..." << endl;
    recorder.stop();

    const sf::SoundBuffer& buffer = recorder.getBuffer();

    buffer.saveToFile("C:/AudioTest/recorder.wav");
    
    Sound sound(buffer);
    sound.play();
    while (true)
        ;

    return 0;
}

#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>

using namespace std;

class MyRecorder : public sf::SoundRecorder {
public:
    virtual bool onStart() {
        std::cout << "Recording started!" << std::endl;
        return true;
    }

    virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) {
        m_buffer.insert(m_buffer.end(), samples, samples + sampleCount);
        return true;
    }

    virtual void onStop() {
        std::cout << "Recording stopped!" << std::endl;
    }

    const std::vector<sf::Int16>& getBuffer() const {
        return m_buffer;
    }

private:
    std::vector<sf::Int16> m_buffer;
};

int main_4() 
{
    // Create an instance of your custom recorder class
    MyRecorder recorder;

    // Start recording audio
    if (!recorder.start()) {
        std::cerr << "Recording failed!" << std::endl;
        return 1;
    }

    // Let the recording run for a certain duration (e.g., 5 seconds)
    //sf::sleep(sf::seconds(5));


    this_thread::sleep_for(5s);


    // Stop recording
    recorder.stop();

    // Get the recorded audio buffer
    const std::vector<sf::Int16>& audioBuffer = recorder.getBuffer();

    // Save the recorded audio buffer to a WAV file
    sf::SoundBuffer soundBuffer;
    soundBuffer.loadFromSamples(&audioBuffer[0], audioBuffer.size(), 1, 44100);
    soundBuffer.saveToFile("C:/AudioTest/recorded_audio.wav");

    std::cout << "Recording saved as 'recorded_audio.wav'" << std::endl;

    return 0;
}
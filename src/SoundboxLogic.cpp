#include "SoundboxLogic.h"

// WAV playback objects
AudioGeneratorWAV* wav = nullptr;
AudioOutputI2S* out = nullptr;
AudioFileSourceSD* file = nullptr;
AudioFileSourceBuffer* buffer = nullptr;

// Generate the audio file playlist based on the number
std::vector<String> generateFileBuffer(int number) {
    std::vector<String> buffer;
    buffer.push_back("/Soundbox/Received.wav"); // Add default received sound

    if (number == 0) {
        buffer.push_back("/Soundbox/0.wav");
    } else {
        if (number >= 1000) {
            int thousands = number / 1000;
            buffer.push_back("/Soundbox/" + String(thousands) + ".wav");
            buffer.push_back("/Soundbox/1000.wav");
            number %= 1000;
        }

        if (number >= 100) {
            int hundreds = number / 100;
            buffer.push_back("/Soundbox/" + String(hundreds) + ".wav");
            buffer.push_back("/Soundbox/100.wav");
            number %= 100;
        }

        if (number >= 20) {
            int tens = (number / 10) * 10; // Find tens place
            buffer.push_back("/Soundbox/" + String(tens) + ".wav");
            number %= 10;
        }

        if (number > 0) {
            buffer.push_back("/Soundbox/" + String(number) + ".wav");
        }
    }

    return buffer;
}

// Play audio files sequentially
void playAudioFiles(const std::vector<String>& fileBuffer) {
    for (const auto& filename : fileBuffer) {
        if (wav) {
            wav->stop();
            delete wav;
            delete buffer;
            delete file;
        }

        Serial.printf("🎵 Playing: %s\n", filename.c_str());

        file = new AudioFileSourceSD(filename.c_str());
        if (!file->isOpen()) {
            Serial.printf("❌ Failed to open file: %s\n", filename.c_str());
            continue;
        }

        buffer = new AudioFileSourceBuffer(file, 2048);
        wav = new AudioGeneratorWAV();
        if (!wav->begin(buffer, out)) {
            Serial.println("❌ Failed to start WAV decoder");
            continue;
        }

        while (wav->isRunning()) {
            wav->loop(); // Play the current file
        }
    }
}

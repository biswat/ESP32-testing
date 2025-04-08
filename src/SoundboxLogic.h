#ifndef SOUNDBOX_LOGIC_H
#define SOUNDBOX_LOGIC_H

#include "Audio.h"
#include "FS.h"
#include "SD.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include <vector>
#include <Arduino.h>

// WAV playback objects
extern AudioGeneratorWAV* wav;
extern AudioOutputI2S* out;
extern AudioFileSourceSD* file;
extern AudioFileSourceBuffer* buffer;

// Function to generate the audio file playlist based on the number
std::vector<String> generateFileBuffer(int number);

// Function to play audio files sequentially
void playAudioFiles(const std::vector<String>& fileBuffer);

#endif // SOUNDBOX_LOGIC_H

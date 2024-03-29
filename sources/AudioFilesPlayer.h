/*
  ==============================================================================

    AudioFilesPlayer.h
    Created: 16 Jun 2019 12:17:38pm
    Author:  David Hill

  ==============================================================================
*/

#pragma once

#include "AudioFilePlayer.h"
#include "../JuceLibraryCode/JuceHeader.h"

namespace hill_AudioTools
{
    class AudioFilesPlayer : private DeletedAtShutdown
    {
    public:

#if ENABLE_SINGLETON_PLAYER
        JUCE_DECLARE_SINGLETON(AudioFilesPlayer, true)
#endif
        
        AudioFilesPlayer();
        ~AudioFilesPlayer();

        AudioFilePlayer* loadFile(const void* sourceData,
                      size_t sourceDataSize,
                      bool keepInternalCopyOfData, String name);
        AudioFilePlayer* loadFile(const File& file, const String& name);

        void playFile(String name);
        void playFileWithId(int ID);
        void playFileWithIndex(int index);
        
        void stopFile(String name);
        void stopFileWithId(int ID);
        void stopFileWithIndex(int index);
        
        void playAll();
        void stopAll();

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
        void releaseResources();
        void process(AudioBuffer<float>& buffer);
        
        AudioFilePlayer* getFile(String name);
        AudioFilePlayer* getFileById(int ID);
        AudioFilePlayer* getFileByIndex(int index);
        
    private:
        
        CriticalSection cs;
        OwnedArray<AudioFilePlayer> audioFiles;
        
        int samplesPerBlockExpected {0};
        double sampleRate {0.0};
    };
}

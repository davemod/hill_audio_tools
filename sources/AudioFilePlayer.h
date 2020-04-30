/*
  ==============================================================================

    AudioPlayer.h
    Created: 11 Jun 2019 11:10:53am
    Author:  David Hill

  ==============================================================================
*/

#pragma once

#include "GainCtrl.h"

#include "../JuceLibraryCode/JuceHeader.h"

namespace juce
{
    class AudioFilePlayer
    {
      
    public:
        
        enum State
        {
            idle,
            playing,
            stopping
        };
        
        AudioFilePlayer(String name = "");

        AudioFilePlayer(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData, String name = "", int index = -1);
        
        ~AudioFilePlayer();

        void play();
        void stop();
        
        State getState();
        bool isActive();
        
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
        void releaseResources();
        void process(AudioBuffer<float>& buffer);
        
        void loadFile(const void* sourceData,
                      size_t sourceDataSize,
                      bool keepInternalCopyOfData);
        
        void setGain(float gain, int time = 0);
        
        void setIndex(int index);
        void setName(String name);
        
        void setLoop (bool b) { loop = b; }
        bool isLoop () { return loop; }
        
        int getIndex();
        int getID();
        
        String getName();
        
    private:
        
        Atomic<State> state;
                
        int index = -1;
        int ID = -1;
        
        AudioFormatManager afm;
        ScopedPointer<AudioFormatReader> reader;
        
        int samplesPerBlockExpected;
        double sampleRate;
        
        int currentPlayBackSample = 0;

        bool loop = false;
        
        ADSR adsr;
        GainCtrl gain;
        
        String name = "";

        void    initialize();
        void    initLagrangeInterpolators(int numChannels);
        
        OwnedArray<LagrangeInterpolator> lris;
        
        AudioDeviceManager * deviceManager;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlayer)

        
    };
}

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

namespace hill_AudioTools
{
    using namespace juce;

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
        AudioFilePlayer(const File& file, const String name = "", int index = -1);
        
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
        void loadFile(const File& file);
        
        void setGain(float gain, int time = 0);
        
        void setLoop (bool b) { loop = b; }
        bool getLoop () { return loop.get(); }
    
        void setIndex (int i) { index = i; }
        int getIndex() { return index; }
            
        void setName(String newName) { name = newName; }
        String getName() { return name; }
        
        int getID() { return ID; }
    
    private:
        
        AudioFormatManager afm;
        std::unique_ptr<AudioFormatReader> reader;
        
        AudioBuffer<float> readBuffer {2,2048};
        CriticalSection cs;
        
        Atomic<State> state;
                
        String name = "";
        int index = -1;
        int ID = -1;
                
        Atomic<int> samplesPerBlockExpected;
        Atomic<double> sampleRate;
        Atomic<bool> loop {false};
        
        ADSR adsr;
        ADSR::Parameters par;
        void setAdsrParamters ();

        GainCtrl gain;
        
        int64 currentPlayBackSample = 0;
        
        OwnedArray<LagrangeInterpolator> lris;

        void initialize();
        void initLagrangeInterpolators(int numChannels);
        void initReaderBuffer (int numSamples = 2048);
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlayer)
    };
}

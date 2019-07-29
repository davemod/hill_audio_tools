/*
  ==============================================================================

    GainCtrl.h
    Created: 16 Jun 2019 9:22:44am
    Author:  David Hill

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace juce
{
    class GainCtrl
    {

    public:
        
        GainCtrl();
        ~GainCtrl();
        
        void setGain(float gain_, float time = 0);
        
        void setSampleRate(double sampleRate);
        
        void applyToBuffer (AudioBuffer<float>& buffer, int startSample, int numSamples);

        float getGain();
        
    private:

        double  sr = 0;
        float   gain = 1.f;
        float   newGain = 1.f;
        
        float   gainRate = 0.f;
        
        void    calculateGain();
        
    };
}

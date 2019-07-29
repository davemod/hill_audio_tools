/*
  ==============================================================================

    GainCtrl.cpp
    Created: 16 Jun 2019 9:22:44am
    Author:  David Hill

  ==============================================================================
*/

#include "GainCtrl.h"

GainCtrl::GainCtrl()
{
    sr = 44100;
}

GainCtrl::~GainCtrl()
{
    
}

void GainCtrl::setGain(float gain_, float time)
{
    jassert (time >= 0);
    
    newGain = gain_;
    
    if (time == 0.f)
        gain = newGain;
    
    gainRate = time > 0 ? (newGain - gain) / (sr * time) : newGain - gain;
    
    DBG(gainRate);
}

void GainCtrl::setSampleRate(double sampleRate)
{
    sr = sampleRate;
}

void GainCtrl::applyToBuffer (AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    jassert (startSample + numSamples <= buffer.getNumSamples());
    
    auto numChannels = buffer.getNumChannels();
    
    while (--numSamples >= 0)
    {
        /*gain=*/ calculateGain();
        
        for (int i = 0; i < numChannels; ++i)
            buffer.getWritePointer (i)[startSample] *= gain;
        
        ++startSample;
    }
}

void GainCtrl::calculateGain()
{
    if ((gainRate > 0 && gain < newGain) || (gainRate < 0 && gain > newGain))
        gain += gainRate;
    else
        gain = newGain;
    
}

float GainCtrl::getGain()
{
    return gain;
}




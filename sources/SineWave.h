/*
  ==============================================================================

    SineWave.h
    Created: 4 May 2019 5:47:30pm
    Author:  David

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace juce
{
    class SineWave
    {
    public:
        SineWave()
        {

        }

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate)
        {
            currentSampleRate = sampleRate;
            updateAngleDelta();
            adsr.setSampleRate(sampleRate);
            
            ADSR::Parameters par;
            par.attack = 0.02;
            par.decay = 0.02;
            par.sustain = 0.8;
            par.release = 0.02;
            adsr.setParameters(par);
        }

        void process(AudioBuffer<float>& buffer)
        {
            auto* leftBuffer = buffer.getWritePointer(0, 0);
            auto* rightBuffer = buffer.getWritePointer(1, 0);

            auto localTargetFrequency = targetFrequency;
            if (localTargetFrequency != currentFrequency)                                                              // [7]
            {
                auto frequencyIncrement = (localTargetFrequency - currentFrequency) / buffer.getNumSamples();         // [8]
                for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    auto currentSample = (float)std::sin(currentAngle);
                    currentFrequency += frequencyIncrement;                                                            // [9]
                    updateAngleDelta();                                                                                // [10]
                    currentAngle += angleDelta;
                    leftBuffer[sample] = currentSample;
                    rightBuffer[sample] = currentSample;
                }
                currentFrequency = localTargetFrequency;
            }
            else                                                                                                       // [11]
            {
                for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    auto currentSample = (float)std::sin(currentAngle);
                    currentAngle += angleDelta;
                    leftBuffer[sample] = currentSample;
                    rightBuffer[sample] = currentSample;
                }
            }

            adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());
        }

        void releaseResources()
        {

        }

        void playBeep(int milliseconds)
        {
            adsr.noteOn();
            Timer::callAfterDelay(milliseconds, [&]() {adsr.noteOff(); });
        }
        
        void playBeep(float freq, int milliseconds)
        {
            targetFrequency = freq;
            playBeep(milliseconds);
        }
        
    private:

        void updateAngleDelta()
        {
            auto cyclesPerSample = currentFrequency / currentSampleRate;         // [2]
            angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;                // [3]
        }

        ADSR adsr;

        double currentAngle = 0;
        double angleDelta = 0;
        double currentSampleRate = 0;
        float currentFrequency = 0.f;
        float targetFrequency = 440.f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWave)
    };
}

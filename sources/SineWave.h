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
            targetFrequency = 440;
            currentSampleRate = sampleRate;
            updateAngleDelta();
            
            ADSR::Parameters par;
            
            par.attack = 0.1;
            par.decay = 0.1;
            par.sustain = 0.9;
            par.release = 0.1;
            
            adsr.setSampleRate(sampleRate);
            adsr.setParameters(par);
        }
        
        void start()
        {
            adsr.noteOn();
        }
        
        void stop()
        {
            adsr.noteOff();
        }
        
        void process(AudioBuffer<float>& buffer)
        {
            {
                auto localTargetFrequency = targetFrequency.get();

                /** this is based on https://docs.juce.com/master/tutorial_sine_synth.html */
                if (localTargetFrequency != currentFrequency)                                                         // [7]
                {
                    auto frequencyIncrement = (localTargetFrequency - currentFrequency) / buffer.getNumSamples();    // [8]
                   
                    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
                    {
                        auto currentSample = (float) std::sin (currentAngle);
                        currentSample *= adsr.getNextSample();
                        
                        currentFrequency += frequencyIncrement;
                        updateAngleDelta();
                        
                        currentAngle += angleDelta;
                        
                        for (int i = buffer.getNumChannels(); --i >= 0;)
                        {
                            buffer.addSample(i, sample, currentSample);
                        }
                    }
                                    
                    currentFrequency = localTargetFrequency;
                }
                else
                {
                    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
                    {
                        auto currentSample = (float) std::sin (currentAngle);
                        currentSample *= adsr.getNextSample();
                        
                        currentAngle += angleDelta;
                        for (int i = buffer.getNumChannels(); --i >= 0;)
                        {
                            buffer.addSample(i, sample, currentSample);
                        }
                    }
                }
            }
        }
        
        void updateAngleDelta ()
        {
            /** this is based on  https://docs.juce.com/master/tutorial_sine_synth.html */
            auto cyclesPerSample = targetFrequency.get() / currentSampleRate;         // [2]
            angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
        }

        void releaseResources()
        {
            
        }

        void playBeep(int milliseconds)
        {
            adsr.noteOn();
            
            if (milliseconds > 0)
                Timer::callAfterDelay(milliseconds, [&]() {adsr.noteOff(); });
        }
        
    private:

        CriticalSection cs;

        ADSR adsr;
        
        Atomic<double> targetFrequency = 440.0;
        double currentFrequency = 440.0;
        double currentAngle = 0.0;
        double angleDelta = 0.0;
        
        double currentSampleRate;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWave)
    };
}

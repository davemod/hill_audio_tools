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

            m_amplitude = 0.5;
            m_frequency = 440;
            m_phase = 0.0;
            m_time = 0.0;
            m_deltaTime = 1 / sampleRate;
            
            ADSR::Parameters par;
            
            par.attack = 0.02;
            par.decay = 0.02;
            par.sustain = 1;
            par.release = 0.02;
            
            adsr.setParameters(par);
            
            isPrepared.set(true);
        }
        
        void start()
        {
            ScopedLock sl (cs);
            adsr.noteOn();
        }
        
        void stop()
        {
            ScopedLock sl (cs);
            adsr.noteOff();
        }
        
        void process(AudioBuffer<float>& buffer)
        {
            
            ScopedLock sl (cs);
  
            if (m_time >= std::numeric_limits<float>::max()) {
                m_time = 0.0;
            }
            
            float monoBuffer [buffer.getNumSamples()];
            
            // generate sin wave in mono
            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
                
                float value = m_amplitude * sin(2 * double_Pi * m_frequency * m_time + m_phase);
                value *= adsr.getNextSample();
                
                monoBuffer[sample] = value;
                m_time += m_deltaTime;
            }
            
            for (int chan = 0; chan < buffer.getNumChannels(); chan++)
            {
                buffer.copyFrom(chan, 0, monoBuffer, buffer.getNumSamples());
            }
        }

        void releaseResources()
        {

        }

        void playBeep(int milliseconds)
        {
            adsr.noteOn();
            Timer::callAfterDelay(milliseconds, [&]() {adsr.noteOff(); });
        }
        
    private:

        CriticalSection cs;
        Atomic<bool> isPrepared = false;

        ADSR adsr;

        float m_amplitude;
        float m_frequency;
        float m_phase;
        float m_time;
        float m_deltaTime;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWave)
    };
}

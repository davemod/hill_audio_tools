/*
  ==============================================================================

    AudioPlayer.cpp
    Created: 11 Jun 2019 11:10:53am
    Author:  David Hill

  ==============================================================================
*/

#include "AudioFilePlayer.h"

namespace hill_AudioTools
{

AudioFilePlayer::AudioFilePlayer(String name)
{
    this->name = name;
    initialize();
}

AudioFilePlayer::AudioFilePlayer(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData, String name, int index)
{
    this->name = name;
    this->index = index;
    
    initialize();
    
    loadFile(sourceData, sourceDataSize, keepInternalCopyOfData);
}


AudioFilePlayer::AudioFilePlayer(const File& file, String name, int index)
{
    
    this->name = name;
    this->index = index;
    
    initialize();
    
    loadFile(file);
}


AudioFilePlayer::~AudioFilePlayer()
{
    
}

void AudioFilePlayer::play() {
    state = playing;
    adsr.noteOn();
}

void AudioFilePlayer::stop() {
    state = stopping;
    adsr.noteOff();
}

AudioFilePlayer::State AudioFilePlayer::getState()
{
    return state.get();
}

bool AudioFilePlayer::isActive()
{
    return state.get() != idle;
}

void AudioFilePlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    
    this->samplesPerBlockExpected = samplesPerBlockExpected;
    this->sampleRate = sampleRate;
    
    adsr.setSampleRate(sampleRate);
    gain.setSampleRate(sampleRate);
    
    for (auto interpolator : lris)
    {
        interpolator->reset();
    }
    
    adsr.setParameters(par);
}

void AudioFilePlayer::releaseResources()
{
    lris.clear();
    readBuffer.clear();
}

void AudioFilePlayer::process(AudioBuffer<float>& buffer) {
    
    if (sampleRate.get() <= 0)
        return;
    
    if (buffer.getNumChannels() != lris.size())
    {
        initLagrangeInterpolators(buffer.getNumChannels());
    }
    
    // lagrange interpolation
    
    if (cs.tryEnter())
    {
        if (reader && state.get() != idle)
        {
            auto sr_play = sampleRate.get();
            
            auto sr_file = reader.get()->sampleRate;
            
            auto ratio = sr_file / sr_play;
            
            auto numSamplesToRead = (int)(buffer.getNumSamples() * ratio);
            
            if (readBuffer.getNumSamples() < numSamplesToRead)
            {
                initReaderBuffer(numSamplesToRead + 20);
            }
        
            reader->read(&readBuffer, 0, numSamplesToRead, currentPlayBackSample, true, true);
            
            adsr.applyEnvelopeToBuffer(readBuffer, 0, readBuffer.getNumSamples());
            gain.applyToBuffer(readBuffer, 0, readBuffer.getNumSamples());

            for (int chan = 0; chan < buffer.getNumChannels(); chan++)
            {
                if (chan < readBuffer.getNumChannels())
                {
                    auto lri = lris[chan]; // als member

                    lri->processAdding(ratio,
                                       readBuffer.getReadPointer(chan),
                                       buffer.getWritePointer(chan),
                                       buffer.getNumSamples(),
                                       numSamplesToRead,
                                       true,
                                       1.0f);
                }
            }
            
            int64 lastPlayBackSample = currentPlayBackSample;
            
            currentPlayBackSample = (currentPlayBackSample + (int64)numSamplesToRead) % reader->lengthInSamples;
            
            if (!loop.get() && lastPlayBackSample > currentPlayBackSample)
                state = idle;
        }
        
        if (state.get() == stopping && !adsr.isActive())
            state = idle;
        
        cs.exit();
    }
}

void AudioFilePlayer::loadFile(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData) {
    
    std::unique_ptr<MemoryInputStream> stream = std::make_unique<MemoryInputStream>( sourceData, sourceDataSize, keepInternalCopyOfData );
    
    if (auto* newreader = afm.createReaderFor(std::make_unique<MemoryInputStream>(sourceData, sourceDataSize, keepInternalCopyOfData)))
    {
        ScopedLock sl {cs};
        reader.reset (newreader);
        initReaderBuffer();
    }
    else
    {
        /** could not create a reader for this data */
        jassertfalse;
    }
}

void AudioFilePlayer::loadFile(const File& file)
{
    if (file.existsAsFile())
    {
        if (auto* newReader = afm.createReaderFor(file))
        {
            ScopedLock sl {cs};
            reader.reset (newReader);
            initReaderBuffer ();
        }
        else
        {
            /** Could not create a reader for this file */
            jassertfalse;
        }
    }
}

void AudioFilePlayer::setGain(float gain_, int time)
{
    gain.setGain(gain_, time);
}

void AudioFilePlayer::initialize()
{
    static int id_ = 0;
    ID = id_++;
    
    afm.registerBasicFormats();
    
    par.attack = 0.02;
    par.decay = 0.02;
    par.sustain = 1.;
    par.release = 0.02;
    
    adsr.setParameters(par);
    
    gain.setGain(1.f);
}

void AudioFilePlayer::initLagrangeInterpolators(int numChannels)
{
    lris.clear();
    for (int i = 0; i < numChannels; i++)
    {
        auto lri = lris.add(new LagrangeInterpolator());
        lri->reset();
    }
}

void AudioFilePlayer::initReaderBuffer (int numSamples)
{
    // first initialize the reader with a file or audio data using loadFile
    jassert (reader);
    
    if (reader)
    {
        readBuffer.setSize (reader->getChannelLayout ().size(), numSamples);
        
    }
}

}

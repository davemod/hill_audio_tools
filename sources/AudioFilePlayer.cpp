/*
  ==============================================================================

    AudioPlayer.cpp
    Created: 11 Jun 2019 11:10:53am
    Author:  David Hill

  ==============================================================================
*/

#include "AudioFilePlayer.h"

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




AudioFilePlayer::~AudioFilePlayer()
{
    
}

void AudioFilePlayer::play() {
    playing = true;
    adsr.noteOn();
}

void AudioFilePlayer::stop() {
    adsr.noteOff();
    float releaseTime = adsr.getParameters().release;
    std::unique_ptr<AudioFilePlayer> instance(this);
    Timer::callAfterDelay(releaseTime*1000, [&](){if(instance) playing = false; });
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
}

void AudioFilePlayer::releaseResources() {
}

void AudioFilePlayer::process(AudioBuffer<float>& buffer) {
    
    if (sampleRate <= 0)
        return;
    
    if (buffer.getNumChannels() != lris.size())
    {
        initLagrangeInterpolators(buffer.getNumChannels());
    }
    
    // lagrange interpolation
    
    if (reader && playing)
    {
        auto sr_play = sampleRate;
        
        
        jassert (reader.get() != nullptr);
        
        auto sr_file = reader.get()->sampleRate;
        
        auto ratio = sr_file / sr_play;
        
//      ohne lagrange interpolation
//        auto numSamplesToRead = buffer.getNumSamples();
        
        auto numSamplesToRead = (int)(buffer.getNumSamples() * ratio);
        
        AudioBuffer<float> readBuffer = AudioBuffer<float>(buffer.getNumChannels(), numSamplesToRead);
    
        reader->read(&readBuffer, 0, numSamplesToRead, currentPlayBackSample, true, true);
        
        
        for (int chan = 0; chan < buffer.getNumChannels(); chan++){
        
            auto lri = lris[chan]; // als member

            lri->process(ratio, readBuffer.getReadPointer(chan), buffer.getWritePointer(chan), buffer.getNumSamples(),numSamplesToRead,1);
        }
        
        int lastPlayBackSample = currentPlayBackSample;
        
        currentPlayBackSample = ((int)currentPlayBackSample + (int)numSamplesToRead) % reader->lengthInSamples;
        
        if (!loop && lastPlayBackSample > currentPlayBackSample)
            playing = false;
    }
    
    adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());
    gain.applyToBuffer(buffer, 0, buffer.getNumSamples());
}

void AudioFilePlayer::loadFile(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData) {
    
    auto* stream = new MemoryInputStream ( sourceData, sourceDataSize, keepInternalCopyOfData );
    
    if (auto* newreader = afm.createReaderFor(stream))
    {
        reader = newreader;
    }
    else
    {
        jassertfalse;
    }
}

void AudioFilePlayer::setGain(float gain_, int time)
{
    gain.setGain(gain_, time);
}

void AudioFilePlayer::setIndex(int index)
{
    this->index = index;
}

void AudioFilePlayer::setName(String name)
{
    this->name = name;
}

int AudioFilePlayer::getIndex()
{
    return index;
}

int AudioFilePlayer::getID()
{
    return ID;
}

String AudioFilePlayer::getName()
{
    return name;
}


void AudioFilePlayer::initialize()
{
    static int id_ = 0;
    ID = id_++;
    
    afm.registerBasicFormats();
    
    ADSR::Parameters par;
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
        lris.add(new LagrangeInterpolator());
    }
}

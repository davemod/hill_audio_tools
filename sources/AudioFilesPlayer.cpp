/*
  ==============================================================================

    AudioFilesPlayer.cpp
    Created: 16 Jun 2019 12:17:38pm
    Author:  David Hill

  ==============================================================================
*/

#include "AudioFilesPlayer.h"


JUCE_IMPLEMENT_SINGLETON(AudioFilesPlayer)

AudioFilesPlayer::AudioFilesPlayer()
{
    sampleRate = 44100;
}

AudioFilesPlayer::~AudioFilesPlayer()
{
    clearSingletonInstance();
}

AudioFilePlayer* AudioFilesPlayer::loadFile(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData, String name)
{
    // call prepareToPlay() first
    jassert (sampleRate>0);
    
    auto file = audioFiles.add(new AudioFilePlayer(sourceData, sourceDataSize,keepInternalCopyOfData, name, audioFiles.size()));
    
    file->prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    return file;
}





void AudioFilesPlayer::playFile(String name)
{
    for (auto file : audioFiles)
        if (file->getName() == name)
            file->play();
}

void AudioFilesPlayer::playFileWithId(int ID)
{
    for (auto file : audioFiles)
        if (file->getID() == ID)
            file->play();
}
void AudioFilesPlayer::playFileWithIndex(int index)
{
    for (auto file : audioFiles)
        if (file->getIndex() == index)
            file->play();
}
void AudioFilesPlayer::stopFile(String name)
{
    for (auto file : audioFiles)
        if (file->getName() == name)
            file->stop();
}
void AudioFilesPlayer::stopFileWithId(int ID)
{
    for (auto file : audioFiles)
        if (file->getID() == ID)
            file->stop();
}
void AudioFilesPlayer::stopFileWithIndex(int index)
{
    for (auto file : audioFiles)
        if (file->getIndex() == index)
            file->stop();
}
void AudioFilesPlayer::playAll()
{
    for (auto file : audioFiles)
        file->play();
}
void AudioFilesPlayer::stopAll()
{
    for (auto file : audioFiles)
        file->stop();
}



void AudioFilesPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlockExpected = samplesPerBlockExpected;
}

void AudioFilesPlayer::releaseResources() {
}

void AudioFilesPlayer::process(AudioBuffer<float> &buffer) {
    
    ScopedLock sl (cs);
    jassert (sampleRate > 0);
    
    AudioBuffer<float> musicBuffer;
    musicBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    musicBuffer.clear();
    
    for (auto file : audioFiles)
    {
        file->process(musicBuffer);
    }
    
    for (int i = 0; i < musicBuffer.getNumChannels(); i++)
    {
        buffer.addFrom(i, 0, musicBuffer, i, 0, musicBuffer.getNumSamples());
    }

}

AudioFilePlayer* AudioFilesPlayer::getFile(String name)
{
    for (auto file : audioFiles)
    {
        if (file->getName() == name)
        {
            return file;
        }
    }
    return nullptr;
}

AudioFilePlayer* AudioFilesPlayer::getFileById(int ID)
{
    for (auto file : audioFiles)
    {
        if (file->getID() == ID)
        {
            return file;
        }
    }
    return nullptr;
}

AudioFilePlayer* AudioFilesPlayer::getFileByIndex(int index)
{
    for (auto file : audioFiles)
    {
        if (file->getIndex() == index)
        {
            return file;
        }
    }
    return nullptr;
}

